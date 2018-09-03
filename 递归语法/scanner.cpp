#include"scanner.h"
//依次对应上面的词法信息表，用于由字符串查找词法信息编码LEX
char * LEXSTR[] = {
	/**保留字**/
	"program" , "procedure" , "type" , "var" , "if" , "then" , "else" , "fi" , "while" , "do",
	"endwh" , "begin" , "end" , "read" , "write" , "array" , "of" , "record" , "return" ,
	/**类型**/
	"integer" , "char" ,
	/**标识符,整数常量,字符常量**/
	"ID" , "INTC" , "CHARC" ,
	/**单双分界符,数组下标限界符**/
	":=" , "=" , "<" , "+" , "-" ,
	"*" , "/" , "(" , ")" , "." ,
	":" , ";" , "," , "[" , "]" ,
	"..",
	/**特殊符号:文件结束符,空格符,出错标号**/
	"EOF" , "SPACE" , "ERROR" , "\\",  "$" //"$"为结束标志
};
TokenNode::TokenNode(int num,int l,const char *lstr,const char *sstr)
:next(NULL)
{
	token.LineNum=num;
	token.Lex=LEX(l);
	strcpy(token.LexStr,lstr);
	strcpy(token.SemStr,sstr);
}
TokenList::~TokenList()
{
	while(front!=NULL)
	{
		TokenNode* ptr=front;
		front=front->next;
		delete ptr;
	}
}
bool TokenList::insertNode(TokenNode* ptr)
{
	if(ptr==NULL)
		return false;
	if(rear==NULL)
		front=rear=ptr,rear->next=NULL;
	else
		rear->next=ptr,rear=ptr,rear->next=NULL;
	return true;
}

bool TokenList::OutToFile(const char *filename,int ntype)
{
	if(front==NULL)
		return false;

	TokenNode* ptr=front;
	FILE* file=fopen(filename,"w+");
	if(NULL==file)
		return 0;
	if(ntype==0)
	{
		fprintf(file,"%5s\t%3s\t%s\n", "LineNUM" , "LEX" , "SemStr");
		fprintf(file,"%5s\t%3s\t%s\n", "-------" , "---" , "----------------");
	}
	else if(ntype==1)
	{
		fprintf(file,"%5s\t%10s\t%s\n", "LineNUM" , "LexStr" , "SemStr");
		fprintf(file,"%5s\t%10s\t%s\n", "-------" , "----------" , "----------------");
	}
	else
	{
		fprintf(file,"%5s\t%3s\t%10s\t%s\n", "LineNUM" , "LEX" , "LexStr" , "SemStr");
		fprintf(file,"%5s\t%3s\t%10s\t%s\n", "-------" , "---" , "----------" , "----------------");
	}
	for(;ptr!=NULL;ptr=ptr->next)
	{
		if(ntype==0)
			fprintf(file,"%5d\t%3d\t%s\n", ptr->token.LineNum , ptr->token.Lex , ptr->token.SemStr);
		else if(ntype==1)
			fprintf(file,"%5d\t%10s\t%s\n", ptr->token.LineNum , ptr->token.LexStr , ptr->token.SemStr);
		else 
			fprintf(file,"%5d\t%3d\t%10s\t%s\n",
			ptr->token.LineNum , ptr->token.Lex , ptr->token.LexStr , ptr->token.SemStr );
	}
	fprintf(file,"%s\n","----------------------------------------------------");
	fclose(file);
	return 1;
}
bool TokenList::OutToCmd(int ntype)
{
	if(front==NULL)
		return false;

	if(ntype==0)
	{
		printf("%5s\t%3s\t%s\n", "LineNUM" , "LEX" , "SemStr");
		printf("%5s\t%3s\t%s\n", "-------" , "---" , "----------------");
	}
	else if(ntype==1)
	{
		printf("%5s\t%10s\t%s\n", "LineNUM" , "LexStr" , "SemStr");
		printf("%5s\t%10s\t%s\n", "-------" , "----------" , "----------------");
	}
	else
	{
		printf("%5s\t%3s\t%10s\t%s\n", "LineNUM" , "LEX" , "LexStr" , "SemStr");
		printf("%5s\t%3s\t%10s\t%s\n", "-------" , "---" , "----------" , "----------------");
	}

	TokenNode* ptr=front;
	for(;ptr!=NULL;ptr=ptr->next)
	{
		if(ntype==0)
			printf("%5d\t%3d\t%s\n", ptr->token.LineNum , ptr->token.Lex , ptr->token.SemStr);
	
		else if(ntype==1)
			printf("%5d\t%10s\t%s\n", ptr->token.LineNum , ptr->token.LexStr , ptr->token.SemStr);
		else 
			printf("%5d\t%3d\t%10s\t%s\n",
			ptr->token.LineNum , ptr->token.Lex , ptr->token.LexStr , ptr->token.SemStr );
	}
	printf("%s\n","----------------------------------------------------");
	return 1;
}

TokenList* LexScanner::scanner(const char* SourceFile,const char* Lexfile,int ntype)
{
	FILE* file=fopen(SourceFile,"r");
	if(file==NULL)
	{
		cerr<<"Error: cannot open the sourcefile!"<<endl;
		return NULL;
	}
	int LineNum=1;
	TokenNode *ptr;
	TokenList *list=new TokenList;
	bool flag=1;
	while(flag)
	{
		char ch=getNextChar(file);
		switch(GotoStatus(ch))
		{
		case S1:
			ungetNextChar(file);
			GetID(file,LineNum,ptr);
			PrintToken(list,ptr);
			break;
		case S2:
			ungetNextChar(file);
			GetINTC(file,LineNum,ptr);
			PrintToken(list,ptr);
			break;
		case S3:
			ungetNextChar(file);
			GetSymbol(file,LineNum,ptr);
			PrintToken(list,ptr);
			break;
		case S4:
			Filter(file,LineNum,ptr);
			if(ptr)
				PrintToken(list,ptr);
			break;
		case S5:
			GetCharC(file,LineNum,ptr);
			PrintToken(list,ptr);
			break;
		case S6:
			SpaceFilter(file,LineNum,ptr);
			break;
		case S7:
			LineNum++;
			break;
		case S8:
			flag=0;
			EndFile(file,LineNum,ptr);
			PrintToken(list,ptr);
			break;
		case S9:
		default:
		//	cerr<<"Error, line"<<LineNum<<" : 不可识别字符"<<endl;
			UnDefineLetter(file,LineNum,ptr);
			PrintToken(list,ptr);
			break;
		}
	}
	fclose(file);
	if(list->Front()!=0)
		tokenlist=list;
	if(!list->OutToFile(Lexfile,ntype))
		cerr<<"Error:cannot open the LexFile!"<<endl;
//	if(!list->OutToCmd(ntype))
//		cerr<<"Error:cannot write on Cmd!"<<endl;
	return list;
}
bool LexScanner::isLetter(char ch)
{
	if(ch>='a'&&ch<='z'||ch>='A'&&ch<='Z')
		return 1;
	return 0;
}
bool LexScanner::isDigit(char ch)
{
	if(ch>='0'&&ch<='9')
		return 1;
	return 0;
}
bool LexScanner::isSymbol(char ch)
{
	char str[]={
		':' , '=' , '<' , '+' , '-' ,
		'*' , '/' , '(' , ')' , '.' ,
		';' , ',' , '[' , ']' , '\\' ,'$'
	};
	for(int i=0;str[i]!='$';i++)
	{
		if(ch==str[i])
			return 1;
	}
	return 0;
}
Status LexScanner::GotoStatus(char ch)
{
	if(isLetter(ch))
		return S1;
	if(isDigit(ch))
		return S2;
	if(isSymbol(ch))
		return S3;
	if(ch=='{')
		return S4;
	if(ch=='\'')
		return S5;
	if(ch==' '||ch=='\t')
		return S6;
	if(ch=='\n'||ch=='\r')
		return S7;
	if(ch==EOF)
		return S8;
	return S9;
}
int LexScanner::getNextChar(FILE * file)
{
	return fgetc(file);
}
int LexScanner::ungetNextChar(FILE * file)
{
	return fseek(file,-1,1);
}
int LexScanner::GetLEX(const char *ch)
//对比LEXSTR,将得到的LEX编码返回,为下面的各函数形成TOKEN服务
{
	for(int i=0;strcmp(LEXSTR[i],"$")!=0;i++)
	{
		if(strcmp(ch,LEXSTR[i])==0)
			return i;
	}
	return -1;
}
void LexScanner::GetID(FILE* file,int LineNum,TokenNode* & ptr)
{
	char ch=getNextChar(file);
	string str;
	while(isLetter(ch)||isDigit(ch))
	{
		str+=ch;
		ch=getNextChar(file);
	}
	if(ch!=EOF)
		ungetNextChar(file);
	int lex;
	if((lex=GetLEX(str.c_str()))!=-1)
		ptr=new TokenNode(LineNum,lex,str.c_str(),SEM_Reservedword);
	else
		ptr=new TokenNode(LineNum,ID,"ID",str.c_str());
}
void LexScanner::GetINTC  (FILE* file,int LineNum,TokenNode* & ptr)
{
	char ch=getNextChar(file);
	string str;
	while(isDigit(ch))
	{
		str+=ch;
		ch=getNextChar(file);
	}
	if(ch!=EOF)
		ungetNextChar(file);
	ptr=new TokenNode(LineNum,INTC,"INTC",str.c_str());
}

void LexScanner::GetSymbol(FILE* file,int LineNum,TokenNode* & ptr)
{
	char ch=getNextChar(file),ch1;
	if(ch==':')
	{
		if(getNextChar(file)=='=')	
		{
			ptr=new TokenNode(LineNum,GetLEX(":="),":=",SEM_dSymbol);
			return ;
		}
		else
			ungetNextChar(file);
	}
	if(ch=='.') 
	{
		ch1=getNextChar(file);
		if(ch1=='.')
		{
			ptr=new TokenNode(LineNum,GetLEX(".."),"..",SEM_array);
			return ;
		}
		if(ch1!=EOF)
			ungetNextChar(file);
		ptr=new TokenNode(LineNum,GetLEX("."),".",SEM_DOT);
		return ;
	}
	char str[2]={ch,0};
	ptr=new TokenNode(LineNum,GetLEX(str),str,SEM_sSymbol);
}
void LexScanner::Filter(FILE* file,int &LineNum,TokenNode* & ptr)
{
	char ch=getNextChar(file);
	char ErrorStr[SEM_MaxSize];
	ptr=0;
	int linenum=LineNum;
	while(ch!='}')
	{
	//	cerr<<"{}"<<endl;
		if(ch=='\n')
			LineNum++;
		else if(ch==EOF)
		{
		//	cerr<<"Error, line"<<LineNum<<" : 到达文件尾，仍找到不注释结束符\"}\""<<endl;
			sprintf(ErrorStr,Error_Annotate,
				ErrorNum,LineNum);
			ptr=new TokenNode(linenum,ERROR1,"error",ErrorStr);
			ErrorNum++;
			break;
		}
		ch=getNextChar(file);
	}
}
void LexScanner::GetCharC   (FILE* file,int &LineNum,TokenNode* & ptr)
{
	string str;
	char ErrorStr[SEM_MaxSize];
	char ch=getNextChar(file);
	int linenum=LineNum;
	while(ch!='\'')
	{
		if(ch=='\n')
			LineNum++;
		else if(ch==EOF)
		{
			//cerr<<"Error, line"<<LineNum<<" : 到达文件尾，仍找到不字符常量结束符\"'\""<<endl;
			sprintf(ErrorStr,Error_CharC,
				ErrorNum,LineNum);
			ptr=new TokenNode(linenum,ERROR1,"error",ErrorStr);
			ErrorNum++;
			return ;
		}
		else
			str+=ch;
		ch=getNextChar(file);
	}
	if(str.length()<=0||str.length()>3||str.length()==2&&str[0]!='\\')
	{
		//cerr<<"Error, line"<<LineNum<<" : 字符常量由过多的字符构成"<<endl;
		sprintf(ErrorStr,Error_MCLetter,
			ErrorNum,LineNum);
		ptr=new TokenNode(linenum,ERROR1,"error",ErrorStr);
		ErrorNum++;
		return ;
	}
	else
		ptr=new TokenNode(LineNum,CHARC,"CHARC",str.c_str());
}
void LexScanner::SpaceFilter(FILE* file,int LineNum,TokenNode* & ptr)
{
	char ch=getNextChar(file);
	while(ch==' '&&ch=='\t')
		ch=getNextChar(file);
	if(ch!=EOF)
		ungetNextChar(file);
}

void LexScanner::EndFile(FILE* file,int LineNum,TokenNode* & ptr)
{
	ptr=new TokenNode(LineNum,EOF1,"EOF",SEM_EOF);
	fclose(file);
}

void LexScanner::UnDefineLetter(FILE* file,int LineNum,TokenNode*  & ptr)
{
	char ErrorStr[SEM_MaxSize];
	ungetNextChar(file);
	sprintf(ErrorStr,Error_Undefine,ErrorNum,LineNum,getNextChar(file));//出错信息编制

	ptr=new TokenNode(LineNum,ERROR1,"error",ErrorStr);
	ErrorNum++;
}			
bool LexScanner::PrintToken(TokenList* list,TokenNode* ptr)
{
	ptr->next=NULL;
	return list->insertNode(ptr);
}