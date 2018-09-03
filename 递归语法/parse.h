#include"scanner.h"
#include<stack>
//这与终极符N(LEX)枚举相联系起来的最小下标
#define LOW  (int(UNDIV)+1)
//非终级符描述枚举
typedef enum Non_ultimate_Symbol
{
	/**总程序**/
	Program=LOW,
		/**程序头**/
		ProgramHead,ProgramName,
		/**程序声明**/
		DeclarePart,
		/**类型声明**/
		TypeDecpart,TypeDec,TypeDecList,TypeDecMore,TypeId,
		/**类型***/
		TypeDef,BaseType,StructureType,ArrayType,Low,Top,RecType,
		FieldDecList,FieldDecMore,IdList,IdMore,
		/**变量声明**/
		VarDecpart,VarDec,VarDecList,VarDecMore,VarIdList,VarIdMore,
		/**过程声明**/
		ProcDecpart,ProcDec,ProcDecMore,ProcName,
		/**参数声明**/
		ParamList,ParamDecList,ParamMore,Param,FormList,FidMore,
		/**过程中的声明部分**/
		ProcDecPart,
		/**过程体**/
		ProcBody,
		/**主程序体**/
		ProgramBody,
		/**语句序列**/
		StmList,StmMore,
		/**语句**/
		Stm,AssCall,
		/**赋值语句**/
		AssignmentRest,
		/**条件语句**/
		ConditionalStm,
		/**循环语句**/
		LoopStm,
		/**输入语句**/
		InputStm,Invar,
		/**输出语句**/
		OutputStm,
		/**返回语句**/
		ReturnStm,
		/**过程调用语句**/
		CallStmRest,ActParamList,ActParamMore,
		/**条件表达式**/
		RelExp,OtherRelE,
		/**算术表达式**/
		Exp,OtherTerm,
		/**项**/
		Term,OtherFactor,
		/**因子**/
		Factor,Variable,VariMore,FieldVar,FieldVarMore,CmpOp,AddOp,MultOp

}N_Symbol;
extern char* ParseStr[];
//产生式总集，方便进行对应处理
extern int *P[];
//产生式预测集汇总
extern int *PreSets[];
//出错信息列表
extern char* SES[];
extern struct SymTableNode;//符号表项
//语法信息结构体
struct Parse
{
	int Symbol;//终极符与非终极符编号，LEX,N_Symbol
	int	LineNum;
	char Parse_Str[SEM_MaxSize];// 语法描述信息
};
string format(const char* ch);//一个格式化输出函数
string space_format(int layer);//一个空格制式函数
//语法树结点
struct ParseNode
{
	Parse parse;
	SymTableNode* symtPtr;///here
	ParseNode* brother;
	ParseNode* child;
	ParseNode():brother(NULL),child(NULL),symtPtr(NULL){}
	ParseNode(int symbol,int linenum,char *str):brother(NULL),child(NULL)
	{
		parse.Symbol=symbol,parse.LineNum=linenum;
		strcpy(parse.Parse_Str,str);
	}
	void OutToCmd(int layer,bool type);
	void OutToFile(FILE* file,int layer,bool type);
	~ParseNode(){if(brother) delete brother;  if(child) delete child;}
};

//语法树类
class ParseTree
{
public:
	ParseNode* root;//树的根结点
	ParseTree():root(NULL){};
	~ParseTree(){if(root) delete root;}
	void OutToFile(char* savefile);
	void OutToCmd();
	ParseNode* SearchChildFirst(ParseNode* ptr,int sym,bool ntype=1);
	//ntype==1 时，表明brother同时遍历，从ptr->child 而后  ptr->brother
	//都是从中间开始遍历，然后遍历child  和brother子村
	ParseNode* SearchBrotherFirst(ParseNode* ptr,int sym,bool ntype=1);
	//ntype==1  类似于其上
	////////////////////////////////////////////////////////////////////
};
class Recursive_Parse
{
private:
	ParseTree parseTree;
	LexScanner lexScanner;
	TokenNode* current;
	TokenList ErrorList;//错误信息暂存链表
	int ErrorNum;//错误数
private:
	/******语法分析所需的基本函数******/
	//判断一个终极符是否是在某产生式的预测集中
	bool Check_T_P(LEX T,int *p);
	//判断终极符是否匹配成功
	bool Match(ParseNode* &ptr,LEX T);
	//出错处理函数 ,返回值为Symbol=-1的ParseNode指针，以以表示出错信息
	//出错信息编入ErrorList中
	ParseNode* error(const char* ch);
	//current=current->next
	void GetNextToken();
public:
	Recursive_Parse():current(NULL),ErrorNum(NULL){};
	~Recursive_Parse(){}; 
	ParseTree* OutParseTree(){return &parseTree;}
	bool outError(int ntype=1);
	//ntype==1只输出语法出错信息，
	//当ntype==2时只输出词法出错信息，
	//当ntype==3时，二者均输出,(下同)
	bool outErrorFile(const char* filename,int ntype=1);
	/****** 词法分析*******/
	TokenList* scanner(const char* openfile,const char * savefile)
	{return  lexScanner.scanner(openfile,savefile); }
	/*****语法分析，当返回值为false时，表明TOKENLIST为空，下同****/
	bool parseScanner();
	bool parseScanner(char* openfile,char * savefile,int ntype=3);//语法分析之二

private:
	/***************各产生式对应的处理函数*****************/
	//以下是各产生式对应的处理函数，返回值是ParseNode*以便使父节点或者前一兄弟节点指向它
	ParseNode* f0(),*f1(),*f2(),*f3(),*f4(),*f5(),*f6(),*f7(),*f8(),*f9(),
		* f10(),*f11(),*f12(),*f13(),*f14(),*f15(),*f16(),*f17(),*f18(),*f19(),
		* f20(),*f21(),*f22(),*f23(),*f24(),*f25(),*f26(),*f27(),*f28(),*f29(),
		* f30(),*f31(),*f32(),*f33(),*f34(),*f35(),*f36(),*f37(),*f38(),*f39(),
		* f40(),*f41(),*f42(),*f43(),*f44(),*f45(),*f46(),*f47(),*f48(),*f49(),
		* f50(),*f51(),*f52(),*f53(),*f54(),*f55(),*f56(),*f57(),*f58(),*f59(),
		* f60(),*f61(),*f62(),*f63(),*f64(),*f65(),*f66(),*f67(),*f68(),*f69(),
		* f70(),*f71(),*f72(),*f73(),*f74(),*f75(),*f76(),*f77(),*f78(),*f79(),
		* f80(),*f81(),*f82(),*f83(),*f84(),*f85(),*f86(),*f87(),*f88(),*f89(),
		* f90(),*f91(),*f92(),*f93(),*f94(),*f95(),*f96(),*f97(),*f98(),*f99(),
		* f100(),*f101(),*f102(),*f103(),*f104();

};

class LL1_Parse
{
private:
	ParseTree parseTree;//形成的语法树
	LexScanner lexScanner;//词法分析器
	TokenNode* current;//正在处理的Token结点指针
	TokenList ErrorList;//错误信息暂存链表
	int ErrorNum;//错误数
	stack<ParseNode*> ParseStack;//分析栈
	/******语法分析所需的基本函数******/
	//获取非终极符在某一终极符下对应的转向产生式P
	int* GetP(int N,int T);
	//取得从Symbol取得语义信息字串
	char* GetStr(int Sym);
	//出错处理函数,出错信息编入ErrorList中
	void error(const char* ch);
	//即current=current->next
	void GetNextToken();
	//LL1分析法中的非终极符处理函数
	void LL1_N(ParseNode* ptr);
	//LL1分析法中的终极符处理函数
	void LL1_T(ParseNode* ptr);
public:
	LL1_Parse():current(NULL),ErrorNum(NULL){};
	~LL1_Parse(){}; 
	ParseTree* OutParseTree(){return &parseTree;}
	TokenList* scanner(const char* openfile,const char * savefile,int ntype=0)
	{return  lexScanner.scanner(openfile,savefile); }// 词法分析
	bool parseScanner();//语法分析
	bool parseScanner(char* openfile,char * savefile,int ntype=0);//语法分析之二
	
};
/********全局函数列表*************/

//由字符串获取LEX枚举型常量
extern int getlex(const char *ch);

//一个定长的格式化字符串处理函数，方便输出规整的语法树
extern string format(const char* ch);
//空格串的格式化函数，用以输出规整的树
extern string space_format(int layer);
