#include<iostream>
#include<string>
using namespace std;
enum State{ S1=1 , S2 , S3 , S4 , S5 , S6 , S7 , S8 };
enum LEX {
	PROGRAM , PROCEDURE , TYPE , VAR , IF , THEN , ELSE , FI , WHILE , DO ,ENDWH , BEGIN , END , READ , WRITE , ARRAY , OF , RECORD , RETURN ,
	INTEGER , CHAR1 ,
	ID , INTC , CHARC ,
	ASSIGN , EQ , LT , ADD , SUB ,MUL , DIV , LPAREN , RPAREN , DOT ,COLON , SEMI , COMMA , LMIDPAREN , RMIDPAREN ,UNDERANGE,
	EOF1 , SPACE , ERROR1  , UNDIV 
};
extern char*LEXSTR[];
#define LEX_MaxSize 256
#define SEM_MaxSize 256
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
	TokenList():front(0),rear(0){}
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
	LexScanner():tokenlist(0),ErrorNum(1){}
	~LexScanner(){if(tokenlist) delete tokenlist;}
private:
	bool isLetter (char ch);
	bool isDigit  (char ch);
	bool isSymbol (char ch);
	State GotoState(char ch);
	int getNextChar  (FILE * file);
	int ungetNextChar(FILE * file);
	int GetLEX(const char *ch);
	void GetID(FILE* file,int LineNum,TokenNode* &ptr);
	void GetINTC(FILE* file,int LineNum,TokenNode*  &ptr);
	void GetSymbol(FILE* file,int LineNum,TokenNode* &ptr);
	void Filter(FILE* file,int &LineNum,TokenNode* &ptr);
	void SpaceFilter(FILE* file,int LineNum,TokenNode* &ptr);
	void EndFile(FILE* file,int LineNum,TokenNode* &ptr);
	void UnDefineLetter(FILE* file,int LineNum,TokenNode* &ptr);
	bool PrintToken(TokenList* list,TokenNode* ptr);
public:
	TokenList* scanner(const char* SourceFile,const char* Lexfile,int ntype=0);
	TokenList* outTokenList(){return tokenlist;}
};






 