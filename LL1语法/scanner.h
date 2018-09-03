#include<iostream>
#include<string>
using namespace std;
enum Status {S1=1,S2,S3,S4,S5,S6,S7,S8};
enum LEX{
	PROGRAM, PROCEDURE,TYPE,VAR,IF,THEN,ELSE,FI,WHILE,DO,ENDWH,BEGIN,END,READ,WRITE,ARRAY,OF,RECORD,RETURN,//������
	INTEGER,CHAR1,//����
	ID,INTC,//��ʶ��������
	ASSIGN,EQ,LT,ADD,SUB,MUL,DIV,LPAREN,RPAREN,DOT,COLON,SEMI,COMMA,LMIDPAREN,RMIDPAREN,UNDERANGE,//��˫�ֽ磬�����±�
	EOFF,SPACE,ERROR1,UNDER //�ļ��������ո񣬴��󣬽���
};
extern char*LEXSTR[];
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
	TokenNode* Front(){return front;}
	TokenNode* Rear(){return rear;}
};
class LexScanner
{
private:
	TokenList* tokenlist;
	int ErrorNum;
public:
	LexScanner():tokenlist(0){}
	~LexScanner(){if(tokenlist) delete tokenlist;}

private:
	bool isLetter(char ch);
	bool isDigit(char ch);
	bool isSymbol(char ch);
	Status GotoStatus(char ch);
	int getNextChar(FILE * file);
	int ungetNextChar(FILE * file);
	int GetLEX(const char *ch);//�Ա�LEXSTR,���õ���LEX���뷵��	
	void GetID(FILE* file,int LineNum,TokenNode* & ptr);
	void GetINTC(FILE* file,int LineNum,TokenNode* & ptr);
	void GetSymbol(FILE* file,int LineNum,TokenNode* & ptr);
	void Filter(FILE* file,int &LineNum,TokenNode* & ptr);
	void SpaceFilter(FILE* file,int LineNum,TokenNode*  & ptr);
	void EndFile(FILE* file,int LineNum,TokenNode*  & ptr);
	void UnDefineLetter(FILE* file,int LineNum,TokenNode*  & ptr);
	bool PrintToken(TokenList* list,TokenNode* ptr);
public:
	TokenList* scanner(const char* SourceFile,const char* Lexfile);//,int ntype=0);
	TokenList* outTokenList(){return tokenlist;}
};










 