#include"scanner.h"
#include<stack>
#define LOW  (int(UNDIV)+1)
typedef enum Non_ultimate_Symbol
{
     	Program=LOW,
		ProgramHead,ProgramName,
		DeclarePart,
		TypeDecpart,TypeDec,TypeDecList,TypeDecMore,TypeId,
		TypeDef,BaseType,StructureType,ArrayType,Low,Top,RecType,
		FieldDecList,FieldDecMore,IdList,IdMore,
		VarDecpart,VarDec,VarDecList,VarDecMore,VarIdList,VarIdMore,
		ProcDecpart,ProcDec,ProcDecMore,ProcName,
		ParamList,ParamDecList,ParamMore,Param,FormList,FidMore,
		ProcDecPart,
		ProcBody,
		ProgramBody,
		StmList,StmMore,
		Stm,AssCall,
		AssignmentRest,
		ConditionalStm,
		LoopStm,
		InputStm,Invar,
		OutputStm,
		ReturnStm,
		CallStmRest,ActParamList,ActParamMore,
		RelExp,OtherRelE,
		Exp,OtherTerm,
		Term,OtherFactor,
		Factor,Variable,VariMore,FieldVar,FieldVarMore,CmpOp,AddOp,MultOp
}N_Symbol;
extern char* ParseStr[];
extern int *P[];
extern int *PreSets[];
extern char* SES[];
extern struct SymTableNode;//���ű���
struct Parse
{
	int Symbol,
		LineNum;
	char Parse_Str[SEM_MaxSize];
};
string format(const char* ch);
string space_format(int layer);
struct ParseNode
{
	Parse parse;
	SymTableNode* symtPtr;///here
	ParseNode* brother;
	ParseNode* child;
	ParseNode():brother(0),child(0),symtPtr(0){}
	ParseNode(int symbol,int linenum,char *str):brother(0),child(0)
	{
		parse.Symbol=symbol,parse.LineNum=linenum;
		strcpy(parse.Parse_Str,str);
	}
	~ParseNode(){if(brother) delete brother;  if(child) delete child;}
};
class ParseTree
{
private:
public:
	ParseNode* root;
	ParseTree():root(0){};
	~ParseTree(){if( root ) delete root;}
	ParseNode* SearchChildFirst(ParseNode* ptr,int sym,bool ntype=1);
	//ntype==1 ʱ������brotherͬʱ��������ptr->child ����  ptr->brother
	//���Ǵ��м俪ʼ������Ȼ�����child  ��brother�Ӵ�
	ParseNode* SearchBrotherFirst(ParseNode* ptr,int sym,bool ntype=1);
	//ntype==1  ����������
};
class LL1_Parse
{
private:
	ParseTree parseTree;
	LexScanner lexScanner;
	TokenNode* current;
	TokenList ErrorList;
	int ErrorNum;
	stack<ParseNode*> ParseStack;//����ջ
private:
	//��ȡ���ռ�����ĳһ�ռ����¶�Ӧ��ת�����ʽP
	int* GetP(int N,int T);
	//ȡ�ô�Symbolȡ��������Ϣ�ִ�
	char* GetStr(int Sym);
	//��������,������Ϣ����ErrorList��
	void GetNextToken();
	void LL1_N(ParseNode* ptr);
	void LL1_T(ParseNode* ptr);
public:
	LL1_Parse():current(0),ErrorNum(0){};
	~LL1_Parse(){}; 
	ParseTree* OutParseTree(){return &parseTree;}
	TokenList* scanner(const char* openfile,const char * savefile,int ntype=0)
	{return  lexScanner.scanner(openfile,savefile,ntype);}
	bool parseScanner();//�﷨����
	bool parseScanner(char* openfile,char * savefile,int ntype=0);
};
extern int getlex(const char *ch);
extern string format(const char* ch);
extern string space_format(int layer);
