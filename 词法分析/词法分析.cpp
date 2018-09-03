#include<iostream>
#include<string>
#include<stdlib.h>
using namespace std;
enum State{ S1=1, S2 , S3 , S4 , S5 , S6 , S7 ,S8};
enum LEX{
	PROGRAM,PROCEDURE,TYPE,VAR,IF,THEN,ELSE,FI,WHILE,DO,ENDWH,BEGIN,END,READ,WRITE,ARRAY,OF,RECORD,RETURN,//������
	INTEGER,CHAR1,//����
	ID,INTC,//��ʶ��,��������
	ASSIGN,EQ,LT,ADD,SUB,MUL,DIV,LPAREN,RPAREN,DOT,COLON,SEMI,COMMA,LMIDPAREN,RMIDPAREN,UNDERANGE,//��˫�ֽ��,�����±��޽��
	EOFF,SPACE,ERROR1,UNDIV //�������:�ļ�������,�ո��,������
};
//��Ӧ�ʷ���Ϣ���������ַ������Ҵʷ���Ϣ����LEX��������������������Ϣ
char* LEXSTR[] = {
	"program", "procedure","type","var","if","then","else","fi","while","do","endwh","begin","end","read","write","array","of","record","return",
	"integer","char",
	"ID", "INTC",
	":=","=","<","+","-","*","/","(",")",".",":",";",",","[","]","..",
	"EOF" , "SPACE" , "ERROR" , "\\",  "$" //"$"Ϊ������־
};

#define LEX_MaxSize 20
#define SEM_MaxSize 30
struct Token
{
	int LineNum;
	LEX Lex;
	char LexStr[LEX_MaxSize];
	char SemStr[SEM_MaxSize];
};
struct TokenNode
{
	Token token;
	TokenNode* next;
	TokenNode(){};
	TokenNode(int num,int l,const char *lstr,const char *sstr);
};
class TokenList
{
private:
	TokenNode* front,*rear;
public:
	TokenList():front(NULL),rear(NULL){}
	~TokenList();
	bool insertNode(TokenNode* ptr);
	bool OutToFile(const char *filename);
	bool OutToScreen();
	TokenNode* Front(){return front;}
	TokenNode* Rear(){return rear;}
};
class LexScanner
{
private:
	TokenList* tokenlist;
public:
	LexScanner():tokenlist(NULL){}
	~LexScanner(){if(tokenlist) delete tokenlist;}
private:
	bool isLetter(char ch);
	bool isDigit(char ch);
	bool isSymbol(char ch);
	State GotoState(char ch);
	int getNextChar(FILE * file);
	int getbackaChar(FILE * file);
	int GetLEX(const char *ch);//�Ա�LEXSTR,���õ���LEX���뷵��
	void GetID(FILE* file,int LineNum,TokenNode* & ptr);
	void GetINTC(FILE* file,int LineNum,TokenNode*  & ptr);
	void GetSymbol(FILE* file,int LineNum,TokenNode* & ptr);
	void Filter(FILE* file,int &LineNum,TokenNode* & ptr);//ֻ�����к�
	void SpaceFilter(FILE* file,int LineNum,TokenNode* & ptr);
	void EndFile(FILE* file,int LineNum,TokenNode*  & ptr);
	void UnDefineLetter(FILE* file,int LineNum,TokenNode* & ptr);
	bool PrintToken(TokenList* list,TokenNode* ptr);
public:
	TokenList* scanner(const char* SourceFile,const char* Lexfile);
};
TokenNode::TokenNode(int num,int l,const char *lstr,const char *sstr)
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
	{
		front=rear=ptr;
	    rear->next=NULL;
	}
	else
	{
		rear->next=ptr;
		rear=ptr;
		rear->next=NULL;
	}
	return true;
}

bool TokenList::OutToFile(const char *filename)
{
	if(front==NULL)
		return false;
	TokenNode* ptr=front;
	FILE* file=fopen(filename,"w+");//���ļ����ڣ�������д
	if(NULL==file)
		return 0;
	fprintf(file,"%5s\t%3s\t%10s\t%2s\n", "LineNUM" , "LEX" , "LexStr" , "SemStr");
	for(;ptr!=NULL;ptr=ptr->next)
	{
			fprintf(file,"%5d\t%3d\t%10s\t%2s\n",ptr->token.LineNum , ptr->token.Lex , ptr->token.LexStr , ptr->token.SemStr );
	}
	fclose(file);
	return 1;
}
bool TokenList::OutToScreen()
{
	if(front==NULL)
		return false;
	printf("%5s\t%3s\t%10s\t%s\n", "LineNUM" , "LEX" , "LexStr" , "SemStr");
	TokenNode* ptr=front;
	for(;ptr!=NULL;ptr=ptr->next)
	{
			printf("%5d\t%3d\t%10s\t%2s\n",ptr->token.LineNum , ptr->token.Lex , ptr->token.LexStr , ptr->token.SemStr );
	}
	return 1;
}
TokenList* LexScanner::scanner(const char* SourceFile,const char* Lexfile)
{
	FILE* file=fopen(SourceFile,"r");
	if(file==NULL)
	{
		cout<<"cannot open the source file!"<<endl;
	    exit(0);
	}
	int LineNum=1;
	TokenNode *ptr;//TOKEN��Ϣ�ݴ�
	TokenList *list=new TokenList;//�½�һ��TokenList,���Ա�������õ���TOKEN����
	bool flag=1;
	while(flag)
	{
		char ch=getNextChar(file);
		switch(GotoState(ch))
		{
		case S1:
			getbackaChar(file);
			GetID(file,LineNum,ptr);
			PrintToken(list,ptr);
			break;
		case S2:
			getbackaChar(file);
			GetINTC(file,LineNum,ptr);
			PrintToken(list,ptr);
			break;
		case S3:
			getbackaChar(file);
			GetSymbol(file,LineNum,ptr);
			PrintToken(list,ptr);
			break;
		case S4:
			Filter(file,LineNum,ptr);
			if(ptr)
			{
				flag=0;
			}
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
			UnDefineLetter(file,LineNum,ptr);
			PrintToken(list,ptr);
			break;
		}
	}
	fclose(file);
	if(list->Front()!=0)
		tokenlist=list;
	if(!list->OutToFile(Lexfile))
		cerr<<"Error:cannot open the LexFile!"<<endl;
    if(!list->OutToScreen())
		cerr<<"Error:cannot write on Screen!"<<endl;
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
	char str[]={':','=','<','+','-','*','/','(',')','.',';',',','[',']','\\','$'};
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
int LexScanner::getbackaChar(FILE * file)
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
		getbackaChar(file);
	int lex;
	if((lex=GetLEX(str.c_str()))!=-1)//c_str() �� char* ��ʽ���� str �ں��ַ���
		ptr=new TokenNode(LineNum,lex,str.c_str(),"�����֣���������Ϣ");
	else
		ptr=new TokenNode(LineNum,ID,"ID",str.c_str());
}
void LexScanner::GetINTC(FILE* file,int LineNum,TokenNode* & ptr)
{
	char ch=getNextChar(file);
	string str;
	while(isDigit(ch))
	{
		str+=ch;
		ch=getNextChar(file);
	}
	if(ch!=EOF)
		getbackaChar(file);
	ptr=new TokenNode(LineNum,INTC,"INTC",str.c_str());
}
void LexScanner::GetSymbol(FILE* file,int LineNum,TokenNode* & ptr)
{
	char ch=getNextChar(file),ch1;
	if(ch==':')
	{
		if(getNextChar(file)=='=')
		{
			ptr=new TokenNode(LineNum,GetLEX(":="),":=","˫�ָ�������������Ϣ");
			return ;
		}
		else
			getbackaChar(file);
	}
	if(ch=='.')
	{
		ch1=getNextChar(file);
		if(ch1=='.')
		{
			ptr=new TokenNode(LineNum,GetLEX(".."),"..","�����޽������������Ϣ");
			return ;
		}
		if(ch1!=EOF)
			getbackaChar(file);
		ptr=new TokenNode(LineNum,GetLEX("."),".","�������������������Ϣ");
		return ;
	}
	char str[2]={ch,0};
	ptr=new TokenNode(LineNum,GetLEX(str),str,"���ָ�������������Ϣ");
}
void LexScanner::Filter(FILE* file,int &LineNum,TokenNode* & ptr)
{
	char ch=getNextChar(file);
	ptr=0;
	while(ch!='}')
	{
		if(ch=='\n')
			LineNum++;
		else if(ch==EOF)
		{
			cerr<<"Error, line"<<LineNum<<" : �����ļ�β�����ҵ���ע�ͽ�����,����Դ��������!"<<endl;
	        fclose(file);
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
		getbackaChar(file);
}
void LexScanner::EndFile(FILE* file,int LineNum,TokenNode* & ptr)
{
	ptr=new TokenNode(LineNum,EOFF,"EOF","�ļ�����������������Ϣ");
	fclose(file);
}
void LexScanner::UnDefineLetter(FILE* file,int LineNum,TokenNode*  & ptr)
{
	getbackaChar(file);
	cerr<<"Error! line"<<LineNum<<" :���ַǷ��ַ�"<<endl;
	ptr=new TokenNode(LineNum,ERROR1,"error","����δ�����ַ�");
}

bool LexScanner::PrintToken(TokenList* list,TokenNode* ptr)
{
	ptr->next=NULL;
	return list->insertNode(ptr);
}

int main()
{
	LexScanner scanner;
	string str1,str2;
	cout<<"����SNL����Դ�ļ����ʹ������Token�����ļ���(�粻�ڵ�ǰ�ļ��������������·��!)"<<endl;
	cin>>str1>>str2;
	cout<<"��Դ������дʷ��������õ�TOKEN��Ϣ���£�"<<endl;
	scanner.scanner(str1.c_str(),str2.c_str());
	return 0;
}
