#include"semantic.h"
char* TypeKindStr[]={"intTy","charTy","boolTy","arrayTy"};
char* AccessKindStr[]={"dir","indir"};
char* IdKindStr[]={"varkind","typekind","prockind"};
typeIR* IntTy,*CharTy,*BoolTy;
bool SymTable::insertNode(SymTableNode* ptr)
{
	if(ptr==NULL)
		return false;
	if(ptr->attrIR.kind!=prockind) 
		ptr->EOFL=0;
	ptr->next=0;
	if(ptr->attrIR.kind==prockind)//���������
	{
		int l=ptr->attrIR.level;
		if(l>maxLayer)
			maxLayer=l;
	}
	if(ptr->attrIR.kind==varkind)
	{
		int l=ptr->attrIR.level;
		if(l>maxLayer)
			maxLayer=l;
	}
	if(front==NULL)
	{
		front=rear=ptr;
	}
	else
	{
		rear->next=ptr;
		rear=rear->next;
	}
	return true;
}
bool SymTable::OutToCmd()
{
	if(front==NULL)
		return 0;
	SymTableNode* ptr;
	for(int i=0;i<=maxLayer;i++)
	{
		cout<<"--------------------------SymTable in Layer "<<i<<"--------------------------"<<endl;
		ptr=front;
		while(ptr)//��δ�������ű�δβ
		{
			if((ptr->attrIR.kind!=prockind&&ptr->attrIR.level==i)||(ptr->attrIR.kind==prockind&&ptr->attrIR.level-1==i))
			{
				if(ptr->attrIR.kind==typekind)	   //����������� 
					printf("%8s:  %8s    %8s\n",ptr->name,TypeKindStr[ptr->attrIR.idtype->kind],IdKindStr[ptr->attrIR.kind]);
				else if(ptr->attrIR.kind==prockind)//������ǹ���
					printf("%8s:  %8s    Level:%2d    Noff: %2d\n",ptr->name,IdKindStr[ptr->attrIR.kind],ptr->attrIR.level,ptr->attrIR.More.ProcAttr.size);
				else if(ptr->attrIR.kind==varkind) //������Ǳ���
					printf("%8s:  %8s    %8s    Level: %2d Offset:%2d   %s\n",
					ptr->name,TypeKindStr[ptr->attrIR.idtype->kind],IdKindStr[ptr->attrIR.kind],
					ptr->attrIR.level,ptr->attrIR.More.VarAttr.offset
					,AccessKindStr[ptr->attrIR.More.VarAttr.access]);
			}
			ptr=ptr->next;
		}
	}
	return 1;
}
bool Scope::newLayer(SymTableNode* ptr)
{
	ScopeNode* nptr=new ScopeNode;//����õ�һ���µĽ�㣬���ݴ�ÿһ���ͷָ����Ϣ
	nptr->front=ptr;//ָ����ű��е�һ����
	if(scope.empty())//�ҵ�����ָ��
		nptr->parent=NULL;
	else
		nptr->parent=scope.top();
	scope.push(nptr);//ѹջ
	return true;
}
bool Scope::DropLayer()
{
	if(scope.empty())
		return false;
	ScopeNode* oPtr=scope.top();
	scope.pop();
	delete oPtr;
	return true;
}
SymTableNode* Scope::FindID(const char* idname,bool ntype)//�ӷ��ű�ջ�в����Ѿ������ı�ʶ��
/*ntype==1ʱ�������в�
**ntype==0ʱ�������Լ����ڲ�
**���ڲ�ͬ��������÷��ǲ�ͬ�ģ�����˵���ڱ���������������õ���ntype=0��
**���൱�������Ӳ��ظ�����ͬ�����Ƶı���
**�����ڳ��������飬��Ӧ��ʹntyp=0�Ӷ��������еĲ㣬
**�Ա�֤����δ�����ı��������ϲ�����Ҳ�������
**/
{
	ScopeNode * ptr=scope.top();
	if(scope.empty())
		return NULL;
	while(ptr)
	{
		SymTableNode* nptr=ptr->front;
		while(nptr)//�ӵ�ǰ���ң����������ű�β����ѭ��
		{
			if(strcmp(nptr->name,idname)==0)
				return nptr;
			if(nptr->EOFL)
				break;
			nptr=nptr->next;
		}
		if(ntype)
			ptr=ptr->parent;
		else
			break;
	}
	return NULL;
}
void Semantic::initTy()//��ʼ��SNL���Ի�������
{
	IntTy=new typeIR; 
	CharTy=new typeIR;
	BoolTy=new typeIR;

	//��������
	IntTy->kind=intTy;
	IntTy->size=INTSIZE;
    //�ַ�����
	CharTy->kind=charTy;
	CharTy->size=CHARSIZE;
    //��������
	BoolTy->kind=boolTy;
	BoolTy->size=BOOLSIZE;
}
ParseNode* Semantic::SearchChildFirst(ParseNode* ptr,int sym,bool ntype)
{
	return parseTree->SearchChildFirst(ptr,sym,ntype);
}
ParseNode* Semantic::SearchBrotherFirst(ParseNode* ptr,int sym,bool ntype)//
{
	return parseTree->SearchBrotherFirst(ptr,sym,ntype);
}
bool Semantic::SemScanner()
{
	if(parseTree==NULL)
		return 0;
	_Program(parseTree->root);//��ʼ
	return 1;
}
void Semantic::_Program(ParseNode* ptr)//�ܳ���
{
	ParseNode* p;//�ݴ�ָ��
	if((p=SearchBrotherFirst(ptr->child,DeclarePart,0)))//��������
	{
		myScope.newLayer(0);//�½���һ�㣬����ʼ���ֵ��������ַ����γ�һ�²�
		int offset=OFFSET;//ƫ��
		_DeclarePart(p,0,offset);
	}
	if((p=SearchBrotherFirst(ptr->child,ProgramBody,0)))//�����岿��
		_ProgramBody(p);
}
void Semantic::_DeclarePart(ParseNode* ptr,int layer,int &offset)//�����������
{
	ParseNode* p;
	if((p=SearchChildFirst(ptr->child,TypeDecpart,0)))//������������
		_TypeDecpart(p,layer);
	if((p=SearchBrotherFirst(ptr->child,VarDecpart,0)))//����������
		_VarDecpart(p,layer,offset);
	if((p=SearchBrotherFirst(ptr->child,ProcDecpart,0)))//��������
		if(SearchChildFirst(p,ProcDecPart,1))
			_ProcDecpart(p,layer+1);//����ǹ����������������һ
}
void Semantic::_ProgramBody(ParseNode* ptr)//�����岿��
{
	ParseNode* p;
	if((p=SearchBrotherFirst(ptr->child,StmList)))//�õ���������﷨�����
		_StmList(p);
}
void Semantic::_TypeDecpart(ParseNode* ptr,int layer)//��������----�γɷ��ű�������Ϣͬʱ���ɣ�
{
	ParseNode* p;
	if(ptr->child&&ptr->child->parse.Symbol==-1)//�������������
		return ;
	if((p=SearchChildFirst(ptr->child,TypeDecList,1)))//����������
		_TypeDecList(p,layer);
}
void Semantic::_VarDecpart(ParseNode* ptr,int layer,int &offset)//��������----�γɷ��ű�ע�������ƫ�Ƶ�ַ����Ϣ������
{
	ParseNode* p;
	if(ptr->child&&ptr->child->parse.Symbol==-1)//�ޱ�������
		return;
	if((p=SearchChildFirst(ptr->child,VarDecList,1)))//�б�������
		_VarDecList(p,layer,offset);
}
void Semantic::_VarDecList(ParseNode* ptr,int layer,int &offset)//���������б�
{
	ParseNode* p;
	if((p=SearchChildFirst(ptr->child,TypeDef,0)))//����
	{
		typeIR* tIR=0;
		_TypeDef(p,tIR,layer);//�õ�����
		if(tIR)
			_VarIdList(p->brother,layer,offset,tIR);//���ɴ������µı��������б�
	}
	if((p=SearchBrotherFirst(ptr->child,VarDecMore,0)))//�Ƿ��и���ı�������
		if((p=SearchChildFirst(p->child,VarDecList,0)))//�������б����������ֵķ���
			_VarDecList(p,layer,offset);
}
void Semantic::_VarIdList(ParseNode* ptr,int layer,int &offset,typeIR* tIR)
//ͬ���ͱ��������ķ��ű����� 
{
	SymTableNode* p=new SymTableNode;//�½�һ�����ű���
	strcpy(p->name,ptr->child->parse.Parse_Str);//�õ�����������
	p->next=0;//ָ����һ����ָ��Ϊ��
	p->attrIR.kind=varkind;//����Ϊ����
	p->attrIR.idtype=tIR; //���ô˱���������
	p->attrIR.More.VarAttr.offset=offset;//�õ�ƫ��
	p->attrIR.level=layer;  //�õ������Ĳ���
	p->attrIR.More.VarAttr.access=dir;	//ֱ�ӷ���
	if(myScope.FindID(p->name,0)!=NULL) //�������Ƿ��Ѿ����ڣ��������������ظ�����
	{
	    printf("�� %d��������ظ�����\n", layer);
	}
	else
	{
		if(p->attrIR.idtype)//��������γ�δ����
		{
			offset+=p->attrIR.idtype->size;//����ƫ��ֵ
			symTable.insertNode(p);//����
			if(myScope.scope.top()->front==NULL)//�Ƿ�Ϊ���������ĵ�һ��Ԫ�أ����ǣ���������Ϊ�ֲ����������
			{
				myScope.scope.top()->front=p;//��Ϊ������ű������
				ptr->symtPtr=p;//��ӵ��﷨�����Ա�����м�������������̶���
			}
		}
		else 
			return;
	}
	ParseNode* q;
	if((q=SearchBrotherFirst(ptr->child,VarIdMore,0)))//�Ƿ��и���ı�������
		if((q=SearchBrotherFirst(q->child,VarIdList,0)))//�У�����������������˳�
			_VarIdList(q,layer,offset,tIR);
}
void Semantic::_ProcDecpart(ParseNode* ptr,int layer)//������������--ע���γ��Լ��ĸ�������������Ϣ,�Լ��ж��Ƿ�����������
{	
	ParseNode* p;
	if((p=SearchChildFirst(ptr->child,ProcDec,0)))//�Ƿ���ڹ�����������������������������
	{
		_ProcDec(p,layer);//һ������������������
		if((p=SearchBrotherFirst(p->child,ProcDecMore,0)))//�Ƿ��и���Ĳ��й�������
			if((p=SearchChildFirst(p->child,ProcDec)))//�������������
				_ProcDec(p,layer,1);
	}
}
void Semantic::_TypeDecList(ParseNode* ptr,int layer)//���������б�
{
	ParseNode* q;
	if(ptr->child&&ptr->child->parse.Symbol==TypeId)//�Ƿ�Ϊ�������ƣ����������������Ϣ
	{
		SymTableNode* p=new SymTableNode;//������һ�����ű���
		strcpy(p->name,ptr->child->child->parse.Parse_Str);//��ȡID����
		p->attrIR.kind=typekind;//����Ϊ  ���ͱ���
		p->attrIR.level=layer;  //���ڵڼ���
		p->next=0;
		if((q=SearchBrotherFirst(ptr->child,TypeDef)))//TypeDef
			_TypeDef(q,p->attrIR.idtype,layer);//�γ�һ��������Ϣ
		if(p->attrIR.idtype)//�Ƿ��γ�����Ч������������Ϣ
		{
			symTable.insertNode(p);//������뵽���ű���
			if(myScope.scope.top()->front==NULL)//��ջ������ָ��ķ��ű�Ϊ��ʱ��������Ϊp
			{
				ptr->symtPtr=p;//������Ϣ��ӵ��﷨��
				myScope.scope.top()->front=p;
			}
		}
		else//�����γɳ��������������Ϣ
		{	
		}
	}
	if((q=SearchBrotherFirst(ptr->child,TypeDecMore,0)))//�Ƿ��и������������
		if((q=SearchChildFirst(q->child,TypeDecList,0)))
			_TypeDecList(q,layer);//��һ���ķ���

}
void Semantic::_TypeDef(ParseNode* ptr ,typeIR*& tIR,int layer)//������������
{
	if(ptr->child==NULL)//û�У��򷵻�
	{
		tIR=NULL;
		return ;
	}
	if(ptr->child->parse.Symbol==BaseType)//�������� 
	{
		if(ptr->child->child->parse.Symbol==INTEGER)//����
			tIR=IntTy;
		else if(ptr->child->child->parse.Symbol==CHAR1)//�ַ���
			tIR=CharTy;
	}
	else if(ptr->child->parse.Symbol==StructureType)//�Զ�������
		_StructureType(ptr->child,tIR,layer);
	else if(ptr->child->parse.Symbol==ID)//���ͱ�ʶ��
	{
		SymTableNode* p=myScope.FindID(ptr->child->parse.Parse_Str);//�ҵ����ͱ�ʶ��
		if(p==NULL)//�Ҳ���
		{
			
		}
		else//�ҵ���
		{
			if(p->attrIR.kind!=typekind)
			{
				
			}
			else
				tIR=p->attrIR.idtype;
		}
	}
}
void Semantic::_StructureType(ParseNode* ptr ,typeIR* &tIR,int layer)//�Զ�����������
{
	tIR=new typeIR;//������һ���ռ�
	ParseNode* p;
	if((p=SearchChildFirst(ptr->child,ArrayType,0)))//�Ƿ�Ϊ����
		_ArrayType(p,tIR,layer);
	else//��������������Ѿ�����Ŀռ�ɾ��
	{
		delete tIR;
		tIR=NULL;
	}
}
void Semantic::_ArrayType(ParseNode* ptr ,typeIR*& tIR,int layer)//��������
{
	tIR->kind=arrayTy; //Ϊ��������
	ParseNode* p=ptr->child->brother->brother;
	if(p->parse.Symbol==Low)//�½� Low
		tIR->More.ArrayAttr.low=atoi(p->child->parse.Parse_Str);
	if((p=p->brother->brother)->parse.Symbol==Top)//�Ͻ� Top
		tIR->More.ArrayAttr.top=atoi(p->child->parse.Parse_Str);
	tIR->More.ArrayAttr.indexTy=IntTy; //��������
	tIR->More.ArrayAttr.elemTy =       //Ԫ�����ͣ�����ΪBaseType ���Ը�ΪTypeDef
		p->brother->brother->brother->child->parse.Symbol==INTEGER?IntTy:CharTy;
	if(tIR->More.ArrayAttr.top<=tIR->More.ArrayAttr.low)//�ж��Ƿ��������½����
	{
		printf("��%d�������±�����½�ӦС���Ͻ�!\n",p->brother->parse.LineNum);
	}
	tIR->size=(tIR->More.ArrayAttr.top-tIR->More.ArrayAttr.low+1)*(tIR->More.ArrayAttr.elemTy->size);//���ʹ�С
}
void Semantic::_ProcDec(ParseNode* ptr,int layer,bool mid)//�������������������
{
	ParseNode* p;
	SymTableNode* sym=new SymTableNode;//�½�һ���ű���
	ptr->child->brother->child->symtPtr=sym;	
	strcpy(sym->name,ptr->child->brother->child->parse.Parse_Str);//���̵�����
	sym->next=0;
	sym->attrIR.idtype=0;//���̵�����
	sym->attrIR.kind=prockind;//���ŵ�����
	sym->attrIR.level=layer;//���̵ĵ���
	sym->attrIR.More.ProcAttr.param=0;//�����б��ʼΪ��
	sym->attrIR.More.ProcAttr.size=0;//�����̵ľֲ�������ʼ��С��Ϊ��
	sym->EOFL=1;//�����Խ����̷ŵ���һ����ȥ��
	if(myScope.scope.top()->front==NULL)
	{
		myScope.scope.top()->front=sym;//Ϊ����ĳ�ʼ���ű���
	}
		symTable.insertNode(sym);//���뵽���ű���
	myScope.newLayer(0);//�Ա����ʱ����ȷʶ��
	p=ptr->child->brother->brother->brother;
	_ParamList(p,layer,sym->attrIR.More.ProcAttr.size,sym->attrIR.More.ProcAttr.param);//�õ������б�
	_DeclarePart((p=p->brother->brother->brother)->child,layer,
		sym->attrIR.More.ProcAttr.size); //�õ���������
	_ProgramBody(p->brother->child);//�Գ���������������	
	myScope.DropLayer();//ȥ����һ�� 	
}
void Semantic::_ParamList(ParseNode* ptr,int layer,int& size,ParamTable*& param)//�γɲ����б�
{
	ParseNode* p;
	if((p=SearchChildFirst(ptr->child,ParamDecList,0)))//�Ƿ���ڲ����б�
		_ParamDecList(p,layer,size,param);
}
void Semantic::_ParamDecList(ParseNode* ptr,int layer,int& size,ParamTable*& param)
//�γɲ����б�ͬʱ�γɷ��ű�
{
	ParseNode* p;
	if(ptr->child&&ptr->child->parse.Symbol==Param)//�в���
	{
		typeIR *tIR=0;//=new typeIR;
		if((p=SearchChildFirst(ptr->child,TypeDef,0)))
		{
			_TypeDef(p,tIR,layer);//�õ���������
			if(tIR)//���ͷ����ɹ�
				_FormList(p->brother,layer,size,param,tIR,0);//���ｫ�޸�param��ָ��
		}
		else if((p=SearchChildFirst(ptr->child,VAR,0)))//�ҵ���ӷô�Ĳ���
		{
			if(p->brother&&p->brother->parse.Symbol==TypeDef)
			{
				_TypeDef(p->brother,tIR,layer);
				if(tIR&&(p=SearchBrotherFirst(p->brother,FormList,0)))
					_FormList(p,layer,size,param,tIR,1);//���ｫ�޸�param��ָ��--�γ��β��б�
			}
		}
	}
	if((p=SearchBrotherFirst(ptr->child,ParamMore,0)))//�Ƿ��и����������͵Ĳ�����
		if((p=SearchBrotherFirst(p->child,ParamDecList,0)))//�����������
		{
			if(param)//��param����һ�η����б��޸�ʱ��Ӧ���ҵ�������β����ָ��
			{
				ParamTable* pm=param;
				while(pm->next) pm=pm->next;//�ҵ���δһ���ǿ���
				_ParamDecList(p,layer,size,pm->next);//���ɸ���Ĳ�����Ϣ
			}
			else 
				_ParamDecList(p,layer,size,param);//������Ȼ��paramָ���λ�ÿ�ʼ����
		}

}
void Semantic::_FormList(ParseNode* ptr,int layer,int& size,ParamTable*& param,typeIR* tIR,bool ntype)//�β���
{
	ParseNode* p;
	SymTableNode* sym=new SymTableNode;//�½���һ�����ű���
	strcpy(sym->name,ptr->child->parse.Parse_Str);//�õ��βε�����
	sym->next=0;
	sym->attrIR.idtype=tIR;//�βε�����
	sym->attrIR.kind=varkind;//��������
	sym->attrIR.More.VarAttr.access=AccessKind(ntype);//ע������ô淽ʽ 
	sym->attrIR.level=layer;//����
	sym->attrIR.More.VarAttr.offset=size;//ƫ��
	if(myScope.FindID(sym->name,0))//�Ƿ�����Ѿ��������������γɳ�����Ϣ
	{	
		if((p=SearchBrotherFirst(ptr->child,FormList,1)))//�������з���
			_FormList(p,layer,size,param,tIR,ntype);
	}
	else //��������뵽���ű�����
	{
		size+=tIR->size;//���δ�ظ����壬������
		symTable.insertNode(sym);//���뵽��������
		param=new ParamTable;//�½���һ���βν��
		param->type=tIR;//����������Ϣ
		param->symPtr=sym;//ָ���˱������Է��������ʵ�εĽ��� ��ָ�����ڴ�ռ䴫ֵ
		param->next=0;
		if(myScope.scope.top()->front==NULL)//���β��Ƿ�Ϊ������ŵĵ�һ������
			myScope.scope.top()->front=sym;//��
		if((p=SearchBrotherFirst(ptr->child,FormList,1)))//�������з���
			_FormList(p,layer,size,param->next,tIR,ntype);
	}
}
void Semantic::_StmList(ParseNode* ptr)//������е�������� 
{
	ParseNode* p;
	if(ptr==NULL)//���Ϊ�գ����������
		return ;
	if((p=SearchChildFirst(ptr->child,ConditionalStm,0)))//�������ķ���
		_ConditionalStm(p);
	else if((p=SearchChildFirst(ptr->child,LoopStm,0)))//ѭ�����ķ���
		_LoopStm(p);
	else if((p=SearchChildFirst(ptr->child,InputStm,0)))//�������ķ���
		_InputStm(p);
	else if((p=SearchChildFirst(ptr->child,OutputStm,0)))//������ķ���
		_OutputStm(p);
	else if((p=SearchChildFirst(ptr->child,ReturnStm,0)))//�������ķ���
		_ReturnStm(p);
	else if((p=SearchChildFirst(ptr->child,ID,0)))//������䣺�������ã���ֵ���ķ���
		_OtherStm(p);
	if((p=SearchBrotherFirst(ptr->child,StmMore,0)))//�Ƿ��и����������У�
		if((p=SearchBrotherFirst(p->child,StmList,0)))//���Ǽ������з���
			_StmList(p);
}
void Semantic::_ConditionalStm(ParseNode* ptr)//�������ķ���
{
	ParseNode* p;
	if((p=SearchBrotherFirst(ptr->child,RelExp,0)))//�Ƿ�Ϊ�������ʽ
		_RelExp(p);
	if((p=SearchBrotherFirst(ptr->child,StmList,0)))//��������е�then�������ķ���
	{
		_StmList(p);
		if((p=SearchBrotherFirst(p->brother,StmList,0)))//else
			_StmList(p);
	}

}
void Semantic::_LoopStm(ParseNode* ptr)//ѭ�����ķ���
{
	ParseNode* p;
	if((p=SearchBrotherFirst(ptr->child,RelExp,0)))//�����ж�
		_RelExp(p);
	if((p=SearchBrotherFirst(ptr->child,StmList,0)))//ѭ������е�do�������ķ���
		_StmList(p);
}
void Semantic::_InputStm(ParseNode* ptr)//������ķ���
{
	ParseNode* p;
	if((p=SearchBrotherFirst(ptr->child,Invar,0)))//�Ƿ����������Ĳ���
		_Invar(p);//�õ�ʵ������Ϣ
}
void Semantic::_OutputStm(ParseNode* ptr)//������
{
	ParseNode* p;
	typeIR* tIR=0;
	int ntype=0;
	if((p=SearchBrotherFirst(ptr->child,Exp,0)))//������ı��ʽ
		_Exp(p,tIR,ntype);//�õ����ʽ
}
void Semantic::_ReturnStm(ParseNode* ptr)//�������
{
}
void Semantic::_OtherStm(ParseNode* ptr)//��ֵ��估���̵������
{
	ParseNode* p;
	SymTableNode* sym=myScope.FindID(ptr->parse.Parse_Str);//�ҵ���ʶ���ķ��ű�λ��
	if(sym==NULL)//δ�ұ�ʶ������
	{
	
	}
	else 
	{
		ptr->symtPtr=sym;//�﷨����㶨�򵽷��ű�
		if((p=SearchChildFirst(ptr->brother,AssignmentRest,0)))//��ֵ���
			_AssignmentRest(p,sym);//�鿴�����Ƿ�����
		else if((p=SearchChildFirst(ptr->brother,CallStmRest,0)))//���̵������
			_CallStmRest(p,sym);
	}
}
void Semantic::_AssignmentRest(ParseNode* ptr,SymTableNode* sym)//��ֵ������
{
	ParseNode* p;
	typeIR* tIR=sym->attrIR.idtype;//����
	int ntype=1;//������Լ��������������Ժ�ķ����ܽ����ڴ�tIR����
	if((p=SearchBrotherFirst(ptr->child,VariMore,0)))//Ϊ������߼�¼����
		_VariMore(p,sym,tIR);
	if(tIR==CharTy) tIR=IntTy;//�м����׶ζ���������޸ģ���֧�������������ַ����͵ļ���
	if((p=SearchBrotherFirst(ptr->child,Exp,0)))//���ʽ
		_Exp(p,tIR,ntype);//��sym�γ����ݹ�ϵ
}
void Semantic::_CallStmRest(ParseNode* ptr,SymTableNode* sym)//���̵������
{
	ParseNode* p;
	if((p=SearchBrotherFirst(ptr->child,ActParamList,0)))//��ʵ�ν��////////////////////////
	{	
		if(sym->attrIR.kind==prockind)
			_ActParamList(p,sym->attrIR.More.ProcAttr.param);
		else
		{
		}
	}
}
void Semantic::_ActParamList(ParseNode* ptr,ParamTable* param)//��ʵ�ν��////////////////�м����׶ν������޸�
{
	ParseNode* p;
	int ntype=1;////////////���ݼ��   �޸����м����ʱ
	typeIR* tIR;
	if((p=SearchChildFirst(ptr->child,Exp,0)))///////////////���ʽ
	{
		
		if(param->type==CharTy)//���ַ����ͣ�ֻ��Ҫ�ֽڼ��ݼ���
			_Exp(p,tIR=IntTy,ntype=0);
		else
			_Exp(p,tIR=param->type,ntype);
		if((p=SearchBrotherFirst(p->brother,ActParamMore,0)))//�����ʵ��
		{
			if((p=SearchBrotherFirst(p->child,ActParamList,0)))
				_ActParamList(p,param->next);
			else if(param->next)//�����β�
			{
			}
		}
	}
	else if(param)/////////////////////////////////////����Ҳ���exp���β�δ��ȫ���
	{
		
	}
}
void Semantic::_RelExp(ParseNode* ptr)//�������ʽ
{
	typeIR* tIR=NULL;
	ParseNode* p;
	int ntype=0;
	if((p=SearchChildFirst(ptr->child,Exp,0)))//�ں����������ʽ
	{
		_Exp(p,tIR,ntype);
		if(tIR==CharTy) tIR=IntTy;///////////////////////�м����׶ζ���������޸ģ���֧�������������ַ����͵ļ���
		if((p=SearchChildFirst(p->brother,Exp,1)))//����
			_Exp(p,tIR,ntype=1);//����
	}
}
void Semantic::_Invar(ParseNode* ptr)
{
	if(ptr->child&&ptr->child->parse.Symbol==ID)
	{
		SymTableNode* sym=myScope.FindID(ptr->child->parse.Parse_Str,1);
		if(sym==NULL)//δ�ҵ�
		{		
		}
		else if(sym->attrIR.kind!=varkind)//���Ǳ���
		{	
		}
		else 
			ptr->child->symtPtr=sym;
	}
}
void Semantic::_Exp(ParseNode* ptr,typeIR* &tIR,int &ntype)
	//��ntype==0ʱ������Ҫ��������ж������ߵôӺ���õ������ͽ������ݴ���
	//��ntype==1ʱ������������Ҫ��sym����������
	//��ntype==2ʱ��������EXP���У�Ҫ��sym���Զ��������γ����Ƽ���ϵ����Ϊ����
{
	ParseNode* p;
	if((p=SearchChildFirst(ptr->child,Term,0)))//�������ʽ�ĵ�һ��
		_Term(p,tIR,ntype);
	if((p=SearchBrotherFirst(ptr->child,OtherTerm,0)))//�������ʽ��������
		_OtherTerm(p,tIR,ntype);
}
void Semantic::_VariMore(ParseNode* ptr,SymTableNode* sym,typeIR* &tIR)
//���ʽ�е��������͵��ڲ�Ԫ��
{
	ParseNode* p,q;
	int ntype=2;
	if((p=SearchBrotherFirst(ptr->child,Exp,0)))//�����±������ʽ
	{
		tIR=sym->attrIR.idtype->More.ArrayAttr.indexTy;
		_Exp(p,tIR,ntype);//����
	}

}
void Semantic::_Term(ParseNode* ptr,typeIR* &tIR,int &ntype)//�������ʽ��һ��
{
	ParseNode* p=0,*q=0;
	if((p=SearchChildFirst(ptr->child,Factor,0)))//����
	{
		if((q=SearchBrotherFirst(p->child,Exp,0)))//���ʽ
			_Exp(q,tIR,ntype);
		else if((q=SearchChildFirst(p->child,Variable,0)))//��Ϊ����Ϊ��¼ʱ
			_Variable(q,tIR,ntype);
		else if(tIR&&p->child && (q=p->child)->parse.Symbol==INTC)//Ϊ��������ʱ
			if(tIR!=IntTy&&ntype!=2)//���ͳ���////////////////////////////////�м��������ʱ���޸�
			{
			}
		if((q=SearchChildFirst(p->brother,OtherFactor,0)))//��������
			if((q=SearchBrotherFirst(q->child,Term,0)))
				_Term(q,tIR,ntype);
	}
}
void Semantic::_OtherTerm(ParseNode* ptr,typeIR* &tIR,int &ntype)//�������ʽ�е�������
{
	ParseNode* p;
	if((p=SearchBrotherFirst(ptr->child,Exp,1)))//������
		_Exp(p,tIR,ntype);
}
void Semantic::_Variable(ParseNode* ptr,typeIR* &tIR,int &ntype)//����----------------���м����׶ν�����ȫ����޸�
{
	ParseNode* p,*q;
	int type;
	if((p=SearchChildFirst(ptr->child,ID)))//Ϊ��ʶ��----------------- p->parse.sym=id
	{
		SymTableNode* sym=myScope.FindID(p->parse.Parse_Str,1);//�ڱ������������
		if(sym)//�ҵ�
		{
			p->symtPtr=sym;//��ӵ��﷨���У�����������м�������ɵȲ���
			if(sym->attrIR.kind!=varkind)//��Ϊ��������
			{
			}
			else//Ϊ����------------        p=>   id
			{
				if(p->brother&&p->brother->child)//�������VaiMore
				{//--------------------------------------------------------------����VariMore
					if((q=SearchBrotherFirst(p->brother->child,Exp,0)))
					{///Ϊ��������
						if(sym->attrIR.idtype->kind!=arrayTy)//����ʶ��������������
						{	
						}
						if(ntype && sym->attrIR.idtype->More.ArrayAttr.elemTy!=tIR)//���β�ƥ��
						{	
						}
						else//�������±��������ʽ�����������
						{
							typeIR* tIR0=IntTy;
							_Exp(q,tIR0,type=2);//q=exp  ��q����������������
							if(ntype==0)//�޸�ƥ������
							{
								ntype=1;
								tIR=sym->attrIR.idtype->More.ArrayAttr.elemTy;//�޸�Ҫƥ�������
							}
						}
					}
				}//VariMore����
				else
				{//---------------------------------------------------------------������Variable:=ID
					if(ntype==0)//���������
					{
						tIR=sym->attrIR.idtype;//�޸ļ�������
						ntype=1;
					}
					else if(ntype==1)//�����1�������������ͼ������
					{
						if(sym->attrIR.idtype!=tIR)//�������
						{	
						}
					}
					else if(ntype==2&&sym->attrIR.idtype!=IntTy)//����Ƿ�Ϊ�����±�����
					{	
					}
				}
			}//     p=>id  p=
		}//////�ҵ���Ӧ��sym
		else //�ֲ������ű���δ�ҵ��˱�ʶ��
		{	
		}
	}
}