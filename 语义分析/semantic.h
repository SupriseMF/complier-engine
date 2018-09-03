#include"parse.h"
#include<stack>
enum TypeKind{intTy,charTy,boolTy,arrayTy};
enum IdKind{varkind,typekind,prockind};
enum AccessKind{dir=0,indir};
extern char* TypeKindStr[];
extern char* AccessKindStr[];
extern char* IdKindStr[];
#define INTSIZE  2
#define CHARSIZE 1
#define BOOLSIZE 1
#define OFFSET 0
//一些常用的数据结构
struct typeIR;
struct ParamTable;
//外部变量
extern typeIR * IntTy,*CharTy,*BoolTy;//一些基本的类型
//类型的数据结构
typedef struct typeIR
{
	int size;
	TypeKind kind;  //类型的类型：无符号整数、字符型、数组类型
	union
	{
		struct
		{
			typeIR* indexTy;//索引类型，要么是整型，要么是字符型
			int low,top;//（自己加的），数组的上下标，可以对其进行语义审查
			typeIR* elemTy;//元素类型
		}ArrayAttr;
	}More;
}TypeIR;
//标识符信息项结构体
struct AttributeIR
{
	typeIR* idtype;
	IdKind kind;   
	int level;     
	union
	{
		struct //变量
		{
			AccessKind access;
			int offset;      
		}VarAttr;
		struct //过程标识符
		{
			ParamTable* param;//参数列表，保存的是参数的类型，用于识别形实参结合是否出错
			int code;
			int size;
		}ProcAttr;
	}More;
	AttributeIR(){}
};
struct ParamTable//参数列表
{
	typeIR* type;//类型的内部表示
	ParamTable *next;//下一参数
	SymTableNode* symPtr; //用以指向变量的地址，以方便形实参的传值
	ParamTable(typeIR* t=NULL):type(0),next(0),symPtr(0){};
	~ParamTable(){ if(next) delete next;}//将参数列表析构
};
struct SymTableNode
{
	char name[LEX_MaxSize];
	AttributeIR attrIR;
	bool EOFL;
	SymTableNode* next; 
	~SymTableNode(){ if(next) delete next;}
};
class SymTable
{
private:
	SymTableNode* front;
	SymTableNode* rear; 
	int maxLayer;       
public:
	SymTable():front(NULL),rear(NULL),maxLayer(-1){};
	~SymTable(){ if(front) delete front;};
public:
	bool insertNode(SymTableNode* ptr);
	bool OutToCmd();      
	SymTableNode* OutFront(){ return front;}
	SymTableNode* OutRear(){ return rear;}
};
struct ScopeNode//本身只保存每一层的符号表首项指针，并不真实地建立一层符号表
{
	SymTableNode* front; 
	ScopeNode* parent;	 
};
class Scope
{
public:
	stack<ScopeNode*> scope;  
	SymTableNode* FindID(const char* idname,bool ntype=1);
	//ntype==1时遍历所有层
	//ntype==0时仅遍历自己所在层
	bool newLayer(SymTableNode* ptr);
	bool DropLayer();				 
};
class Semantic
{
private:
	ParseTree* parseTree;
	SymTable symTable;   
	Scope myScope;     
	void initTy();
	ParseNode* SearchChildFirst(ParseNode* ptr,int sym,bool ntype=1);
	//ntype==1 时，表明brother同时遍历，从ptr->child 而后  ptr->brother
	//都是从中间开始遍历，然后遍历child  和brother子村
	ParseNode* SearchBrotherFirst(ParseNode* ptr,int sym,bool ntype=1);
public:
	Semantic():parseTree(0){initTy();}
	~Semantic(){}
	void OutToCmd(){symTable.OutToCmd();}	
	SymTable* OutSymTable(){return &symTable;}
public:
	bool setParseTree(ParseTree* ptr){ return (parseTree=ptr)!=NULL;}
	bool SemScanner();//进行语义分析
private:
	void _Program    (ParseNode* ptr);//对Program进行分析

	void _DeclarePart(ParseNode* ptr,int layer,int &offset);
	void _ProgramBody(ParseNode* ptr);

	//声明部分的各分块部分
	void _TypeDecpart(ParseNode* ptr,int layer);
	void _VarDecpart (ParseNode* ptr,int layer,int &offset);
	void _ProcDecpart(ParseNode* ptr,int layer);

	//类型声明
	void _TypeDecList(ParseNode* ptr,int layer);//类型声明列表
	void _TypeDef(ParseNode* ptr ,typeIR*& tIR,int layer);//类型声明 	
	void _StructureType(ParseNode* ptr ,typeIR*& tIR,int layer);//自定义类型声明	
	void _ArrayType(ParseNode* ptr ,typeIR*& tIR,int layer);//数组声明

	//变量声明
	void _VarDecList(ParseNode* ptr,int layer,int &offset);//变量声明序列语义分析
	void _VarIdList(ParseNode* ptr,int layer,int &offset,typeIR* tIR);//同一类型的变量声明列表

	//过程声明部分
	void _ProcDec(ParseNode* ptr,int layer,bool mid=0);//过程声明
	void _ParamList(ParseNode* ptr,int layer,int& size,ParamTable*& param);//参数列表
	void _ParamDecList(ParseNode* ptr,int layer,int& size,ParamTable*& param);//参数声明列表
	void _FormList(ParseNode* ptr,int layer,int& size,ParamTable*& param,typeIR* tIR,bool ntype);//同一类型的形参列表

	//程序体部分
	void _StmList		(ParseNode* ptr);//语句序列
	void _ConditionalStm(ParseNode* ptr);//条件语句
	void _LoopStm		(ParseNode* ptr);//循环语句
	void _InputStm		(ParseNode* ptr);//输入语句
	void _OutputStm		(ParseNode* ptr);//输出语句
	void _ReturnStm		(ParseNode* ptr);//返回语句
	void _OtherStm		(ParseNode* ptr);//其它语句：赋值语句、函数调用语句

	void _RelExp		(ParseNode* ptr);//条件表达式
	void _Invar			(ParseNode* ptr);//接收输入语句的值的变量read(invar)
	void _Exp			(ParseNode* ptr,typeIR* &tIR,int &ntype);//算术表达式
		//当ntype==0时，不需要对其进行判定，或者得从后面得到的类型进行相容处理
		//当ntype==1时，表明此类型要与tIR的类型相容
		//当ntype==2时，表明此EXP序列，要与tIR的自定义类型形成限制级关系，即为数组
	void _VariMore		(ParseNode* ptr,SymTableNode* sym,typeIR* &tIR);//纪录或者数组的更多信息
	void _Term			(ParseNode* ptr,typeIR* &tIR,int &ntype);//表达式第一项
	void _OtherTerm		(ParseNode* ptr,typeIR* &tIR,int &ntype);//表达式其它项
	void _Variable		(ParseNode* ptr,typeIR* &tIR,int &ntype);//变量表达式

	void _AssignmentRest(ParseNode* ptr,SymTableNode* sym);//赋值语句语义分析
	void _CallStmRest	(ParseNode* ptr,SymTableNode* sym);//调用语句语义分析
	void _ActParamList	(ParseNode* ptr,ParamTable* param);//调用过程时的实参列表
};
