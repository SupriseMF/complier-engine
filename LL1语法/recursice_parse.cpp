#include"parse.h"
inline int getlex(const char *ch)
{
	for(int i=0;strcmp(LEXSTR[i],"$")!=0;i++)
	{
		if(strcmp(ch,LEXSTR[i])==0)
			return i;
	}
	return -1;
}
#define SPACESTR "              "
#define SLEN strlen(SPACESTR)
string format(const char* ch)
{
	string str(ch);
	int l=strlen(ch);
	for(int i=l;i<SLEN;i++)
		str+=' ';
	str[SLEN]+=0;
	return str;
}
string space_format(int layer)
{
	string str;
	for(int i=0;i<layer;i++)
		str+="       ";
	return str;
}

void ParseNode::OutToFile(FILE* file,int layer,bool type)
{
	if(type&&parse.Symbol!=-1)
		fprintf(file,"\nline %d :%s",parse.LineNum,space_format(layer).c_str());
	if(parse.Symbol!=-1)
		fprintf(file,"%s",format(parse.Parse_Str).c_str());
	if(child)
		child->OutToFile(file,layer+1,0);
	if(brother)
		brother->OutToFile(file,layer,1);
}
void ParseTree::OutToFile(char* savefile)
{
	FILE* file=fopen(savefile,"w+");
	if(file)
	{
		if(root)
			root->OutToFile(file,0,1);
		else
			cerr<<"语法树为空!"<<endl;
		fclose(file);
	}
	else
		cerr<<"文件打开失败!"<<endl;

}
void Recursive_Parse::GetNextToken()
{
	do
	{
		current=current->next;
		if(current==NULL)
		{
			exit(0);
		}
	}
	while(current->token.Lex==ERROR1);
}
//判断一个终极符是否是在一个非终极符关于某产生式的预测集中
bool Recursive_Parse::Check_T_P(LEX T,int *p)
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
		cout<<str<<endl;
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
	parseTree.root=f1();
	return 1;
}
//Program
ParseNode* Recursive_Parse::f1()
{
	ParseNode* ptr=new ParseNode(Program,current->token.LineNum,ParseStr[Program-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[1]))
	{
		ptr->child=f2();
		ptr->child->brother=f4();
		ptr->child->brother->brother=f57();
	}
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[Program-LINE]);
	return ptr;
}
ParseNode* Recursive_Parse::f2()
{
	ParseNode* ptr=new ParseNode(ProgramHead,current->token.LineNum,ParseStr[ProgramHead-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[2]))
	{
		Match(ptr->child,PROGRAM);
		ptr->child->brother=f3();
	}
	else
	    printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[ProgramHead-LINE]);
	return ptr;
}
//ID
ParseNode* Recursive_Parse::f3()
{
	ParseNode* ptr=new ParseNode(ProgramName,current->token.LineNum,ParseStr[ProgramName-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[3]))
	{
		ptr->child=new ParseNode(ID,current->token.LineNum,current->token.SemStr);
		GetNextToken();
	}
	else
	 printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[ProgramName-LINE]);
	return ptr;
}
//DeclarePart
ParseNode* Recursive_Parse::f4()
{
	ParseNode* ptr=new ParseNode(DeclarePart,current->token.LineNum,ParseStr[DeclarePart-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[4]))
	{
		ptr->child=f5();
		/****VarDecpart******/
		ptr->child->brother=f30();
		/*****ProcDecpart*******/
		ptr->child->brother->brother=f39();
	}
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[DeclarePart-LINE]);
	return ptr;
}
//TypeDecpart
ParseNode* Recursive_Parse::f5()
{
	if(Check_T_P(current->token.Lex,PreSets[5]))
		return new ParseNode(-1,current->token.LineNum,"");
	else if(Check_T_P(current->token.Lex,PreSets[6]))
		return f6();
	else
	{
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[DeclarePart-LINE]);
		return 0;
	}
}
//TypeDecpart
ParseNode* Recursive_Parse::f6()
{
	ParseNode* ptr=new ParseNode(TypeDecpart,current->token.LineNum,ParseStr[TypeDecpart-LINE]);
	ptr->child=f7();
	return ptr;
}
//TypeDec
ParseNode* Recursive_Parse::f7()
{
	ParseNode* ptr=new ParseNode(TypeDec,current->token.LineNum,ParseStr[TypeDec-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[7]))
	{
		Match(ptr->child,TYPE);
		ptr->child->brother=f8();
	}
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[TypeDec-LINE]);
	return ptr;
}
//TypeDecList
ParseNode* Recursive_Parse::f8()
{
	ParseNode* ptr=new ParseNode(TypeDecList,current->token.LineNum,ParseStr[TypeDecList-LINE]),
		*p;
	if(Check_T_P(current->token.Lex,PreSets[8] ) )
	{
		p=ptr->child=f11();
		Match(p->brother,(LEX)getlex("="));
		p=p->brother;
		p=p->brother=f12();
		Match(p->brother,(LEX)getlex(";"));
		p=p->brother;
		p=p->brother=f9();
	}
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[TypeDecList-LINE]);
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
	{
       printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[TypeDecMore-LINE]);	
    	return 0;
	}
}
//TypeDecMore
ParseNode* Recursive_Parse::f10()
{
	ParseNode* ptr=new ParseNode(TypeDecMore,current->token.LineNum,ParseStr[TypeDecMore-LINE]);
	ptr->child=f8();
	return ptr;
}
//TypeId
ParseNode* Recursive_Parse::f11()
{
	ParseNode* ptr=new ParseNode(TypeId,current->token.LineNum,ParseStr[TypeId-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[11]))
	{
		ptr->child=new ParseNode(ID,current->token.LineNum,current->token.SemStr);
		GetNextToken();
	}
	else
	printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[TypeId-LINE]);
	return ptr;
}
//TypeDef
ParseNode* Recursive_Parse::f12()
{
	if(Check_T_P(current->token.Lex,PreSets[12]))
	{
		ParseNode* ptr=new ParseNode(TypeDef,current->token.LineNum,ParseStr[TypeDef-LINE]);
		ptr->child=f15();
		return ptr;
	}
	else if(Check_T_P(current->token.Lex,PreSets[13]))
		return f13();
	else if(Check_T_P(current->token.Lex,PreSets[14]))
		return f14();
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[TypeDef-LINE]);
}
//TypeDef
ParseNode* Recursive_Parse::f13()
{
	ParseNode* ptr=new ParseNode(TypeDef,current->token.LineNum,ParseStr[TypeDef-LINE]);
	ptr->child=f17();
	return ptr;
}
//TypeDef
ParseNode* Recursive_Parse::f14()
{
	ParseNode* ptr=new ParseNode(TypeDef,current->token.LineNum,ParseStr[TypeDef-LINE]);
	ptr->child=new ParseNode(ID,current->token.LineNum,current->token.SemStr);
	GetNextToken();
	return ptr;
}
//BaseType
ParseNode* Recursive_Parse::f15()
{
	if(Check_T_P(current->token.Lex,PreSets[15]))
	{
		ParseNode* ptr=new ParseNode(BaseType,current->token.LineNum,ParseStr[BaseType-LINE]);
		ptr->child=new ParseNode(INTEGER,current->token.LineNum,current->token.LexStr);
		GetNextToken();
		return ptr;
	}
	else if(Check_T_P(current->token.Lex,PreSets[16]))
		return f16();
	else
	printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[BaseType-LINE]);
}
//BaseType
ParseNode* Recursive_Parse::f16()
{
	ParseNode* ptr=new ParseNode(BaseType,current->token.LineNum,ParseStr[BaseType-LINE]);
	ptr->child=new ParseNode(CHAR1,current->token.LineNum,current->token.LexStr);
	GetNextToken();
	return ptr;
}
//StructureType
ParseNode* Recursive_Parse::f17()
{
	if(Check_T_P(current->token.Lex,PreSets[17]))
	{
		ParseNode* ptr=new ParseNode(StructureType,current->token.LineNum,ParseStr[StructureType-LINE]);
		ptr->child=f19();
		return ptr;
	}
	else if(Check_T_P(current->token.Lex,PreSets[18]))
		return f18();
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[StructureType-LINE]);
}
//StructureType
ParseNode* Recursive_Parse::f18()
{
	ParseNode* ptr=new ParseNode(StructureType,current->token.LineNum,ParseStr[StructureType-LINE]);
	ptr->child=f22();
	return ptr;
}
//ArrayType
ParseNode* Recursive_Parse::f19()
{
	ParseNode* ptr=new ParseNode(ArrayType,current->token.LineNum,ParseStr[ArrayType-LINE]),
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
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[ArrayType-LINE]);
	return ptr;
}
//Low
ParseNode* Recursive_Parse::f20()
{
	ParseNode* ptr=new ParseNode(Low,current->token.LineNum,ParseStr[Low-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[20]))
	{
		ptr->child=new ParseNode(INTC,current->token.LineNum,current->token.SemStr);
		GetNextToken();
	}
	else
	printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[Low-LINE]);
	return ptr;
}
//Top
ParseNode* Recursive_Parse::f21()
{
	ParseNode* ptr=new ParseNode(Top,current->token.LineNum,ParseStr[Top-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[21]))
	{
		ptr->child=new ParseNode(INTC,current->token.LineNum,current->token.SemStr);
		GetNextToken();
	}
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[Top-LINE]);
	return ptr;
}
//RecType
ParseNode* Recursive_Parse::f22()
{
	ParseNode* ptr=new ParseNode(RecType,current->token.LineNum,ParseStr[RecType-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[22]))
	{
		Match(ptr->child,RECORD);
		ptr->child->brother=f23();
		Match(ptr->child->brother->brother,END);
	}
	else
	printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[RecType-LINE]);
	return ptr;
}
//FieldDecList
ParseNode* Recursive_Parse::f23()
{
	if(Check_T_P(current->token.Lex,PreSets[23]))
	{
		ParseNode* ptr=
			new ParseNode(FieldDecList,current->token.LineNum,ParseStr[FieldDecList-LINE]),*p;
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
			printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[FieldDecList-LINE]);
}
//FieldDecList
ParseNode* Recursive_Parse::f24()
{
	ParseNode* ptr=
		new ParseNode(FieldDecList,current->token.LineNum,ParseStr[FieldDecList-LINE]),*p;
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
	printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[FieldDecMore-LINE]);
	
}
//FieldDecMore
ParseNode* Recursive_Parse::f26()
{
	ParseNode* ptr=
		new ParseNode(FieldDecMore,current->token.LineNum,ParseStr[FieldDecMore-LINE]);
	ptr->child=f23();
	return ptr;
}
//IdList
ParseNode* Recursive_Parse::f27()
{
	ParseNode* ptr=
		new ParseNode(IdList,current->token.LineNum,ParseStr[IdList-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[27]))
	{
		ptr->child=new ParseNode(ID,current->token.LineNum,current->token.SemStr);
		GetNextToken();
		ptr->child->brother=f28();
	}
	else
	printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[IdList-LINE]);
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
			printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[IdMore-LINE]);
	
}
//IdMore
ParseNode* Recursive_Parse::f29()
{
	ParseNode* ptr=new ParseNode(IdMore,current->token.LineNum,ParseStr[IdMore-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[29]))
	{
		Match(ptr->child,(LEX)getlex(","));
		ptr->child->brother=f27();
	}
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[IdMore-LINE]);
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
	printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[VarDecpart-LINE]);
		
}
//VarDecpart
ParseNode* Recursive_Parse::f31()
{
	ParseNode* ptr=new ParseNode(VarDecpart,current->token.LineNum,ParseStr[VarDecpart-LINE]);
	Match(ptr->child,VAR);
	ptr->child->brother=f33();
	return ptr;
}
//VarDec
ParseNode* Recursive_Parse::f32()
{
	ParseNode* ptr=new ParseNode(VarDec,current->token.LineNum,ParseStr[VarDec-LINE]),
		*p;
	if(Check_T_P(current->token.Lex,PreSets[32]))
	{
		Match(ptr->child,VAR);
		p=f33();
	}
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[VarDec-LINE]);
	return ptr;
}
//VarDecList
ParseNode* Recursive_Parse::f33()
{
	ParseNode* ptr=new ParseNode(VarDecList,current->token.LineNum,ParseStr[VarDecList-LINE]),
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
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[VarDecList-LINE]);
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
			printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[VarDecMore-LINE]);
	
}
//VarDecMore
ParseNode* Recursive_Parse::f35()
{
	ParseNode* ptr=new ParseNode(VarDecMore,current->token.LineNum,ParseStr[VarDecMore-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[35]))
		ptr->child=f33();
	else
			printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[VarDecMore-LINE]);
		
	return ptr;
}
//VarIdList
ParseNode* Recursive_Parse::f36()
{
	ParseNode* ptr=new ParseNode(VarIdList,current->token.LineNum,ParseStr[VarIdList-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[36]))
	{
		ptr->child=new ParseNode(ID,current->token.LineNum,current->token.SemStr);
		GetNextToken();
		ptr->child->brother=f37();
	}
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[VarIdList-LINE]);
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
			printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[VarIdMore-LINE]);
	
}
//VarIdMore
ParseNode* Recursive_Parse::f38()
{
	ParseNode* ptr=new ParseNode(VarIdMore,current->token.LineNum,ParseStr[VarIdMore-LINE]);
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
	printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[ProcDecpart-LINE]);
	
}
//ProcDecpart
ParseNode* Recursive_Parse::f40()
{
	
	ParseNode* ptr=new ParseNode(ProcDecpart,current->token.LineNum,ParseStr[ProcDecpart-LINE]);
	ptr->child=f41();
	return ptr;
}
ParseNode* Recursive_Parse::f41()
{
	ParseNode* ptr=new ParseNode(ProcDec,current->token.LineNum,ParseStr[ProcDec-LINE]),*p;
	if(Check_T_P(current->token.Lex,PreSets[41]))
	{
		Match(ptr->child,PROCEDURE);
		p=ptr->child->brother=f44();
		Match(p->brother,(LEX)getlex("("));
		p=p->brother->brother=f45();
		Match(p->brother,(LEX)getlex(")"));
		Match(p->brother->brother,(LEX)getlex(";"));
		p=p->brother->brother->brother=f55();
		p=p->brother=f56();
		p=p->brother=f42();
	}
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[ProcDec-LINE]);
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
			printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[ProcDecMore-LINE]);
}
//ProcDecMore
ParseNode* Recursive_Parse::f43()
{
	
	ParseNode* ptr=new ParseNode(ProcDecMore,current->token.LineNum,ParseStr[ProcDecMore-LINE]);
	ptr->child=f41();//注意这里
	return ptr;
}
//ProcName
ParseNode* Recursive_Parse::f44()
{
	ParseNode* ptr=new ParseNode(ProcName,current->token.LineNum,ParseStr[ProcName-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[44]))
	{
		ptr->child=new ParseNode(ID,current->token.LineNum,current->token.SemStr);
		GetNextToken();
	}
	else
			printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[ProcName-LINE]);
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
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[ParamList-LINE]);
	
}
//ParamList
ParseNode* Recursive_Parse::f46()
{
	ParseNode* ptr=new ParseNode(ParamList,current->token.LineNum,ParseStr[ParamList-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[46]))
		ptr->child=f47();
	else
	{
		
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[ParamList-LINE]);
	}
	return ptr;
}
//ParamDecList
ParseNode* Recursive_Parse::f47()
{
	ParseNode* ptr=
		new ParseNode(ParamDecList,current->token.LineNum,ParseStr[ParamDecList-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[47]))
	{
		ptr->child=f50();
		ptr->child->brother=f48();
	}
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[ParamDecList-LINE]);
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
			printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[ParamMore-LINE]);

}
//ParamMore
ParseNode* Recursive_Parse::f49()
{
	ParseNode* ptr=new ParseNode(ParamMore,current->token.LineNum,ParseStr[ParamMore-LINE]);
	Match(ptr->child,(LEX)getlex(";"));
	ptr->child->brother=f47();
	return ptr;
}
//Param
ParseNode* Recursive_Parse::f50()
{	
	if(Check_T_P(current->token.Lex,PreSets[50]))
	{
		ParseNode* ptr=new ParseNode(Param,current->token.LineNum,ParseStr[Param-LINE]);
		ptr->child=f12();
		ptr->child->brother=f52();
		return ptr;
	} 
	else if(Check_T_P(current->token.Lex,PreSets[51]))
		return f51();
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[Param-LINE]);
}
//Param
ParseNode* Recursive_Parse::f51()
{	
	
	ParseNode* ptr=new ParseNode(Param,current->token.LineNum,ParseStr[Param-LINE]);
	Match(ptr->child,VAR);
	ptr->child->brother=f12();
	ptr->child->brother=f52();
	return ptr;
}
//FormList
ParseNode* Recursive_Parse::f52()
{	
	ParseNode* ptr=new ParseNode(FormList,current->token.LineNum,ParseStr[FormList-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[52]))
	{
		ptr->child=new ParseNode(ID,current->token.LineNum,current->token.SemStr);
		GetNextToken();
		ptr->child->brother=f53();
	}
	else
	printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[FormList-LINE]);
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
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[FidMore-LINE]);
	
}
//FidMore
ParseNode* Recursive_Parse::f54()
{	
	ParseNode* ptr=new ParseNode(FidMore,current->token.LineNum,ParseStr[FidMore-LINE]);
	Match(ptr->child,(LEX)getlex(","));
	ptr->child->brother=f52();
	return ptr;
}
//ProcDecPart
ParseNode* Recursive_Parse::f55()
{
	ParseNode* ptr=new ParseNode(ProcDecPart,current->token.LineNum,ParseStr[ProcDecPart-LINE]);	
	if(Check_T_P(current->token.Lex,PreSets[55]))
		ptr->child=f4();
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[ProcDecPart-LINE]);
	return ptr;
}
//ProcBody
ParseNode* Recursive_Parse::f56()
{
	ParseNode* ptr=new ParseNode(ProcBody,current->token.LineNum,ParseStr[ProcBody-LINE]);	
	ptr->child=f57();
	return ptr;
}
//ProgramBody
ParseNode* Recursive_Parse::f57()
{
	ParseNode* ptr=new ParseNode(ProgramBody,current->token.LineNum,ParseStr[ProgramBody-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[57]))
	{
		Match(ptr->child,BEGIN);
		ptr->child->brother=f58();
		Match(ptr->child->brother->brother,END);
	}
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[ProgramBody-LINE]);
	return ptr;
}
//StmList
ParseNode* Recursive_Parse::f58()
{
	ParseNode* ptr=new ParseNode(StmList,current->token.LineNum,ParseStr[StmList-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[58]))
	{
		ptr->child=f61();
		ptr->child->brother=f59();
	}
	else
			printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[StmList-LINE]);
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
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[StmMore-LINE]);

}
//StmMore
ParseNode* Recursive_Parse::f60()
{
	ParseNode* ptr=new ParseNode(StmMore,current->token.LineNum,ParseStr[StmMore-LINE]);	
	Match(ptr->child,(LEX)getlex(";"));
	ptr->child=f58();
	return ptr;
}
//Stm
ParseNode* Recursive_Parse::f61()
{	
	if(Check_T_P(current->token.Lex,PreSets[61]))
	{
		ParseNode* ptr=new ParseNode(Stm,current->token.LineNum,ParseStr[Stm-LINE]);
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
	ParseNode* ptr=new ParseNode(Stm,current->token.LineNum,ParseStr[Stm-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[62]))
		ptr->child=f71();
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[Stm-LINE]);
	return ptr;
}
//Stm
ParseNode* Recursive_Parse::f63()
{
	ParseNode* ptr=new ParseNode(Stm,current->token.LineNum,ParseStr[Stm-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[63]))
		ptr->child=f72();
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[Stm-LINE]);
	return ptr;
}
//Stm
ParseNode* Recursive_Parse::f64()
{
	ParseNode* ptr=new ParseNode(Stm,current->token.LineNum,ParseStr[Stm-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[64]))
		ptr->child=f74();
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[Stm-LINE]);
	return ptr;
}
//Stm
ParseNode* Recursive_Parse::f65()
{
	ParseNode* ptr=new ParseNode(Stm,current->token.LineNum,ParseStr[Stm-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[65]))
		ptr->child=f75();
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[Stm-LINE]);
	return ptr;
}
//Stm
ParseNode* Recursive_Parse::f66()
{
	ParseNode* ptr=new ParseNode(Stm,current->token.LineNum,ParseStr[Stm-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[66]))
	{
		ptr->child=new ParseNode(ID,current->token.LineNum,current->token.SemStr);
		GetNextToken();
		ptr->child=f67();
	}
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[Stm-LINE]);
	return ptr;
}
//AssCall
ParseNode* Recursive_Parse::f67()
{
	if(Check_T_P(current->token.Lex,PreSets[67]))
	{
		ParseNode* ptr=new ParseNode(AssCall,current->token.LineNum,ParseStr[AssCall-LINE]);
		ptr->child=f69();
		return ptr;
	}
	else if(Check_T_P(current->token.Lex,PreSets[68]))
		return f68();
	else
		return 	new ParseNode(-1,current->token.LineNum,"");
}
//AssCall
ParseNode* Recursive_Parse::f68()
{
	ParseNode* ptr=new ParseNode(AssCall,current->token.LineNum,ParseStr[AssCall-LINE]);	
	ptr->child=f76();
	return ptr;
}
//AssignmentRest
ParseNode* Recursive_Parse::f69()
{
	ParseNode* ptr=
		new ParseNode(AssignmentRest,current->token.LineNum,ParseStr[AssignmentRest-LINE]),*p;
	if(Check_T_P(current->token.Lex,PreSets[69]))
	{
		p=ptr->child=f93();
		Match(p->brother,LEX(getlex(":=")));
		p=p->brother;
		p->brother=f83();
	} 
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[AssignmentRest-LINE]);
	return ptr;
}
//ConditionalStm
ParseNode* Recursive_Parse::f70()
{
	ParseNode* ptr=
			new ParseNode(ConditionalStm,current->token.LineNum,ParseStr[ConditionalStm-LINE]),*p;
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
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[ConditionalStm-LINE]);
	return ptr;
}
//LoopStm
ParseNode* Recursive_Parse::f71()
{
	ParseNode* ptr=new ParseNode(LoopStm,current->token.LineNum,ParseStr[LoopStm-LINE]),
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
	printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[LoopStm-LINE]);
	return ptr;
}
//InputStm
ParseNode* Recursive_Parse::f72()
{
	ParseNode* ptr=new ParseNode(InputStm,current->token.LineNum,ParseStr[InputStm-LINE]),
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
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[InputStm-LINE]);
	return ptr;
}
//Invar
ParseNode* Recursive_Parse::f73()
{
	ParseNode* ptr=new ParseNode(Invar,current->token.LineNum,ParseStr[Invar-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[73]))
	{
		ptr->child=new ParseNode(ID,current->token.LineNum,current->token.SemStr);
		GetNextToken();
	}
	else
			printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[Invar-LINE]);
	return ptr;
}
//OutputStm
ParseNode* Recursive_Parse::f74()
{
	ParseNode* ptr=new ParseNode(OutputStm,current->token.LineNum,ParseStr[OutputStm-LINE]),
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
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[OutputStm-LINE]);
	return ptr;
}
//ReturnStm
ParseNode* Recursive_Parse::f75()
{
	ParseNode* ptr=new ParseNode(ReturnStm,current->token.LineNum,ParseStr[ReturnStm-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[75]))
		Match(ptr->child,RETURN);
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[ReturnStm-LINE]);
	return ptr;
}
//CallStmRest
ParseNode* Recursive_Parse::f76()
{
	ParseNode* ptr=new ParseNode(CallStmRest,current->token.LineNum,ParseStr[CallStmRest-LINE]),
		*p;
	if(Check_T_P(current->token.Lex,PreSets[76]))
	{
		Match(ptr->child,(LEX)getlex("("));
		p=ptr->child;
		p=p->brother=f77();
		Match(p->brother,(LEX)getlex(")"));
	}
	else
			printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[CallStmRest-LINE]);
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
			printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[ActParamList-LINE]);

}
//ActParamList
ParseNode* Recursive_Parse::f78()
{
	ParseNode* ptr=new ParseNode(ActParamList,current->token.LineNum,ParseStr[ActParamList-LINE]),*p;
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
			printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[ActParamList-LINE]);
}
//ActParamMore
ParseNode* Recursive_Parse::f80()
{
	ParseNode* ptr=new ParseNode(ActParamMore,current->token.LineNum,ParseStr[ActParamMore-LINE]);
	Match(ptr->child,(LEX)getlex(","));
	ptr->child->brother=f77();
	return ptr;
}
//RelExp
ParseNode* Recursive_Parse::f81()
{
	ParseNode* ptr=new ParseNode(RelExp,current->token.LineNum,ParseStr[RelExp-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[81]))
	{
		ptr->child=f83();
		ptr->child->brother=f82();
	}
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[RelExp-LINE]);
	return ptr;
}
//OtherRelE
ParseNode* Recursive_Parse::f82()
{
	ParseNode* ptr=new ParseNode(OtherRelE,current->token.LineNum,ParseStr[OtherRelE-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[82]))
	{
		ptr->child=f99();
		ptr->child->brother=f83();
	}
	else
		
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[OtherRelE-LINE]);
	return ptr;
}
//Exp
ParseNode* Recursive_Parse::f83()
{
	ParseNode* ptr=new ParseNode(Exp,current->token.LineNum,ParseStr[Exp-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[83]))
	{
		ptr->child=f86();
		ptr->child->brother=f84();
	}
	else
			printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[Exp-LINE]);
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
			printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[OtherTerm-LINE]);
}
//OtherTerm
ParseNode* Recursive_Parse::f85()
{
	ParseNode* ptr=new ParseNode(OtherTerm,current->token.LineNum,ParseStr[OtherTerm-LINE]);
	ptr->child=f101();
	ptr->child->brother=f83();
	return ptr;
}
//Term
ParseNode* Recursive_Parse::f86()
{
	ParseNode* ptr=new ParseNode(Term,current->token.LineNum,ParseStr[Term-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[86]))
	{
		ptr->child=f89();
		ptr->child->brother=f87();
	}
	else
	printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[Term-LINE]);
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
			printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[OtherFactor-LINE]);
	
}
//OtherFactor
ParseNode* Recursive_Parse::f88()
{
	ParseNode* ptr=new ParseNode(OtherFactor,current->token.LineNum,ParseStr[OtherFactor-LINE]);
	ptr->child=f103();
	ptr->child->brother=f86();
	return ptr;
}
//Factor
ParseNode* Recursive_Parse::f89()
{
	
	if(Check_T_P(current->token.Lex,PreSets[89]))
	{
		ParseNode* ptr=new ParseNode(Factor,current->token.LineNum,ParseStr[Factor-LINE]),*p;
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
			printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[Factor-LINE]);
}
//Factor
ParseNode* Recursive_Parse::f90()
{
	ParseNode* ptr=new ParseNode(Factor,current->token.LineNum,ParseStr[Factor-LINE]);
	ptr->child=new ParseNode(INTC,current->token.LineNum,current->token.SemStr);
	GetNextToken();
	return ptr;
}
//Factor
ParseNode* Recursive_Parse::f91()
{
	ParseNode* ptr=new ParseNode(Factor,current->token.LineNum,ParseStr[Factor-LINE]);
	ptr->child=f92();
	return ptr;
}
//Variable
ParseNode* Recursive_Parse::f92()
{
	ParseNode* ptr=new ParseNode(Variable,current->token.LineNum,ParseStr[Variable-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[92]))
	{
		ptr->child=new ParseNode(ID,current->token.LineNum,current->token.SemStr);
		GetNextToken();
		ptr->child->brother=f93();
	}
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[Variable-LINE]);
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
			printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[VariMore-LINE]);

}
//VariMore
ParseNode* Recursive_Parse::f94()
{
    ParseNode* ptr=new ParseNode(VariMore,current->token.LineNum,ParseStr[VariMore-LINE]),
		*p;
	Match(ptr->child,(LEX)getlex("["));
	p=ptr->child->brother=f83();
	Match(p->brother,(LEX)getlex("]"));
	return ptr;
}
//VariMore
ParseNode* Recursive_Parse::f95()
{
	ParseNode* ptr=new ParseNode(VariMore,current->token.LineNum,ParseStr[VariMore-LINE]);
	Match(ptr->child,(LEX)getlex("."));
	ptr->child->brother=f96();
	return ptr;
}
//FieldVar
ParseNode* Recursive_Parse::f96()
{
	ParseNode* ptr=new ParseNode(FieldVar,current->token.LineNum,ParseStr[FieldVar-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[96]))
	{
		ptr->child=new ParseNode(ID,current->token.LineNum,current->token.SemStr);
		GetNextToken();
		ptr->child->brother=f97();
	}
	else
	printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[FieldVar-LINE]);
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
			printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[FieldVarMore-LINE]);
	
}
//FieldVarMore
ParseNode* Recursive_Parse::f98()
{
	ParseNode* ptr=new ParseNode(FieldVarMore,current->token.LineNum,ParseStr[FieldVarMore-LINE]),*p;
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
		ParseNode* ptr=new ParseNode(CmpOp,current->token.LineNum,ParseStr[CmpOp-LINE]);
		Match(ptr->child,(LEX)getlex("<"));
		return ptr;
	}
	else if(Check_T_P(current->token.Lex,PreSets[100]))
		return f100();
	else 
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[CmpOp-LINE]);
}
//CmpOp
ParseNode* Recursive_Parse::f100()
{
	ParseNode* ptr=new ParseNode(CmpOp,current->token.LineNum,ParseStr[CmpOp-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[100]))
		Match(ptr->child,(LEX)getlex("="));
	else
			printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[CmpOp-LINE]);
	return ptr;
}
//AddOp
ParseNode* Recursive_Parse::f101()
{
	if(Check_T_P(current->token.Lex,PreSets[101]))
	{
		ParseNode* ptr=new ParseNode(AddOp,current->token.LineNum,ParseStr[AddOp-LINE]);
		Match(ptr->child,(LEX)getlex("+"));
		return ptr;
	}
	else if(Check_T_P(current->token.Lex,PreSets[102]))
		return f102();
	else
			printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[AddOp-LINE]);
}
//AddOp
ParseNode* Recursive_Parse::f102()
{
	ParseNode* ptr=new ParseNode(AddOp,current->token.LineNum,ParseStr[AddOp-LINE]);
	Match(ptr->child,(LEX)getlex("-"));
	return ptr;
}
//MultOp
ParseNode* Recursive_Parse::f103()
{
	if(Check_T_P(current->token.Lex,PreSets[103]))
	{
		ParseNode* ptr=new ParseNode(MultOp,current->token.LineNum,ParseStr[MultOp-LINE]);
		Match(ptr->child,(LEX)getlex("*"));
		return ptr;
	}
	else if(Check_T_P(current->token.Lex,PreSets[104]))
		return f104();
	else
		printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[MultOp-LINE]);
}
//MultOp
ParseNode* Recursive_Parse::f104()
{
	ParseNode* ptr=new ParseNode(AddOp,current->token.LineNum,ParseStr[AddOp-LINE]);
	if(Check_T_P(current->token.Lex,PreSets[104]))
		Match(ptr->child,(LEX)getlex("/"));
	else
			printf("Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,SES[MultOp-LINE]);
	return ptr;
}
