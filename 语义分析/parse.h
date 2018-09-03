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
extern struct SymTableNode;//符号表项
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
	//ntype==1 时，表明brother同时遍历，从ptr->child 而后  ptr->brother
	//都是从中间开始遍历，然后遍历child  和brother子村
	ParseNode* SearchBrotherFirst(ParseNode* ptr,int sym,bool ntype=1);
	//ntype==1  类似于其上
};
class LL1_Parse
{
private:
	ParseTree parseTree;
	LexScanner lexScanner;
	TokenNode* current;
	TokenList ErrorList;
	int ErrorNum;
	stack<ParseNode*> ParseStack;//分析栈
private:
	//获取非终极符在某一终极符下对应的转向产生式P
	int* GetP(int N,int T);
	//取得从Symbol取得语义信息字串
	char* GetStr(int Sym);
	//出错处理函数,出错信息编入ErrorList中
	void GetNextToken();
	void LL1_N(ParseNode* ptr);
	void LL1_T(ParseNode* ptr);
public:
	LL1_Parse():current(0),ErrorNum(0){};
	~LL1_Parse(){}; 
	ParseTree* OutParseTree(){return &parseTree;}
	TokenList* scanner(const char* openfile,const char * savefile,int ntype=0)
	{return  lexScanner.scanner(openfile,savefile,ntype);}
	bool parseScanner();//语法分析
	bool parseScanner(char* openfile,char * savefile,int ntype=0);
};
extern int getlex(const char *ch);
extern string format(const char* ch);
extern string space_format(int layer);
