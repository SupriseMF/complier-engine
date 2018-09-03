#include"scanner.h"
char * LEXSTR[] = {
	"program" , "procedure" , "type" , "var" , "if" , "then" , "else" , "fi" , "while" , "do",
	"endwh" , "begin" , "end" , "read" , "write" , "array" , "of" , "record" , "return" ,
	"integer" , "char" ,
	"ID" , "INTC" , "CHARC" ,
	":=" , "=" , "<" , "+" , "-" ,
	"*" , "/" , "(" , ")" , "." ,
	":" , ";" , "," , "[" , "]" ,
	"..",
	"EOF" , "SPACE" , "ERROR" , "$" //"$"为结束标志
};
TokenNode::TokenNode(int num,int l,const char *lstr,const char *sstr)
:next(NULL)
{
	this->token.LineNum=num;
	this->token.Lex=LEX(l);
	strcpy(this->token.LexStr,lstr);
	strcpy(this->token.SemStr,sstr);
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
		switch(GotoState(ch))
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
			SpaceFilter(file,LineNum,ptr);
			break;
		case S6:
			LineNum++;
			break;
		case S7:
			flag=0;
			EndFile(file,LineNum,ptr);
			PrintToken(list,ptr);
			break;
		case S8:
		default:
			cerr<<"Error, line"<<LineNum<<" : 不可识别字符"<<endl;
			UnDefineLetter(file,LineNum,ptr);
			PrintToken(list,ptr);
			break;
		}
	}
	fclose(file);
	if(list->Front()!=0)
		tokenlist=list;
	return list;
}
bool LexScanner::isLetter (char ch)
{
	if(ch>='a'&&ch<='z'||ch>='A'&&ch<='Z')
		return 1;
	return 0;
}
bool LexScanner::isDigit  (char ch)
{
	if(ch>='0'&&ch<='9')
		return 1;
	return 0;
}
bool LexScanner::isSymbol (char ch)
{
	char str[]={	':' , '=' , '<' , '+' , '-' ,'*' , '/' , '(' , ')' , '.' ,';' , ',' , '[' , ']' , '\\' ,'$'};
	for(int i=0;str[i]!='$';i++)
	{
		if(ch==str[i])
			return 1;
	}
	return 0;
}
State LexScanner::GotoState(char ch)
{
	if(isLetter(ch))
		return S1;
	if(isDigit(ch))
		return S2;
	if(isSymbol(ch))
		return S3;
	if(ch=='{')
		return S4;
	if(ch==' '||ch=='\t')
		return S5;
	if(ch=='\n'||ch=='\r')
		return S6;
	if(ch==EOF)
		return S7;
	return S8;
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
		ptr=new TokenNode(LineNum,lex,str.c_str(),"保留字，无语义信息");
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
			ptr=new TokenNode(LineNum,GetLEX(":="),":=","双分隔符，无语义信息");
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
			ptr=new TokenNode(LineNum,GetLEX(".."),"..","数组下标，无语义信息");
			return ;
		}
		if(ch1!=EOF)
			ungetNextChar(file);
		ptr=new TokenNode(LineNum,GetLEX("."),".","程序结束符，无语义信息");
		return ;
	}
	char str[2]={ch,0};
	ptr=new TokenNode(LineNum,GetLEX(str),str,"单分隔符，无语义信息");
}
void LexScanner::Filter(FILE* file,int &LineNum,TokenNode* & ptr)
{
	char ch=getNextChar(file);

	ptr=0;
	int linenum=LineNum;
	while(ch!='}')
	{
		if(ch=='\n')
			LineNum++;
		else if(ch==EOF)
		{
			ptr=new TokenNode(linenum,ERROR1,"error","错误");
			break;
		}
		ch=getNextChar(file);
	}
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
	ptr=new TokenNode(LineNum,EOF1,"EOF","文件结束符，无语义信息");
	fclose(file);
}
void LexScanner::UnDefineLetter(FILE* file,int LineNum,TokenNode*  & ptr)
{
	char ErrorStr[SEM_MaxSize];
	ungetNextChar(file);
	ptr=new TokenNode(LineNum,ERROR1,"error","出错");
}			
bool LexScanner::PrintToken(TokenList* list,TokenNode* ptr)
{
	ptr->next=NULL;
	return list->insertNode(ptr);
}