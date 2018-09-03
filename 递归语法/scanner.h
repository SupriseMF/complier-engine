/**********************************************************
*SNL�ʷ�������scanner.h�ļ�
*����������ö�����ͣ����ݽṹ���壬LexScanner��Ķ���
***********************************************************/


#include<iostream>
#include<string>
using namespace std;

enum Status { S1=1 , S2 , S3 , S4 , S5 , S6 , S7 , S8 ,S9};
//�ʷ���Ϣ��Lex(ö������)
enum LEX {
	/**������**/
	PROGRAM , PROCEDURE , TYPE , VAR , IF , THEN , ELSE , FI , WHILE , DO ,
	ENDWH , BEGIN , END , READ , WRITE , ARRAY , OF , RECORD , RETURN ,
	/**����**/
	INTEGER , CHAR1 ,
	/**��ʶ��,��������,�ַ�����**/
	ID , INTC , CHARC ,
	/**��˫�ֽ��,�����±��޽��**/
	ASSIGN , EQ , LT , ADD , SUB ,
	MUL , DIV , LPAREN , RPAREN , DOT ,
	COLON , SEMI , COMMA , LMIDPAREN , RMIDPAREN ,
	UNDERANGE,
	/**�������:�ļ�������,�ո��,������**/
	EOF1 , SPACE , ERROR1  , UNDIV 
};


extern char*LEXSTR[];

//������Ϣ��SEM 
#define SEM_Reservedword  "�����֣���������Ϣ"
#define SEM_sSymbol       "���ֽ������������Ϣ"
#define SEM_dSymbol       "˫�ֽ������������Ϣ"
#define SEM_array         "�����޽������������Ϣ"
#define SEM_EOF           "�ļ�����������������Ϣ"
#define SEM_DOT           "�������������������Ϣ"

//������Ϣ�б�
#define Error_Annotate  "Error %d Line %d : lex error :�����ļ�β�����ҵ���ע�ͽ�����\"}\""
#define Error_CharC     "Error %d Line %d : lex error :�����ļ�β�����ҵ����ַ�����������\"'\""
#define Error_MCLetter  "Error %d Line %d : lex error :�ַ������ɹ�����ַ�����"
#define Error_Undefine1 "Error %d Line %d : lex error :����ʶ���ַ�"
#define Error_Undefine2 "Error %d Line %d : lex error :ʹ���˷Ƿ��ַ�"
#define Error_Undefine  "Error %d Line %d : lex error :ʹ���˷Ƿ��ַ�\"%c\""

//SNL���Դʷ���λ��󳤶�
#define LEX_MaxSize 256
//������Ϣ��󳤶�
#define SEM_MaxSize 256

//TOKEN��Ϣ�ṹ��
struct Token
{
	int LineNum;              
	LEX Lex;                   
	char LexStr[LEX_MaxSize];  
	char SemStr[SEM_MaxSize];  
};

//TOKEN������
struct TokenNode
{
	Token token;
	TokenNode* next;
	TokenNode(){};
	TokenNode(int num,int l,const char *lstr,const char *sstr);
};

//TOKEN������
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


//�ʷ�����ɨ�����LexScanner��

class LexScanner
{
private:
	TokenList* tokenlist;
	int ErrorNum;
public:
	LexScanner():tokenlist(0),ErrorNum(1){}//��ʼ��ΪNULL
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
	void GetCharC(FILE* file,int &LineNum,TokenNode* & ptr);
	void SpaceFilter(FILE* file,int LineNum,TokenNode*  & ptr);
	void EndFile(FILE* file,int LineNum,TokenNode*  & ptr);
	void UnDefineLetter(FILE* file,int LineNum,TokenNode*  & ptr);
	bool PrintToken(TokenList* list,TokenNode* ptr);

public:
	TokenList* scanner(const char* SourceFile,const char* Lexfile,int ntype=0);
	TokenList* outTokenList(){return tokenlist;}
};










 