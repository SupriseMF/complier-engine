#include"parse.h"
inline int getlex(const char *ch)
//对比LEXSTR,将得到的LEX编码返回,为下面的各函数形成TOKEN服务
{
	for(int i=0;strcmp(LEXSTR[i],"$")!=0;i++)
	{
		if(strcmp(ch,LEXSTR[i])==0)
			return i;
	}
	return -1;
}
//一个格式化输出函数
#define SPACESTR "              "
#define DOTSPACE ".             "
#define SLEN strlen(SPACESTR)
string format(const char* ch)
{
	string str(ch);
	int l=strlen(ch);
	for(int i=l;i<SLEN;i++)
		str+=' ';
	str[SLEN]+=0;
	return str;//.substr(0,SLEN-1);
}
string space_format(int layer)
{
	string str;
	for(int i=0;i<layer;i++)
		str+=DOTSPACE;
	//  str+=SPACESTR;
	return str;
}
void ParseNode::OutToCmd(int layer,bool type)
//参数说明，layer为树的层数，用以产生树的规整形状
//type为需要换行与否标志，type=true为需要换行
{
	//换行并且输出行号标识符
	if(type&&parse.Symbol!=-1)//type==true为需要换行，如果要输出的字串为空，那么不换行
		cout<<'\n'<<"line "<<parse.LineNum<<" :"<<space_format(layer);
	if(parse.Symbol!=-1)//当为空串时，不输出格式化字串
		cout<<format(parse.Parse_Str);//输出节点信息
	if(child)
		child->OutToCmd(layer+1,0);
	if(brother&&child)
		brother->OutToCmd(layer,1);
	else if(brother)
		brother->OutToCmd(layer,0);
}
void ParseNode::OutToFile(FILE* file,int layer,bool type)
//参数说明，layer为树的层数，用以产生树的规整形状
//type为需要换行与否标志，type=true为需要换行
{
	//换行并且输出行号标识符
	if(type&&parse.Symbol!=-1)//type==true为需要换行，如果要输出的字串为空，那么不换行
		fprintf(file,"\nline %d :%s",parse.LineNum,space_format(layer).c_str());
	if(parse.Symbol!=-1)//当为空串时，不输出格式化字串
		fprintf(file,"%s",format(parse.Parse_Str).c_str());//输出节点信息
	if(child)
		child->OutToFile(file,layer+1,0);
	if(brother)
		brother->OutToFile(file,layer,1);
}

//输出到CMD界面
void ParseTree::OutToCmd()
{
	if(root)
	{
		root->OutToCmd(0,1);
		cout<<endl;
	}
}
//树轮子以文件
void ParseTree::OutToFile(char* savefile)
{
	FILE* file=fopen(savefile,"w+");
	if(file)
	{
		if(root)
			this->root->OutToFile(file,0,1);//第零层开始
		else
			cerr<<"语法树为空！"<<endl;
		fclose(file);
	}
	else
		cerr<<"文件打开失败！"<<endl;

}

ParseNode* Recursive_Parse::error(const char *ch)
{
	char ErrorStr[SEM_MaxSize];
	ErrorNum++;
	sprintf(ErrorStr,"Error %d Line %d :syntax error :%s",
		ErrorNum,current->token.LineNum,ch);
	TokenNode* ptr=new TokenNode(0,0,"",ErrorStr);
	ErrorList.insertNode(ptr);
	return new ParseNode(-1,current->token.LineNum,"");
}
bool Recursive_Parse::outError(int ntype)
//ntype==1只输出语法出错信息
{
	bool b=0;
	if(ntype==1)
	{
		TokenNode* ptr=ErrorList.Front();
		while(ptr)
		{
			cout<<ptr->token.SemStr<<endl;
			ptr=ptr->next,b=1;
		}
	}
	return b;
}
bool Recursive_Parse::outErrorFile(const char* filename,int ntype)
{
	FILE* file=fopen(filename,"w+");
	if(0==file)
	{
		cerr<<"不能打开错误输出文件!"<<endl;
		return 0;
	}
	bool b=0;
	if(ntype==3)
	{
		TokenNode* ptr=ErrorList.Front();
		while(ptr)
		{
			fprintf(file,"%s\n",ptr->token.SemStr);
			ptr=ptr->next,b=1;
		}
	}
	fclose(file);
	return b;
}
//获取下一个TokenNode结点
void Recursive_Parse::GetNextToken()
{
	do
	{
		current=current->next;
		if(NULL==current)
		{
		//	throw 0;
			exit(0);
		}
	}
	while(current->token.Lex==ERROR1);
}
//判断一个终极符是否是在一个非终极符关于某产生式的预测集中
bool Recursive_Parse::Check_T_P(LEX T,int *p)
//这里的p是那个产生式的预测集
{
	for(int i=0;p[i]!=-1;i++)
	{
		if(T==p[i])
			return 1;
	}
	return 0;
}
//判断终极符是否匹配成功
bool Recursive_Parse::Match(ParseNode* &ptr,LEX T)
{
	if(current->token.Lex==T)
	{
		ptr=new ParseNode(T,current->token.LineNum,LEXSTR[T]);
		GetNextToken();
		return 1;
	}
	else
	{
		string str="此处缺少：";
		str+=LEXSTR[T];
		ptr=error(str.c_str());
		return 0;
	}
}
bool Recursive_Parse::parseScanner()
{
	TokenList* list;
	if(list=lexScanner.outTokenList())
			current=list->Front();
	if(0==current)
		return 0;
	if(current->token.Lex==ERROR1)
		GetNextToken();
	parseTree.root=f1();//调用产生式（1）得到了树根
	return 1;
}
//Program
ParseNode* Recursive_Parse::f1()
{
	ParseNode* ptr=new ParseNode(Program,current->token.LineNum,ParseStr[Program-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[1]))
	{
		ptr->child=f2();//形成一个儿子节点
		ptr->child->brother=f4();//形成一个儿子节点的兄弟节点，即下一个儿子节点
		ptr->child->brother->brother=f57();//再下一个儿子节点
	}
	else
		delete error(SES[Program-LOW]);//"程序头出错！");
	return ptr;
}
ParseNode* Recursive_Parse::f2()
{
	ParseNode* ptr=new ParseNode(ProgramHead,current->token.LineNum,ParseStr[ProgramHead-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[2]))
	{
		Match(ptr->child,PROGRAM);
		ptr->child->brother=f3();
	}
	else
		delete error(SES[ProgramHead-LOW]);
	return ptr;
}
//ID
ParseNode* Recursive_Parse::f3()
{
	ParseNode* ptr=new ParseNode(ProgramName,current->token.LineNum,ParseStr[ProgramName-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[3]))
	{
		ptr->child=new ParseNode(ID,current->token.LineNum,current->token.SemStr);
		GetNextToken();
	}
	else
		delete error(SES[ProgramName-LOW]);
	return ptr;
}
//DeclarePart
ParseNode* Recursive_Parse::f4()
{
	ParseNode* ptr=new ParseNode(DeclarePart,current->token.LineNum,ParseStr[DeclarePart-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[4]))
	{
		/***TypeDecPart***/
		ptr->child=f5();
		/****VarDecpart******/
		ptr->child->brother=f30();
		/*****ProcDecpart*******/
		ptr->child->brother->brother=f39();
	}
	else
		delete error(SES[DeclarePart-LOW]);
	return ptr;
}
//TypeDecpart
ParseNode* Recursive_Parse::f5()//空
{
	if(Check_T_P(current->token.Lex,PreSets[5]))
		return new ParseNode(-1,current->token.LineNum,"");
	else if(Check_T_P(current->token.Lex,PreSets[6]))
		return f6();
	else//当前TOKEN结点不在其预测集之内时，报错
		return error(SES[TypeDecpart-LOW]);
}
//TypeDecpart
ParseNode* Recursive_Parse::f6()
{
	ParseNode* ptr=new ParseNode(TypeDecpart,current->token.LineNum,ParseStr[TypeDecpart-LOW]);
	ptr->child=f7();
	return ptr;
}
//TypeDec
ParseNode* Recursive_Parse::f7()
{
	ParseNode* ptr=new ParseNode(TypeDec,current->token.LineNum,ParseStr[TypeDec-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[7]))
	{
		Match(ptr->child,TYPE);
		ptr->child->brother=f8();
	}
	else
		delete error(SES[TypeDec-LOW]);
	return ptr;
}
//TypeDecList
ParseNode* Recursive_Parse::f8()
{
	ParseNode* ptr=new ParseNode(TypeDecList,current->token.LineNum,ParseStr[TypeDecList-LOW]),
		*p;
	if( Check_T_P ( current->token.Lex,PreSets[8] ) )
	{
		p=ptr->child=f11();
		Match(p->brother,(LEX)getlex("="));//匹配=号
		p=p->brother;
		p=p->brother=f12();
		Match(p->brother,(LEX)getlex(";"));//匹配;号
		p=p->brother;
		p=p->brother=f9();
	}
	else
		delete error(SES[TypeDecList-LOW]);
	return ptr;
}
//TypeDecMore
ParseNode* Recursive_Parse::f9()
{
	if(Check_T_P(current->token.Lex,PreSets[9]))
		return new ParseNode(-1,current->token.LineNum,"");
	else if(Check_T_P(current->token.Lex,PreSets[10]))
		return f10();
	else 
		return error(SES[TypeDecMore-LOW]);
}
//TypeDecMore
ParseNode* Recursive_Parse::f10()
{
	ParseNode* ptr=new ParseNode(TypeDecMore,current->token.LineNum,ParseStr[TypeDecMore-LOW]);
	ptr->child=f8();
	return ptr;
}
//TypeId
ParseNode* Recursive_Parse::f11()
{
	ParseNode* ptr=new ParseNode(TypeId,current->token.LineNum,ParseStr[TypeId-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[11]))
	{
		ptr->child=new ParseNode(ID,current->token.LineNum,current->token.SemStr);
		GetNextToken();
	}
	else
		delete error(SES[TypeId-LOW]);
	return ptr;
}
//TypeDef
ParseNode* Recursive_Parse::f12()
{
	if(Check_T_P(current->token.Lex,PreSets[12]))
	{
		ParseNode* ptr=new ParseNode(TypeDef,current->token.LineNum,ParseStr[TypeDef-LOW]);
		ptr->child=f15();
		return ptr;
	}
	else if(Check_T_P(current->token.Lex,PreSets[13]))
		return f13();
	else if(Check_T_P(current->token.Lex,PreSets[14]))
		return f14();
	else
		return error(SES[TypeDef-LOW]);
}
//TypeDef
ParseNode* Recursive_Parse::f13()
{
	ParseNode* ptr=new ParseNode(TypeDef,current->token.LineNum,ParseStr[TypeDef-LOW]);
	ptr->child=f17();
	return ptr;
}
//TypeDef
ParseNode* Recursive_Parse::f14()
{
	ParseNode* ptr=new ParseNode(TypeDef,current->token.LineNum,ParseStr[TypeDef-LOW]);
	ptr->child=new ParseNode(ID,current->token.LineNum,current->token.SemStr);
	GetNextToken();
	return ptr;
}
//BaseType
ParseNode* Recursive_Parse::f15()
{
	if(Check_T_P(current->token.Lex,PreSets[15]))
	{
		ParseNode* ptr=new ParseNode(BaseType,current->token.LineNum,ParseStr[BaseType-LOW]);
		ptr->child=new ParseNode(INTEGER,current->token.LineNum,current->token.LexStr);
		GetNextToken();
		return ptr;
	}
	else if(Check_T_P(current->token.Lex,PreSets[16]))
		return f16();
	else
		return error(SES[BaseType-LOW]);
}
//BaseType
ParseNode* Recursive_Parse::f16()
{
	ParseNode* ptr=new ParseNode(BaseType,current->token.LineNum,ParseStr[BaseType-LOW]);
	ptr->child=new ParseNode(CHAR1,current->token.LineNum,current->token.LexStr);
	GetNextToken();
	return ptr;
}
//StructureType
ParseNode* Recursive_Parse::f17()
{
	if(Check_T_P(current->token.Lex,PreSets[17]))
	{
		ParseNode* ptr=
			new ParseNode(StructureType,current->token.LineNum,ParseStr[StructureType-LOW]);
		ptr->child=f19();
		return ptr;
	}
	else if(Check_T_P(current->token.Lex,PreSets[18]))
		return f18();
	else
		return error(SES[StructureType-LOW]);
}
//StructureType
ParseNode* Recursive_Parse::f18()
{
	ParseNode* ptr=
		new ParseNode(StructureType,current->token.LineNum,ParseStr[StructureType-LOW]);
	ptr->child=f22();
	return ptr;
}
//ArrayType
ParseNode* Recursive_Parse::f19()
{
	ParseNode* ptr=new ParseNode(ArrayType,current->token.LineNum,ParseStr[ArrayType-LOW]),
		*p;
	if(Check_T_P(current->token.Lex,PreSets[19]))
	{
		Match(ptr->child,ARRAY);
		p=ptr->child;
		Match(p->brother,(LEX)getlex("["));
		p=p->brother;
		p=p->brother=f20();
		Match(p->brother,(LEX)getlex(".."));
		p=p->brother;
		p=p->brother=f21();
		Match(p->brother,(LEX)getlex("]"));
		p=p->brother;
		Match(p->brother,OF);
		p=p->brother;
		p=p->brother=f15();
	}
	else
		delete error(SES[ArrayType-LOW]);
	return ptr;
}
//Low
ParseNode* Recursive_Parse::f20()
{
	ParseNode* ptr=new ParseNode(Low,current->token.LineNum,ParseStr[Low-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[20]))
	{
		ptr->child=new ParseNode(INTC,current->token.LineNum,current->token.SemStr);
		GetNextToken();
	}
	else
		delete error(SES[Low-LOW]);
	return ptr;
}
//Top
ParseNode* Recursive_Parse::f21()
{
	ParseNode* ptr=new ParseNode(Top,current->token.LineNum,ParseStr[Top-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[21]))
	{
		ptr->child=new ParseNode(INTC,current->token.LineNum,current->token.SemStr);
		GetNextToken();
	}
	else
		delete error(SES[Top-LOW]);
	return ptr;
}
//RecType
ParseNode* Recursive_Parse::f22()
{
	ParseNode* ptr=new ParseNode(RecType,current->token.LineNum,ParseStr[RecType-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[22]))
	{
		Match(ptr->child,RECORD);
		ptr->child->brother=f23();
		Match(ptr->child->brother->brother,END);
	}
	else
		delete error(SES[RecType-LOW]);
	return ptr;
}
//FieldDecList
ParseNode* Recursive_Parse::f23()
{
	if(Check_T_P(current->token.Lex,PreSets[23]))
	{
		ParseNode* ptr=
			new ParseNode(FieldDecList,current->token.LineNum,ParseStr[FieldDecList-LOW]),*p;
		p=ptr->child=f15();
		p=p->brother=f27();
		Match(p->brother,LEX(getlex(";")));
		p=p->brother;
		p->brother=f25();
		return ptr;
	} 
	else if(Check_T_P(current->token.Lex,PreSets[24]))
		return f24();
	else
		return 	error(SES[FieldDecList-LOW]);
}
//FieldDecList
ParseNode* Recursive_Parse::f24()
{
	ParseNode* ptr=
		new ParseNode(FieldDecList,current->token.LineNum,ParseStr[FieldDecList-LOW]),*p;
	p=ptr->child=f19();
	p=p->brother=f27();
	Match(p->brother,LEX(getlex(";")));
	p=p->brother;
	p->brother=f25();
	return ptr;
}
//FieldDecMore
ParseNode* Recursive_Parse::f25()
{
	if(Check_T_P(current->token.Lex,PreSets[25]))
	{
		ParseNode* ptr=new ParseNode(-1,current->token.LineNum,"");
		return ptr;
	} 
	else if(Check_T_P(current->token.Lex,PreSets[26]))
		return f26();
	else
		return 	error(SES[FieldDecMore-LOW]);
}
//FieldDecMore
ParseNode* Recursive_Parse::f26()
{
	ParseNode* ptr=
		new ParseNode(FieldDecMore,current->token.LineNum,ParseStr[FieldDecMore-LOW]);
	ptr->child=f23();
	return ptr;
}
//IdList
ParseNode* Recursive_Parse::f27()
{
	ParseNode* ptr=
		new ParseNode(IdList,current->token.LineNum,ParseStr[IdList-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[27]))
	{
		ptr->child=new ParseNode(ID,current->token.LineNum,current->token.SemStr);
		GetNextToken();
		ptr->child->brother=f28();
	}
	else
		delete error(SES[IdList-LOW]);
	return ptr;
}
//IdMore
ParseNode* Recursive_Parse::f28()
{
	if(Check_T_P(current->token.Lex,PreSets[28]))
	{
		ParseNode* ptr=new ParseNode(-1,current->token.LineNum,"");
		return ptr;
	} 
	else if(Check_T_P(current->token.Lex,PreSets[29]))
		return f29();
	else
		return 	error(SES[IdMore-LOW]);
}
//IdMore
ParseNode* Recursive_Parse::f29()
{
	ParseNode* ptr=new ParseNode(IdMore,current->token.LineNum,ParseStr[IdMore-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[29]))
	{
		Match(ptr->child,(LEX)getlex(","));
		ptr->child->brother=f27();
	}
	else
		delete error(SES[IdMore-LOW]);
	return ptr;
}
//VarDecpart
ParseNode* Recursive_Parse::f30()
{	
	if(Check_T_P(current->token.Lex,PreSets[30]))
	{
		ParseNode* ptr=new ParseNode(-1,current->token.LineNum,"");
		return ptr;
	} 
	else if(Check_T_P(current->token.Lex,PreSets[31]))
		return f31();
	else
		return 	error(SES[VarDecpart-LOW]);
}
//VarDecpart
ParseNode* Recursive_Parse::f31()
{
	ParseNode* ptr=new ParseNode(VarDecpart,current->token.LineNum,ParseStr[VarDecpart-LOW]);
	Match(ptr->child,VAR);
	ptr->child->brother=f33();
	return ptr;
}
//VarDec
ParseNode* Recursive_Parse::f32()
{
	ParseNode* ptr=new ParseNode(VarDec,current->token.LineNum,ParseStr[VarDec-LOW]),
		*p;
	if(Check_T_P(current->token.Lex,PreSets[32]))
	{
		Match(ptr->child,VAR);
		p=f33();
	}
	else
		delete error(SES[VarDec-LOW]);
	return ptr;
}
//VarDecList
ParseNode* Recursive_Parse::f33()
{
	ParseNode* ptr=new ParseNode(VarDecList,current->token.LineNum,ParseStr[VarDecList-LOW]),
		*p;
	if(Check_T_P(current->token.Lex,PreSets[33]))
	{
		p=ptr->child=f12();
		p=p->brother=f36();
		Match(p->brother,(LEX)getlex(";"));
		p=p->brother;
		p->brother=f34();
	}
	else
		delete error(SES[VarDecList-LOW]);
	return ptr;
}
//VarDecMore
ParseNode* Recursive_Parse::f34()
{	
	if(Check_T_P(current->token.Lex,PreSets[34]))
	{
		ParseNode* ptr=new ParseNode(-1,current->token.LineNum,"");
		return ptr;
	} 
	else if(Check_T_P(current->token.Lex,PreSets[35]))
		return f35();
	else
		return error(SES[VarDecMore-LOW]);
}
//VarDecMore
ParseNode* Recursive_Parse::f35()
{
	ParseNode* ptr=new ParseNode(VarDecMore,current->token.LineNum,ParseStr[VarDecMore-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[35]))
		ptr->child=f33();
	else
		delete error(SES[VarDecMore-LOW]);
	return ptr;
}
//VarIdList
ParseNode* Recursive_Parse::f36()
{
	ParseNode* ptr=new ParseNode(VarIdList,current->token.LineNum,ParseStr[VarIdList-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[36]))
	{
		ptr->child=new ParseNode(ID,current->token.LineNum,current->token.SemStr);
		GetNextToken();
		ptr->child->brother=f37();
	}
	else
		delete error(SES[VarIdList-LOW]);
	return ptr;
}
//VarIdMore
ParseNode* Recursive_Parse::f37()
{	
	if(Check_T_P(current->token.Lex,PreSets[37]))
	{
		ParseNode* ptr=new ParseNode(-1,current->token.LineNum,"");
		return ptr;
	} 
	else if(Check_T_P(current->token.Lex,PreSets[38]))
		return f38();
	else
		return 	error(SES[VarIdMore-LOW]);
}
//VarIdMore
ParseNode* Recursive_Parse::f38()
{
	
	ParseNode* ptr=new ParseNode(VarIdMore,current->token.LineNum,ParseStr[VarIdMore-LOW]);
	Match(ptr->child,(LEX)getlex(","));
	ptr->child->brother=f36();
	return ptr;
}
//ProcDecpart
ParseNode* Recursive_Parse::f39()
{
	if(Check_T_P(current->token.Lex,PreSets[39]))
	{
		ParseNode* ptr=new ParseNode(-1,current->token.LineNum,"");
		return ptr;
	} 
	else if(Check_T_P(current->token.Lex,PreSets[40]))
		return f40();
	else
		return 	error(SES[ProcDecpart-LOW]);
}
//ProcDecpart
ParseNode* Recursive_Parse::f40()
{
	
	ParseNode* ptr=new ParseNode(ProcDecpart,current->token.LineNum,ParseStr[ProcDecpart-LOW]);
	ptr->child=f41();
	return ptr;
}
//ProcDec,修正了一下关于ProcDecpart 与ProcDecPart在产生式中以及其它地方的混淆情形 
ParseNode* Recursive_Parse::f41()
{
	ParseNode* ptr=new ParseNode(ProcDec,current->token.LineNum,ParseStr[ProcDec-LOW]),*p;
	if(Check_T_P(current->token.Lex,PreSets[41]))
	{
		Match(ptr->child,PROCEDURE);
		p=ptr->child->brother=f44();//ProcName
		Match(p->brother,(LEX)getlex("("));
		p=p->brother->brother=f45();//ParamList
		Match(p->brother,(LEX)getlex(")"));
		Match(p->brother->brother,(LEX)getlex(";"));
		p=p->brother->brother->brother=f55();//ProcDecPart
		p=p->brother=f56();//ProcBody
		p=p->brother=f42();//ProcDecMore
	}
	else
		error(SES[ProcDec-LOW]);
	return ptr;
}
//ProcDecMore
ParseNode* Recursive_Parse::f42()
{	
	if(Check_T_P(current->token.Lex,PreSets[42]))
	{
		ParseNode* ptr=new ParseNode(-1,current->token.LineNum,"");
		return ptr;
	} 
	else if(Check_T_P(current->token.Lex,PreSets[43]))
		return f43();
	else
		return 	error(SES[ProcDecMore-LOW]);
}
//ProcDecMore
ParseNode* Recursive_Parse::f43()
{
	
	ParseNode* ptr=new ParseNode(ProcDecMore,current->token.LineNum,ParseStr[ProcDecMore-LOW]);
	ptr->child=f41();//注意这里
	return ptr;
}
//ProcName
ParseNode* Recursive_Parse::f44()
{
	ParseNode* ptr=new ParseNode(ProcName,current->token.LineNum,ParseStr[ProcName-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[44]))
	{
		ptr->child=new ParseNode(ID,current->token.LineNum,current->token.SemStr);
		GetNextToken();
	}
	else
		delete error(SES[ProcName-LOW]);
	return ptr;
}
//ParamList
ParseNode* Recursive_Parse::f45()
{	
	if(Check_T_P(current->token.Lex,PreSets[45]))
	{
		ParseNode* ptr=new ParseNode(-1,current->token.LineNum,"");
		return ptr;
	} 
	else if(Check_T_P(current->token.Lex,PreSets[46]))
		return f46();
	else
		return 	error(SES[ParamList-LOW]);
}
//ParamList
ParseNode* Recursive_Parse::f46()
{
	ParseNode* ptr=new ParseNode(ParamList,current->token.LineNum,ParseStr[ParamList-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[46]))
		ptr->child=f47();
	else
	{
		
		delete error(SES[ParamList-LOW]);
	}
	return ptr;
}
//ParamDecList
ParseNode* Recursive_Parse::f47()
{
	ParseNode* ptr=
		new ParseNode(ParamDecList,current->token.LineNum,ParseStr[ParamDecList-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[47]))
	{
		ptr->child=f50();
		ptr->child->brother=f48();
	}
	else
		delete error(SES[ParamDecList-LOW]);
	return ptr;
}
//ParamMore
ParseNode* Recursive_Parse::f48()
{	
	if(Check_T_P(current->token.Lex,PreSets[48]))
	{
		ParseNode* ptr=new ParseNode(-1,current->token.LineNum,"");
		return ptr;
	} 
	else if(Check_T_P(current->token.Lex,PreSets[49]))
		return f49();
	else
		return 	error(SES[ParamMore-LOW]);
}
//ParamMore
ParseNode* Recursive_Parse::f49()
{
	ParseNode* ptr=new ParseNode(ParamMore,current->token.LineNum,ParseStr[ParamMore-LOW]);
	Match(ptr->child,(LEX)getlex(";"));
	ptr->child->brother=f47();
	return ptr;
}
//Param
ParseNode* Recursive_Parse::f50()
{	
	if(Check_T_P(current->token.Lex,PreSets[50]))
	{
		ParseNode* ptr=new ParseNode(Param,current->token.LineNum,ParseStr[Param-LOW]);
		ptr->child=f12();
		ptr->child->brother=f52();
		return ptr;
	} 
	else if(Check_T_P(current->token.Lex,PreSets[51]))
		return f51();
	else
		return 	error(SES[Param-LOW]);
}
//Param
ParseNode* Recursive_Parse::f51()
{	
	
	ParseNode* ptr=new ParseNode(Param,current->token.LineNum,ParseStr[Param-LOW]);
	Match(ptr->child,VAR);
	ptr->child->brother=f12();
	ptr->child->brother=f52();
	return ptr;
}
//FormList
ParseNode* Recursive_Parse::f52()
{	
	ParseNode* ptr=new ParseNode(FormList,current->token.LineNum,ParseStr[FormList-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[52]))
	{
		ptr->child=new ParseNode(ID,current->token.LineNum,current->token.SemStr);
		GetNextToken();
		ptr->child->brother=f53();
	}
	else
		delete error(SES[FormList-LOW]);
	return ptr;
}
//FidMore
ParseNode* Recursive_Parse::f53()
{	
	if(Check_T_P(current->token.Lex,PreSets[53]))
		return new ParseNode(-1,current->token.LineNum,"");
	else if(Check_T_P(current->token.Lex,PreSets[54]))
		return f54();
	else
		return 	error(SES[FidMore-LOW]);
}
//FidMore
ParseNode* Recursive_Parse::f54()
{	
	ParseNode* ptr=new ParseNode(FidMore,current->token.LineNum,ParseStr[FidMore-LOW]);
	Match(ptr->child,(LEX)getlex(","));
	ptr->child->brother=f52();
	return ptr;
}
//ProcDecPart
ParseNode* Recursive_Parse::f55()
{
	ParseNode* ptr=new ParseNode(ProcDecPart,current->token.LineNum,ParseStr[ProcDecPart-LOW]);	
	if(Check_T_P(current->token.Lex,PreSets[55]))
		ptr->child=f4();
	else
		delete error(SES[ProcDecPart-LOW]);
	return ptr;
}
//ProcBody
ParseNode* Recursive_Parse::f56()
{
	ParseNode* ptr=new ParseNode(ProcBody,current->token.LineNum,ParseStr[ProcBody-LOW]);	
	ptr->child=f57();
	return ptr;
}
//ProgramBody
ParseNode* Recursive_Parse::f57()
{
	ParseNode* ptr=new ParseNode(ProgramBody,current->token.LineNum,ParseStr[ProgramBody-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[57]))
	{
		Match(ptr->child,BEGIN);
		ptr->child->brother=f58();
		Match(ptr->child->brother->brother,END);
	}
	else
		delete error(SES[ProgramBody-LOW]);
	return ptr;
}
//StmList
ParseNode* Recursive_Parse::f58()
{
	ParseNode* ptr=new ParseNode(StmList,current->token.LineNum,ParseStr[StmList-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[58]))
	{
		ptr->child=f61();
		ptr->child->brother=f59();
	}
	else
		delete error(SES[StmList-LOW]);
	return ptr;
}
//StmMore
ParseNode* Recursive_Parse::f59()
{	
	if(Check_T_P(current->token.Lex,PreSets[59]))
		return new ParseNode(-1,current->token.LineNum,"");
	else if(Check_T_P(current->token.Lex,PreSets[60]))
		return f60();
	else
		return 	error(SES[StmMore-LOW]);
}
//StmMore
ParseNode* Recursive_Parse::f60()
{
	ParseNode* ptr=new ParseNode(StmMore,current->token.LineNum,ParseStr[StmMore-LOW]);	
	Match(ptr->child,(LEX)getlex(";"));
	ptr->child=f58();
	return ptr;
}
//Stm
ParseNode* Recursive_Parse::f61()
{	
	if(Check_T_P(current->token.Lex,PreSets[61]))
	{
		ParseNode* ptr=new ParseNode(Stm,current->token.LineNum,ParseStr[Stm-LOW]);
		ptr->child=f70();
		return ptr;
	}
	else if(Check_T_P(current->token.Lex,PreSets[62]))
		return f62();
	else if(Check_T_P(current->token.Lex,PreSets[63]))
		return f63();
	else if(Check_T_P(current->token.Lex,PreSets[64]))
		return f64();
	else if(Check_T_P(current->token.Lex,PreSets[65]))
		return f65();
	else if(Check_T_P(current->token.Lex,PreSets[66]))
		return f66();
	else
		return 	new ParseNode(-1,current->token.LineNum,"");
}
//Stm
ParseNode* Recursive_Parse::f62()
{
	ParseNode* ptr=new ParseNode(Stm,current->token.LineNum,ParseStr[Stm-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[62]))
		ptr->child=f71();
	else
		delete error(SES[Stm-LOW]);
	return ptr;
}
//Stm
ParseNode* Recursive_Parse::f63()
{
	ParseNode* ptr=new ParseNode(Stm,current->token.LineNum,ParseStr[Stm-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[63]))
		ptr->child=f72();
	else
		delete error(SES[Stm-LOW]);
	return ptr;
}
//Stm
ParseNode* Recursive_Parse::f64()
{
	ParseNode* ptr=new ParseNode(Stm,current->token.LineNum,ParseStr[Stm-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[64]))
		ptr->child=f74();
	else
		delete error(SES[Stm-LOW]);
	return ptr;
}
//Stm
ParseNode* Recursive_Parse::f65()
{
	ParseNode* ptr=new ParseNode(Stm,current->token.LineNum,ParseStr[Stm-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[65]))
		ptr->child=f75();
	else
		delete error(SES[Stm-LOW]);
	return ptr;
}
//Stm
ParseNode* Recursive_Parse::f66()
{
	ParseNode* ptr=new ParseNode(Stm,current->token.LineNum,ParseStr[Stm-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[66]))
	{
		ptr->child=new ParseNode(ID,current->token.LineNum,current->token.SemStr);
		GetNextToken();
		ptr->child=f67();
	}
	else
		delete error(SES[Stm-LOW]);
	return ptr;
}
//AssCall
ParseNode* Recursive_Parse::f67()
{
	if(Check_T_P(current->token.Lex,PreSets[67]))
	{
		ParseNode* ptr=new ParseNode(AssCall,current->token.LineNum,ParseStr[AssCall-LOW]);
		ptr->child=f69();
		return ptr;
	}
	else if(Check_T_P(current->token.Lex,PreSets[68]))
		return f68();
	else
		//
		//cerr<<"Error "<<ErrorNum<<" Line "<<current->token.LineNum
	//	<<" :syntax error :"<<"非法AssCall语句！"<<endl;
		return 	new ParseNode(-1,current->token.LineNum,"");
}
//AssCall
ParseNode* Recursive_Parse::f68()
{
	ParseNode* ptr=new ParseNode(AssCall,current->token.LineNum,ParseStr[AssCall-LOW]);	
	ptr->child=f76();
	return ptr;
}
///////////////////////////////////////////////////////////////////////////
//AssignmentRest
ParseNode* Recursive_Parse::f69()
{
	ParseNode* ptr=
		new ParseNode(AssignmentRest,current->token.LineNum,ParseStr[AssignmentRest-LOW]),*p;
	if(Check_T_P(current->token.Lex,PreSets[69]))
	{
		p=ptr->child=f93();
		Match(p->brother,LEX(getlex(":=")));
		p=p->brother;
		p->brother=f83();
	} 
	else
		delete error(SES[AssignmentRest-LOW]);
	return ptr;
}
//ConditionalStm
ParseNode* Recursive_Parse::f70()
{
	ParseNode* ptr=
			new ParseNode(ConditionalStm,current->token.LineNum,ParseStr[ConditionalStm-LOW]),*p;
	if(Check_T_P(current->token.Lex,PreSets[70]))
	{
		Match(ptr->child,IF);
		p=ptr->child->brother=f81();
		Match(p->brother,THEN);
		p=p->brother;
		p=p->brother=f58();
		Match(p->brother,ELSE);
		p=p->brother;
		p=p->brother=f58();
		Match(p->brother,FI);
	}
	else
		delete error(SES[ConditionalStm-LOW]);
	return ptr;
}
//LoopStm
ParseNode* Recursive_Parse::f71()
{
	ParseNode* ptr=new ParseNode(LoopStm,current->token.LineNum,ParseStr[LoopStm-LOW]),
		*p;
	if(Check_T_P(current->token.Lex,PreSets[71]))
	{
		Match(ptr->child,WHILE);
		p=ptr->child->brother=f81();
		Match(p->brother,DO);
		p=p->brother;
		p=p->brother=f58();
		Match(p->brother,ENDWH);
	}
	else
		delete error(SES[LoopStm-LOW]);
	return ptr;
}
//InputStm
ParseNode* Recursive_Parse::f72()
{
	ParseNode* ptr=new ParseNode(InputStm,current->token.LineNum,ParseStr[InputStm-LOW]),
		*p;
	if(Check_T_P(current->token.Lex,PreSets[72]))
	{
		Match(ptr->child,READ);
		p=ptr->child;
		Match(p->brother,(LEX)getlex("("));
		p=p->brother;
		p=p->brother=f73();
		Match(p->brother,(LEX)getlex(")"));
	}
	else
		delete error(SES[InputStm-LOW]);
	return ptr;
}
//Invar
ParseNode* Recursive_Parse::f73()
{
	ParseNode* ptr=new ParseNode(Invar,current->token.LineNum,ParseStr[Invar-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[73]))
	{
		ptr->child=new ParseNode(ID,current->token.LineNum,current->token.SemStr);
		GetNextToken();
	}
	else
		delete error(SES[Invar-LOW]);
	return ptr;
}
//OutputStm
ParseNode* Recursive_Parse::f74()
{
	ParseNode* ptr=new ParseNode(OutputStm,current->token.LineNum,ParseStr[OutputStm-LOW]),
		*p;
	if(Check_T_P(current->token.Lex,PreSets[74]))
	{
		Match(ptr->child,WRITE);
		p=ptr->child;
		Match(p->brother,(LEX)getlex("("));
		p=p->brother;
		p=p->brother=f83();
		Match(p->brother,(LEX)getlex(")"));
	}
	else
		delete error(SES[OutputStm-LOW]);
	return ptr;
}
//ReturnStm
ParseNode* Recursive_Parse::f75()
{
	ParseNode* ptr=new ParseNode(ReturnStm,current->token.LineNum,ParseStr[ReturnStm-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[75]))
		Match(ptr->child,RETURN);
	else
		delete error(SES[ReturnStm-LOW]);
	return ptr;
}
//CallStmRest
ParseNode* Recursive_Parse::f76()
{
	ParseNode* ptr=new ParseNode(CallStmRest,current->token.LineNum,ParseStr[CallStmRest-LOW]),
		*p;
	if(Check_T_P(current->token.Lex,PreSets[76]))
	{
		Match(ptr->child,(LEX)getlex("("));
		p=ptr->child;
		p=p->brother=f77();
		Match(p->brother,(LEX)getlex(")"));
	}
	else
		delete error(SES[CallStmRest-LOW]);
	return ptr;
}
//ActParamList
ParseNode* Recursive_Parse::f77()
{
	if(Check_T_P(current->token.Lex,PreSets[77]))
		return new ParseNode(-1,current->token.LineNum,"");
	else if(Check_T_P(current->token.Lex,PreSets[78]))
		return f78();
	else
		return 	error(SES[ActParamList-LOW]);
}
//ActParamList
ParseNode* Recursive_Parse::f78()
{
	ParseNode* ptr=new ParseNode(ActParamList,current->token.LineNum,ParseStr[ActParamList-LOW]),*p;
	p=ptr->child=f83();
	p->brother=f79();
	return ptr;
}
//ActParamMore
ParseNode* Recursive_Parse::f79()
{
	if(Check_T_P(current->token.Lex,PreSets[79]))
		return new ParseNode(-1,current->token.LineNum,"");
	else if(Check_T_P(current->token.Lex,PreSets[80]))
		return f80();
	else
		return 	error(SES[ActParamMore-LOW]);
}
//ActParamMore
ParseNode* Recursive_Parse::f80()
{
	ParseNode* ptr=new ParseNode(ActParamMore,current->token.LineNum,ParseStr[ActParamMore-LOW]);
	Match(ptr->child,(LEX)getlex(","));
	ptr->child->brother=f77();
	return ptr;
}
//RelExp
ParseNode* Recursive_Parse::f81()
{
	ParseNode* ptr=new ParseNode(RelExp,current->token.LineNum,ParseStr[RelExp-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[81]))
	{
		ptr->child=f83();
		ptr->child->brother=f82();
	}
	else
		delete error(SES[RelExp-LOW]);
	return ptr;
}
//OtherRelE
ParseNode* Recursive_Parse::f82()
{
	ParseNode* ptr=new ParseNode(OtherRelE,current->token.LineNum,ParseStr[OtherRelE-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[82]))
	{
		ptr->child=f99();
		ptr->child->brother=f83();
	}
	else
		delete error(SES[OtherRelE-LOW]);
	return ptr;
}
//Exp
ParseNode* Recursive_Parse::f83()
{
	ParseNode* ptr=new ParseNode(Exp,current->token.LineNum,ParseStr[Exp-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[83]))
	{
		ptr->child=f86();
		ptr->child->brother=f84();
	}
	else
		delete error(SES[Exp-LOW]);
	return ptr;
}
//OtherTerm
ParseNode* Recursive_Parse::f84()
{
	if(Check_T_P(current->token.Lex,PreSets[84]))
		return new ParseNode(-1,current->token.LineNum,"");
	else if(Check_T_P(current->token.Lex,PreSets[85]))
		return f85();
	else
		return 	error(SES[OtherTerm-LOW]);
}
//OtherTerm
ParseNode* Recursive_Parse::f85()
{
	ParseNode* ptr=new ParseNode(OtherTerm,current->token.LineNum,ParseStr[OtherTerm-LOW]);
	ptr->child=f101();
	ptr->child->brother=f83();
	return ptr;
}
//Term
ParseNode* Recursive_Parse::f86()
{
	ParseNode* ptr=new ParseNode(Term,current->token.LineNum,ParseStr[Term-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[86]))
	{
		ptr->child=f89();
		ptr->child->brother=f87();
	}
	else
		delete error(SES[Term-LOW]);
	return ptr;
}
//OtherFactor
ParseNode* Recursive_Parse::f87()
{
	if(Check_T_P(current->token.Lex,PreSets[87]))
		return new ParseNode(-1,current->token.LineNum,"");
	else if(Check_T_P(current->token.Lex,PreSets[88]))
		return f88();
	else
		return 	error(SES[OtherFactor-LOW]);
}
//OtherFactor
ParseNode* Recursive_Parse::f88()
{
	ParseNode* ptr=new ParseNode(OtherFactor,current->token.LineNum,ParseStr[OtherFactor-LOW]);
	ptr->child=f103();
	ptr->child->brother=f86();
	return ptr;
}
//Factor
ParseNode* Recursive_Parse::f89()
{
	
	if(Check_T_P(current->token.Lex,PreSets[89]))
	{
		ParseNode* ptr=new ParseNode(Factor,current->token.LineNum,ParseStr[Factor-LOW]),*p;
		Match(ptr->child,(LEX)getlex("("));
		p=ptr->child;
		p=p->brother=f83();
		Match(p->brother,(LEX)getlex(")"));
		return ptr;
	}
	else if(Check_T_P(current->token.Lex,PreSets[90]))
		return f90();
	else if(Check_T_P(current->token.Lex,PreSets[91]))
		return f91();
	else
		return error(SES[Factor-LOW]);
}
//Factor
ParseNode* Recursive_Parse::f90()
{
	ParseNode* ptr=new ParseNode(Factor,current->token.LineNum,ParseStr[Factor-LOW]);
	ptr->child=new ParseNode(INTC,current->token.LineNum,current->token.SemStr);
	GetNextToken();
	return ptr;
}
//Factor
ParseNode* Recursive_Parse::f91()
{
	ParseNode* ptr=new ParseNode(Factor,current->token.LineNum,ParseStr[Factor-LOW]);
	ptr->child=f92();
	return ptr;
}
//Variable
ParseNode* Recursive_Parse::f92()
{
	ParseNode* ptr=new ParseNode(Variable,current->token.LineNum,ParseStr[Variable-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[92]))
	{
		ptr->child=new ParseNode(ID,current->token.LineNum,current->token.SemStr);
		GetNextToken();
		ptr->child->brother=f93();
	}
	else
		delete error(SES[Variable-LOW]);
	return ptr;
}
//VariMore
ParseNode* Recursive_Parse::f93()
{
	if(Check_T_P(current->token.Lex,PreSets[93]))
		return  new ParseNode(-1,current->token.LineNum,"");
	else if(Check_T_P(current->token.Lex,PreSets[94]))
		return f94();
	else if(Check_T_P(current->token.Lex,PreSets[95]))
		return f95();
	else
		return 	error(SES[VariMore-LOW]);
}
//VariMore
ParseNode* Recursive_Parse::f94()
{
    ParseNode* ptr=new ParseNode(VariMore,current->token.LineNum,ParseStr[VariMore-LOW]),
		*p;
	Match(ptr->child,(LEX)getlex("["));
	p=ptr->child->brother=f83();
	Match(p->brother,(LEX)getlex("]"));
	return ptr;
}
//VariMore
ParseNode* Recursive_Parse::f95()
{
	ParseNode* ptr=new ParseNode(VariMore,current->token.LineNum,ParseStr[VariMore-LOW]);
	Match(ptr->child,(LEX)getlex("."));
	ptr->child->brother=f96();
	return ptr;
}
//FieldVar
ParseNode* Recursive_Parse::f96()
{
	ParseNode* ptr=new ParseNode(FieldVar,current->token.LineNum,ParseStr[FieldVar-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[96]))
	{
		ptr->child=new ParseNode(ID,current->token.LineNum,current->token.SemStr);
		GetNextToken();
		ptr->child->brother=f97();
	}
	else
		delete error(SES[FieldVar-LOW]);
	return ptr;
}
//FieldVarMore
ParseNode* Recursive_Parse::f97()
{
	if(Check_T_P(current->token.Lex,PreSets[97]))
		return new ParseNode(-1,current->token.LineNum,"");
	else if(Check_T_P(current->token.Lex,PreSets[98]))
		return f98();
	else
		return 	error(SES[FieldVarMore-LOW]);
}
//FieldVarMore
ParseNode* Recursive_Parse::f98()
{
	ParseNode* ptr=new ParseNode(FieldVarMore,current->token.LineNum,ParseStr[FieldVarMore-LOW]),*p;
	Match(ptr->child,(LEX)getlex("["));
	p=ptr->child;
	p=p->brother=f83();
	Match(p->brother,(LEX)getlex("]"));
	return ptr;
}
//CmpOp
ParseNode* Recursive_Parse::f99()
{
	if(Check_T_P(current->token.Lex,PreSets[99]))
	{
		ParseNode* ptr=new ParseNode(CmpOp,current->token.LineNum,ParseStr[CmpOp-LOW]);
		Match(ptr->child,(LEX)getlex("<"));
		return ptr;
	}
	else if(Check_T_P(current->token.Lex,PreSets[100]))
		return f100();
	else 
		return error(SES[CmpOp-LOW]);
}
//CmpOp
ParseNode* Recursive_Parse::f100()
{
	ParseNode* ptr=new ParseNode(CmpOp,current->token.LineNum,ParseStr[CmpOp-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[100]))
		Match(ptr->child,(LEX)getlex("="));
	else
		delete error(SES[CmpOp-LOW]);
	return ptr;
}
//AddOp
ParseNode* Recursive_Parse::f101()
{
	if(Check_T_P(current->token.Lex,PreSets[101]))
	{
		ParseNode* ptr=new ParseNode(AddOp,current->token.LineNum,ParseStr[AddOp-LOW]);
		Match(ptr->child,(LEX)getlex("+"));
		return ptr;
	}
	else if(Check_T_P(current->token.Lex,PreSets[102]))
		return f102();
	else
		return error(SES[AddOp-LOW]);
}
//AddOp
ParseNode* Recursive_Parse::f102()
{
	ParseNode* ptr=new ParseNode(AddOp,current->token.LineNum,ParseStr[AddOp-LOW]);
	Match(ptr->child,(LEX)getlex("-"));
	return ptr;
}
//MultOp
ParseNode* Recursive_Parse::f103()
{
	if(Check_T_P(current->token.Lex,PreSets[103]))
	{
		ParseNode* ptr=new ParseNode(MultOp,current->token.LineNum,ParseStr[MultOp-LOW]);
		Match(ptr->child,(LEX)getlex("*"));
		return ptr;
	}
	else if(Check_T_P(current->token.Lex,PreSets[104]))
		return f104();
	else
		return error(SES[MultOp-LOW]);
}
//MultOp
ParseNode* Recursive_Parse::f104()
{
	ParseNode* ptr=new ParseNode(AddOp,current->token.LineNum,ParseStr[AddOp-LOW]);
	if(Check_T_P(current->token.Lex,PreSets[104]))
		Match(ptr->child,(LEX)getlex("/"));
	else
		delete error(SES[MultOp-LOW]);
	return ptr;
}
