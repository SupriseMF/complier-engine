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
	if(ptr->attrIR.kind==prockind)//获得最大层数
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
		while(ptr)//当未遇到符号表未尾
		{
			if((ptr->attrIR.kind!=prockind&&ptr->attrIR.level==i)||(ptr->attrIR.kind==prockind&&ptr->attrIR.level-1==i))
			{
				if(ptr->attrIR.kind==typekind)	   //输出的是类型 
					printf("%8s:  %8s    %8s\n",ptr->name,TypeKindStr[ptr->attrIR.idtype->kind],IdKindStr[ptr->attrIR.kind]);
				else if(ptr->attrIR.kind==prockind)//输出的是过程
					printf("%8s:  %8s    Level:%2d    Noff: %2d\n",ptr->name,IdKindStr[ptr->attrIR.kind],ptr->attrIR.level,ptr->attrIR.More.ProcAttr.size);
				else if(ptr->attrIR.kind==varkind) //输出的是变量
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
	ScopeNode* nptr=new ScopeNode;//申请得到一个新的结点，以暂存每一层的头指针信息
	nptr->front=ptr;//指向符号表中的一个项
	if(scope.empty())//找到父层指针
		nptr->parent=NULL;
	else
		nptr->parent=scope.top();
	scope.push(nptr);//压栈
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
SymTableNode* Scope::FindID(const char* idname,bool ntype)//从符号表栈中查找已经声明的标识符
/*ntype==1时遍历所有层
**ntype==0时仅遍历自己所在层
**对于不同的情况，用法是不同的，比如说对于变量的声明，则采用的是ntype=0，
**则相当于允许子层重复声明同样名称的变量
**而对于程序体的审查，则应该使ntyp=0从而遍历所有的层，
**以保证本层未声明的变量，在上层声明也是允许的
**/
{
	ScopeNode * ptr=scope.top();
	if(scope.empty())
		return NULL;
	while(ptr)
	{
		SymTableNode* nptr=ptr->front;
		while(nptr)//从当前层找，如果到达符号表尾结束循环
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
void Semantic::initTy()//初始化SNL语言基本类型
{
	IntTy=new typeIR; 
	CharTy=new typeIR;
	BoolTy=new typeIR;

	//整数类型
	IntTy->kind=intTy;
	IntTy->size=INTSIZE;
    //字符类型
	CharTy->kind=charTy;
	CharTy->size=CHARSIZE;
    //布尔类型
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
	_Program(parseTree->root);//开始
	return 1;
}
void Semantic::_Program(ParseNode* ptr)//总程序
{
	ParseNode* p;//暂存指针
	if((p=SearchBrotherFirst(ptr->child,DeclarePart,0)))//声明部分
	{
		myScope.newLayer(0);//新建立一层，程序开始部分的声明部分分先形成一新层
		int offset=OFFSET;//偏移
		_DeclarePart(p,0,offset);
	}
	if((p=SearchBrotherFirst(ptr->child,ProgramBody,0)))//程序体部分
		_ProgramBody(p);
}
void Semantic::_DeclarePart(ParseNode* ptr,int layer,int &offset)//处理程序声明
{
	ParseNode* p;
	if((p=SearchChildFirst(ptr->child,TypeDecpart,0)))//类型声明部分
		_TypeDecpart(p,layer);
	if((p=SearchBrotherFirst(ptr->child,VarDecpart,0)))//变量声明区
		_VarDecpart(p,layer,offset);
	if((p=SearchBrotherFirst(ptr->child,ProcDecpart,0)))//过程声明
		if(SearchChildFirst(p,ProcDecPart,1))
			_ProcDecpart(p,layer+1);//如果是过程声明，则层数加一
}
void Semantic::_ProgramBody(ParseNode* ptr)//程序体部分
{
	ParseNode* p;
	if((p=SearchBrotherFirst(ptr->child,StmList)))//得到语句序列语法树结点
		_StmList(p);
}
void Semantic::_TypeDecpart(ParseNode* ptr,int layer)//类型声明----形成符号表（类型信息同时生成）
{
	ParseNode* p;
	if(ptr->child&&ptr->child->parse.Symbol==-1)//如果无类型声明
		return ;
	if((p=SearchChildFirst(ptr->child,TypeDecList,1)))//有类型声明
		_TypeDecList(p,layer);
}
void Semantic::_VarDecpart(ParseNode* ptr,int layer,int &offset)//变量声明----形成符号表，注意变量的偏移地址等信息的生成
{
	ParseNode* p;
	if(ptr->child&&ptr->child->parse.Symbol==-1)//无变量声明
		return;
	if((p=SearchChildFirst(ptr->child,VarDecList,1)))//有变量声明
		_VarDecList(p,layer,offset);
}
void Semantic::_VarDecList(ParseNode* ptr,int layer,int &offset)//变量声明列表
{
	ParseNode* p;
	if((p=SearchChildFirst(ptr->child,TypeDef,0)))//类型
	{
		typeIR* tIR=0;
		_TypeDef(p,tIR,layer);//得到类型
		if(tIR)
			_VarIdList(p->brother,layer,offset,tIR);//生成此类型下的变量声明列表
	}
	if((p=SearchBrotherFirst(ptr->child,VarDecMore,0)))//是否还有更多的变量声明
		if((p=SearchChildFirst(p->child,VarDecList,0)))//继续进行变量声明部分的分析
			_VarDecList(p,layer,offset);
}
void Semantic::_VarIdList(ParseNode* ptr,int layer,int &offset,typeIR* tIR)
//同类型变量声明的符号表生成 
{
	SymTableNode* p=new SymTableNode;//新建一个符号表项
	strcpy(p->name,ptr->child->parse.Parse_Str);//得到变量的名称
	p->next=0;//指向下一结点的指针为空
	p->attrIR.kind=varkind;//类型为变量
	p->attrIR.idtype=tIR; //设置此变量的类型
	p->attrIR.More.VarAttr.offset=offset;//得到偏移
	p->attrIR.level=layer;  //得到变量的层数
	p->attrIR.More.VarAttr.access=dir;	//直接访问
	if(myScope.FindID(p->name,0)!=NULL) //本层中是否已经存在，如存在则表明有重复定义
	{
	    printf("在 %d层变量有重复定义\n", layer);
	}
	else
	{
		if(p->attrIR.idtype)//如果类型形成未出错
		{
			offset+=p->attrIR.idtype->size;//更新偏移值
			symTable.insertNode(p);//将其
			if(myScope.scope.top()->front==NULL)//是否为本层声明的第一个元素，如是，将其设置为局部化表的首项
			{
				myScope.scope.top()->front=p;//置为本层符号表的首项
				ptr->symtPtr=p;//添加到语法树，以便进行中间代码或者其它后继动作
			}
		}
		else 
			return;
	}
	ParseNode* q;
	if((q=SearchBrotherFirst(ptr->child,VarIdMore,0)))//是否有更多的变量声明
		if((q=SearchBrotherFirst(q->child,VarIdList,0)))//有，则继续分析，无则退出
			_VarIdList(q,layer,offset,tIR);
}
void Semantic::_ProcDecpart(ParseNode* ptr,int layer)//函数声明部分--注意形成自己的各个声明部分信息,以及判断是否产生主义错误
{	
	ParseNode* p;
	if((p=SearchChildFirst(ptr->child,ProcDec,0)))//是否存在过程声明，存在则进入过程声明分析
	{
		_ProcDec(p,layer);//一个过程声明分析函数
		if((p=SearchBrotherFirst(p->child,ProcDecMore,0)))//是否有更多的并行过程声明
			if((p=SearchChildFirst(p->child,ProcDec)))//存在则继续分析
				_ProcDec(p,layer,1);
	}
}
void Semantic::_TypeDecList(ParseNode* ptr,int layer)//类型声明列表
{
	ParseNode* q;
	if(ptr->child&&ptr->child->parse.Symbol==TypeId)//是否为类型名称，是则生成其相关信息
	{
		SymTableNode* p=new SymTableNode;//新申请一个符号表项
		strcpy(p->name,ptr->child->child->parse.Parse_Str);//获取ID名称
		p->attrIR.kind=typekind;//类型为  类型变量
		p->attrIR.level=layer;  //处于第几层
		p->next=0;
		if((q=SearchBrotherFirst(ptr->child,TypeDef)))//TypeDef
			_TypeDef(q,p->attrIR.idtype,layer);//形成一个类型信息
		if(p->attrIR.idtype)//是否形成了有效的类型声明信息
		{
			symTable.insertNode(p);//将其插入到符号表中
			if(myScope.scope.top()->front==NULL)//当栈顶层所指向的符号表为空时，将其置为p
			{
				ptr->symtPtr=p;//语义信息添加到语法树
				myScope.scope.top()->front=p;
			}
		}
		else//类型形成出错，则输出出错信息
		{	
		}
	}
	if((q=SearchBrotherFirst(ptr->child,TypeDecMore,0)))//是否有更多的类型声明
		if((q=SearchChildFirst(q->child,TypeDecList,0)))
			_TypeDecList(q,layer);//进一步的分析

}
void Semantic::_TypeDef(ParseNode* ptr ,typeIR*& tIR,int layer)//类型声明部分
{
	if(ptr->child==NULL)//没有，则返回
	{
		tIR=NULL;
		return ;
	}
	if(ptr->child->parse.Symbol==BaseType)//基本类型 
	{
		if(ptr->child->child->parse.Symbol==INTEGER)//整型
			tIR=IntTy;
		else if(ptr->child->child->parse.Symbol==CHAR1)//字符型
			tIR=CharTy;
	}
	else if(ptr->child->parse.Symbol==StructureType)//自定义类型
		_StructureType(ptr->child,tIR,layer);
	else if(ptr->child->parse.Symbol==ID)//类型标识符
	{
		SymTableNode* p=myScope.FindID(ptr->child->parse.Parse_Str);//找到类型标识符
		if(p==NULL)//找不到
		{
			
		}
		else//找到了
		{
			if(p->attrIR.kind!=typekind)
			{
				
			}
			else
				tIR=p->attrIR.idtype;
		}
	}
}
void Semantic::_StructureType(ParseNode* ptr ,typeIR* &tIR,int layer)//自定义类型声明
{
	tIR=new typeIR;//新申请一个空间
	ParseNode* p;
	if((p=SearchChildFirst(ptr->child,ArrayType,0)))//是否为数组
		_ArrayType(p,tIR,layer);
	else//都不是则出错，将已经申请的空间删除
	{
		delete tIR;
		tIR=NULL;
	}
}
void Semantic::_ArrayType(ParseNode* ptr ,typeIR*& tIR,int layer)//数组类型
{
	tIR->kind=arrayTy; //为数组类型
	ParseNode* p=ptr->child->brother->brother;
	if(p->parse.Symbol==Low)//下界 Low
		tIR->More.ArrayAttr.low=atoi(p->child->parse.Parse_Str);
	if((p=p->brother->brother)->parse.Symbol==Top)//上界 Top
		tIR->More.ArrayAttr.top=atoi(p->child->parse.Parse_Str);
	tIR->More.ArrayAttr.indexTy=IntTy; //索引类型
	tIR->More.ArrayAttr.elemTy =       //元素类型，这里为BaseType 可以改为TypeDef
		p->brother->brother->brother->child->parse.Symbol==INTEGER?IntTy:CharTy;
	if(tIR->More.ArrayAttr.top<=tIR->More.ArrayAttr.low)//判断是否数组上下界出错
	{
		printf("第%d行数组下标出错，下界应小于上界!\n",p->brother->parse.LineNum);
	}
	tIR->size=(tIR->More.ArrayAttr.top-tIR->More.ArrayAttr.low+1)*(tIR->More.ArrayAttr.elemTy->size);//类型大小
}
void Semantic::_ProcDec(ParseNode* ptr,int layer,bool mid)//过程声明语义分析函数
{
	ParseNode* p;
	SymTableNode* sym=new SymTableNode;//新建一符号表项
	ptr->child->brother->child->symtPtr=sym;	
	strcpy(sym->name,ptr->child->brother->child->parse.Parse_Str);//过程的名称
	sym->next=0;
	sym->attrIR.idtype=0;//过程的类型
	sym->attrIR.kind=prockind;//符号的类型
	sym->attrIR.level=layer;//过程的导数
	sym->attrIR.More.ProcAttr.param=0;//参数列表初始为空
	sym->attrIR.More.ProcAttr.size=0;//将过程的局部变量初始大小置为零
	sym->EOFL=1;//这里仍将过程放到上一层中去，
	if(myScope.scope.top()->front==NULL)
	{
		myScope.scope.top()->front=sym;//为本层的初始符号表项
	}
		symTable.insertNode(sym);//插入到符号表中
	myScope.newLayer(0);//以便调用时能正确识别
	p=ptr->child->brother->brother->brother;
	_ParamList(p,layer,sym->attrIR.More.ProcAttr.size,sym->attrIR.More.ProcAttr.param);//得到参数列表
	_DeclarePart((p=p->brother->brother->brother)->child,layer,
		sym->attrIR.More.ProcAttr.size); //得到声明部分
	_ProgramBody(p->brother->child);//对程序体进行语义分析	
	myScope.DropLayer();//去除旧一层 	
}
void Semantic::_ParamList(ParseNode* ptr,int layer,int& size,ParamTable*& param)//形成参数列表
{
	ParseNode* p;
	if((p=SearchChildFirst(ptr->child,ParamDecList,0)))//是否存在参数列表
		_ParamDecList(p,layer,size,param);
}
void Semantic::_ParamDecList(ParseNode* ptr,int layer,int& size,ParamTable*& param)
//形成参数列表，同时形成符号表
{
	ParseNode* p;
	if(ptr->child&&ptr->child->parse.Symbol==Param)//有参数
	{
		typeIR *tIR=0;//=new typeIR;
		if((p=SearchChildFirst(ptr->child,TypeDef,0)))
		{
			_TypeDef(p,tIR,layer);//得到的是类型
			if(tIR)//类型分析成功
				_FormList(p->brother,layer,size,param,tIR,0);//这里将修改param的指向
		}
		else if((p=SearchChildFirst(ptr->child,VAR,0)))//找到间接访存的参数
		{
			if(p->brother&&p->brother->parse.Symbol==TypeDef)
			{
				_TypeDef(p->brother,tIR,layer);
				if(tIR&&(p=SearchBrotherFirst(p->brother,FormList,0)))
					_FormList(p,layer,size,param,tIR,1);//这里将修改param的指向--形成形参列表
			}
		}
	}
	if((p=SearchBrotherFirst(ptr->child,ParamMore,0)))//是否有更多其它类型的参数项
		if((p=SearchBrotherFirst(p->child,ParamDecList,0)))//有则继续分析
		{
			if(param)//当param在上一次分析中被修改时，应该找到它的最尾部的指针
			{
				ParamTable* pm=param;
				while(pm->next) pm=pm->next;//找到最未一个非空项
				_ParamDecList(p,layer,size,pm->next);//生成更多的参数信息
			}
			else 
				_ParamDecList(p,layer,size,param);//否则依然从param指向的位置开始继续
		}

}
void Semantic::_FormList(ParseNode* ptr,int layer,int& size,ParamTable*& param,typeIR* tIR,bool ntype)//形参项
{
	ParseNode* p;
	SymTableNode* sym=new SymTableNode;//新建立一个符号表项
	strcpy(sym->name,ptr->child->parse.Parse_Str);//得到形参的名称
	sym->next=0;
	sym->attrIR.idtype=tIR;//形参的类型
	sym->attrIR.kind=varkind;//变量类型
	sym->attrIR.More.VarAttr.access=AccessKind(ntype);//注意这里，访存方式 
	sym->attrIR.level=layer;//层数
	sym->attrIR.More.VarAttr.offset=size;//偏移
	if(myScope.FindID(sym->name,0))//是否参数已经声明过，如是形成出错信息
	{	
		if((p=SearchBrotherFirst(ptr->child,FormList,1)))//继续进行分析
			_FormList(p,layer,size,param,tIR,ntype);
	}
	else //否则将其插入到符号表项中
	{
		size+=tIR->size;//如果未重复定义，则增加
		symTable.insertNode(sym);//加入到参数表中
		param=new ParamTable;//新建立一个形参结点
		param->type=tIR;//保存类型信息
		param->symPtr=sym;//指出了变量，以方便进行形实参的结束 向指定的内存空间传值
		param->next=0;
		if(myScope.scope.top()->front==NULL)//此形参是否为本层符号的第一个表项
			myScope.scope.top()->front=sym;//是
		if((p=SearchBrotherFirst(ptr->child,FormList,1)))//继续进行分析
			_FormList(p,layer,size,param->next,tIR,ntype);
	}
}
void Semantic::_StmList(ParseNode* ptr)//语句序列的语义分析 
{
	ParseNode* p;
	if(ptr==NULL)//如果为空，则结束分析
		return ;
	if((p=SearchChildFirst(ptr->child,ConditionalStm,0)))//条件语句的分析
		_ConditionalStm(p);
	else if((p=SearchChildFirst(ptr->child,LoopStm,0)))//循环语句的分析
		_LoopStm(p);
	else if((p=SearchChildFirst(ptr->child,InputStm,0)))//输入语句的分析
		_InputStm(p);
	else if((p=SearchChildFirst(ptr->child,OutputStm,0)))//输出语句的分析
		_OutputStm(p);
	else if((p=SearchChildFirst(ptr->child,ReturnStm,0)))//返回语句的分析
		_ReturnStm(p);
	else if((p=SearchChildFirst(ptr->child,ID,0)))//其它语句：函数调用，赋值语句的分析
		_OtherStm(p);
	if((p=SearchBrotherFirst(ptr->child,StmMore,0)))//是否有更多的语句序列，
		if((p=SearchBrotherFirst(p->child,StmList,0)))//如是继续进行分析
			_StmList(p);
}
void Semantic::_ConditionalStm(ParseNode* ptr)//条件语句的分析
{
	ParseNode* p;
	if((p=SearchBrotherFirst(ptr->child,RelExp,0)))//是否为条件表达式
		_RelExp(p);
	if((p=SearchBrotherFirst(ptr->child,StmList,0)))//条件语句中的then部分语句的分析
	{
		_StmList(p);
		if((p=SearchBrotherFirst(p->brother,StmList,0)))//else
			_StmList(p);
	}

}
void Semantic::_LoopStm(ParseNode* ptr)//循环语句的分析
{
	ParseNode* p;
	if((p=SearchBrotherFirst(ptr->child,RelExp,0)))//条件判断
		_RelExp(p);
	if((p=SearchBrotherFirst(ptr->child,StmList,0)))//循环语句中的do部分语句的分析
		_StmList(p);
}
void Semantic::_InputStm(ParseNode* ptr)//输出语句的分析
{
	ParseNode* p;
	if((p=SearchBrotherFirst(ptr->child,Invar,0)))//是否有输入语句的参数
		_Invar(p);//得到实参数信息
}
void Semantic::_OutputStm(ParseNode* ptr)//输出语句
{
	ParseNode* p;
	typeIR* tIR=0;
	int ntype=0;
	if((p=SearchBrotherFirst(ptr->child,Exp,0)))//输出语句的表达式
		_Exp(p,tIR,ntype);//得到表达式
}
void Semantic::_ReturnStm(ParseNode* ptr)//返回语句
{
}
void Semantic::_OtherStm(ParseNode* ptr)//赋值语句及过程调用语句
{
	ParseNode* p;
	SymTableNode* sym=myScope.FindID(ptr->parse.Parse_Str);//找到标识符的符号表位置
	if(sym==NULL)//未找标识符名称
	{
	
	}
	else 
	{
		ptr->symtPtr=sym;//语法树结点定向到符号表
		if((p=SearchChildFirst(ptr->brother,AssignmentRest,0)))//赋值语句
			_AssignmentRest(p,sym);//查看类型是否适宜
		else if((p=SearchChildFirst(ptr->brother,CallStmRest,0)))//过程调用语句
			_CallStmRest(p,sym);
	}
}
void Semantic::_AssignmentRest(ParseNode* ptr,SymTableNode* sym)//赋值语句分析
{
	ParseNode* p;
	typeIR* tIR=sym->attrIR.idtype;//类型
	int ntype=1;//分析的约束条件，这表明以后的分析受结束于此tIR类型
	if((p=SearchBrotherFirst(ptr->child,VariMore,0)))//为数组或者纪录类型
		_VariMore(p,sym,tIR);
	if(tIR==CharTy) tIR=IntTy;//中间代码阶段对其进行了修改，以支持整数类型与字符类型的兼容
	if((p=SearchBrotherFirst(ptr->child,Exp,0)))//表达式
		_Exp(p,tIR,ntype);//与sym形成相容关系
}
void Semantic::_CallStmRest(ParseNode* ptr,SymTableNode* sym)//过程调用语句
{
	ParseNode* p;
	if((p=SearchBrotherFirst(ptr->child,ActParamList,0)))//形实参结合////////////////////////
	{	
		if(sym->attrIR.kind==prockind)
			_ActParamList(p,sym->attrIR.More.ProcAttr.param);
		else
		{
		}
	}
}
void Semantic::_ActParamList(ParseNode* ptr,ParamTable* param)//形实形结合////////////////中间代码阶段进行了修改
{
	ParseNode* p;
	int ntype=1;////////////兼容检查   修改于中间代码时
	typeIR* tIR;
	if((p=SearchChildFirst(ptr->child,Exp,0)))///////////////表达式
	{
		
		if(param->type==CharTy)//是字符类型，只需要字节兼容即可
			_Exp(p,tIR=IntTy,ntype=0);
		else
			_Exp(p,tIR=param->type,ntype);
		if((p=SearchBrotherFirst(p->brother,ActParamMore,0)))//更多的实参
		{
			if((p=SearchBrotherFirst(p->child,ActParamList,0)))
				_ActParamList(p,param->next);
			else if(param->next)//仍有形参
			{
			}
		}
	}
	else if(param)/////////////////////////////////////如果找不到exp但形参未完全结合
	{
		
	}
}
void Semantic::_RelExp(ParseNode* ptr)//条件表达式
{
	typeIR* tIR=NULL;
	ParseNode* p;
	int ntype=0;
	if((p=SearchChildFirst(ptr->child,Exp,0)))//内含的算术表达式
	{
		_Exp(p,tIR,ntype);
		if(tIR==CharTy) tIR=IntTy;///////////////////////中间代码阶段对其进行了修改，以支持整数类型与字符类型的兼容
		if((p=SearchChildFirst(p->brother,Exp,1)))//仍有
			_Exp(p,tIR,ntype=1);//受限
	}
}
void Semantic::_Invar(ParseNode* ptr)
{
	if(ptr->child&&ptr->child->parse.Symbol==ID)
	{
		SymTableNode* sym=myScope.FindID(ptr->child->parse.Parse_Str,1);
		if(sym==NULL)//未找到
		{		
		}
		else if(sym->attrIR.kind!=varkind)//不是变量
		{	
		}
		else 
			ptr->child->symtPtr=sym;
	}
}
void Semantic::_Exp(ParseNode* ptr,typeIR* &tIR,int &ntype)
	//当ntype==0时，不需要对其进行判定，或者得从后面得到的类型进行相容处理
	//当ntype==1时，表明此类型要与sym的类型相容
	//当ntype==2时，表明此EXP序列，要与sym的自定义类型形成限制级关系，即为数组
{
	ParseNode* p;
	if((p=SearchChildFirst(ptr->child,Term,0)))//算术表达式的第一项
		_Term(p,tIR,ntype);
	if((p=SearchBrotherFirst(ptr->child,OtherTerm,0)))//算术表达式的其它项
		_OtherTerm(p,tIR,ntype);
}
void Semantic::_VariMore(ParseNode* ptr,SymTableNode* sym,typeIR* &tIR)
//表达式中的数组类型的内部元素
{
	ParseNode* p,q;
	int ntype=2;
	if((p=SearchBrotherFirst(ptr->child,Exp,0)))//数组下标计算表达式
	{
		tIR=sym->attrIR.idtype->More.ArrayAttr.indexTy;
		_Exp(p,tIR,ntype);//数组
	}

}
void Semantic::_Term(ParseNode* ptr,typeIR* &tIR,int &ntype)//算术表达式第一项
{
	ParseNode* p=0,*q=0;
	if((p=SearchChildFirst(ptr->child,Factor,0)))//因子
	{
		if((q=SearchBrotherFirst(p->child,Exp,0)))//表达式
			_Exp(q,tIR,ntype);
		else if((q=SearchChildFirst(p->child,Variable,0)))//当为数组为纪录时
			_Variable(q,tIR,ntype);
		else if(tIR&&p->child && (q=p->child)->parse.Symbol==INTC)//为整数常量时
			if(tIR!=IntTy&&ntype!=2)//类型出错////////////////////////////////中间代码生成时有修改
			{
			}
		if((q=SearchChildFirst(p->brother,OtherFactor,0)))//其它因子
			if((q=SearchBrotherFirst(q->child,Term,0)))
				_Term(q,tIR,ntype);
	}
}
void Semantic::_OtherTerm(ParseNode* ptr,typeIR* &tIR,int &ntype)//算术表达式中的其它项
{
	ParseNode* p;
	if((p=SearchBrotherFirst(ptr->child,Exp,1)))//其它项
		_Exp(p,tIR,ntype);
}
void Semantic::_Variable(ParseNode* ptr,typeIR* &tIR,int &ntype)//变量----------------在中间代码阶段进行了全面的修改
{
	ParseNode* p,*q;
	int type;
	if((p=SearchChildFirst(ptr->child,ID)))//为标识符----------------- p->parse.sym=id
	{
		SymTableNode* sym=myScope.FindID(p->parse.Parse_Str,1);//在本层逐次向上找
		if(sym)//找到
		{
			p->symtPtr=sym;//添加到语法树中，方便后续的中间代码生成等步骤
			if(sym->attrIR.kind!=varkind)//不为变量类型
			{
			}
			else//为变量------------        p=>   id
			{
				if(p->brother&&p->brother->child)//如果存在VaiMore
				{//--------------------------------------------------------------存在VariMore
					if((q=SearchBrotherFirst(p->brother->child,Exp,0)))
					{///为数组类型
						if(sym->attrIR.idtype->kind!=arrayTy)//本标识符并非数组类型
						{	
						}
						if(ntype && sym->attrIR.idtype->More.ArrayAttr.elemTy!=tIR)//类形不匹配
						{	
						}
						else//对数组下标算术表达式进行语义分析
						{
							typeIR* tIR0=IntTy;
							_Exp(q,tIR0,type=2);//q=exp  对q进行深入的语义分析
							if(ntype==0)//修改匹配类型
							{
								ntype=1;
								tIR=sym->attrIR.idtype->More.ArrayAttr.elemTy;//修改要匹配的类型
							}
						}
					}
				}//VariMore结束
				else
				{//---------------------------------------------------------------仅仅有Variable:=ID
					if(ntype==0)//如果是首项
					{
						tIR=sym->attrIR.idtype;//修改兼容类型
						ntype=1;
					}
					else if(ntype==1)//如果是1，则对其进行类型兼容审查
					{
						if(sym->attrIR.idtype!=tIR)//检测类型
						{	
						}
					}
					else if(ntype==2&&sym->attrIR.idtype!=IntTy)//检测是否为数组下标类型
					{	
					}
				}
			}//     p=>id  p=
		}//////找到对应的sym
		else //局部化符号表中未找到此标识符
		{	
		}
	}
}