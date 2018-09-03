/**********************************************************
*SNL词法分析器scanner.h文件
*包含基本的枚举类型，数据结构定义，LexScanner类的定义
***********************************************************/


#include<iostream>
#include<string>
using namespace std;

enum Status { S1=1 , S2 , S3 , S4 , S5 , S6 , S7 , S8 ,S9};
//词法信息表Lex(枚举类型)
enum LEX {
	/**保留字**/
	PROGRAM , PROCEDURE , TYPE , VAR , IF , THEN , ELSE , FI , WHILE , DO ,
	ENDWH , BEGIN , END , READ , WRITE , ARRAY , OF , RECORD , RETURN ,
	/**类型**/
	INTEGER , CHAR1 ,
	/**标识符,整数常量,字符常量**/
	ID , INTC , CHARC ,
	/**单双分界符,数组下标限界符**/
	ASSIGN , EQ , LT , ADD , SUB ,
	MUL , DIV , LPAREN , RPAREN , DOT ,
	COLON , SEMI , COMMA , LMIDPAREN , RMIDPAREN ,
	UNDERANGE,
	/**特殊符号:文件结束符,空格符,出错标号**/
	EOF1 , SPACE , ERROR1  , UNDIV 
};


extern char*LEXSTR[];

//语义信息表SEM 
#define SEM_Reservedword  "保留字，无语义信息"
#define SEM_sSymbol       "单分界符，无语义信息"
#define SEM_dSymbol       "双分界符，无语义信息"
#define SEM_array         "数组限界符，无语义信息"
#define SEM_EOF           "文件结束符，无语义信息"
#define SEM_DOT           "程序结束符，无语义信息"

//出错信息列表
#define Error_Annotate  "Error %d Line %d : lex error :到达文件尾，仍找到不注释结束符\"}\""
#define Error_CharC     "Error %d Line %d : lex error :到达文件尾，仍找到不字符常量结束符\"'\""
#define Error_MCLetter  "Error %d Line %d : lex error :字符常量由过多的字符构成"
#define Error_Undefine1 "Error %d Line %d : lex error :不可识别字符"
#define Error_Undefine2 "Error %d Line %d : lex error :使用了非法字符"
#define Error_Undefine  "Error %d Line %d : lex error :使用了非法字符\"%c\""

//SNL语言词法单位最大长度
#define LEX_MaxSize 256
//语义信息最大长度
#define SEM_MaxSize 256

//TOKEN信息结构体
struct Token
{
	int LineNum;              
	LEX Lex;                   
	char LexStr[LEX_MaxSize];  
	char SemStr[SEM_MaxSize];  
};

//TOKEN链表结点
struct TokenNode
{
	Token token;
	TokenNode* next;
	TokenNode(){};
	TokenNode(int num,int l,const char *lstr,const char *sstr);
};

//TOKEN链表类
class TokenList
{
private:
	TokenNode* front,*rear;
public:
	TokenList():front(NULL),rear(NULL){}
	~TokenList();
	bool insertNode(TokenNode* ptr);
	bool OutToFile(const char *filename,int ntype=0);
	bool OutToCmd(int ntype=0);

	TokenNode* Front(){return front;}
	TokenNode* Rear(){return rear;}
};


//词法分析扫描程序LexScanner类

class LexScanner
{
private:
	TokenList* tokenlist;
	int ErrorNum;
public:
	LexScanner():tokenlist(0),ErrorNum(1){}//初始化为NULL
	~LexScanner(){if(tokenlist) delete tokenlist;}

private:
	bool isLetter(char ch);
	bool isDigit(char ch);
	bool isSymbol(char ch);
	Status GotoStatus(char ch);
	int getNextChar(FILE * file);
	int ungetNextChar(FILE * file);
	int GetLEX(const char *ch);//对比LEXSTR,将得到的LEX编码返回	
	void GetID(FILE* file,int LineNum,TokenNode* & ptr);
	void GetINTC(FILE* file,int LineNum,TokenNode* & ptr);
	void GetSymbol(FILE* file,int LineNum,TokenNode* & ptr);
	void Filter(FILE* file,int &LineNum,TokenNode* & ptr);
	void GetCharC(FILE* file,int &LineNum,TokenNode* & ptr);
	void SpaceFilter(FILE* file,int LineNum,TokenNode*  & ptr);
	void EndFile(FILE* file,int LineNum,TokenNode*  & ptr);
	void UnDefineLetter(FILE* file,int LineNum,TokenNode*  & ptr);
	bool PrintToken(TokenList* list,TokenNode* ptr);

public:
	TokenList* scanner(const char* SourceFile,const char* Lexfile,int ntype=0);
	TokenList* outTokenList(){return tokenlist;}
};










 