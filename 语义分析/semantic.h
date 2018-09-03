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
//һЩ���õ����ݽṹ
struct typeIR;
struct ParamTable;
//�ⲿ����
extern typeIR * IntTy,*CharTy,*BoolTy;//һЩ����������
//���͵����ݽṹ
typedef struct typeIR
{
	int size;
	TypeKind kind;  //���͵����ͣ��޷����������ַ��͡���������
	union
	{
		struct
		{
			typeIR* indexTy;//�������ͣ�Ҫô�����ͣ�Ҫô���ַ���
			int low,top;//���Լ��ӵģ�����������±꣬���Զ�������������
			typeIR* elemTy;//Ԫ������
		}ArrayAttr;
	}More;
}TypeIR;
//��ʶ����Ϣ��ṹ��
struct AttributeIR
{
	typeIR* idtype;
	IdKind kind;   
	int level;     
	union
	{
		struct //����
		{
			AccessKind access;
			int offset;      
		}VarAttr;
		struct //���̱�ʶ��
		{
			ParamTable* param;//�����б�������ǲ��������ͣ�����ʶ����ʵ�ν���Ƿ����
			int code;
			int size;
		}ProcAttr;
	}More;
	AttributeIR(){}
};
struct ParamTable//�����б�
{
	typeIR* type;//���͵��ڲ���ʾ
	ParamTable *next;//��һ����
	SymTableNode* symPtr; //����ָ������ĵ�ַ���Է�����ʵ�εĴ�ֵ
	ParamTable(typeIR* t=NULL):type(0),next(0),symPtr(0){};
	~ParamTable(){ if(next) delete next;}//�������б�����
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
struct ScopeNode//����ֻ����ÿһ��ķ��ű�����ָ�룬������ʵ�ؽ���һ����ű�
{
	SymTableNode* front; 
	ScopeNode* parent;	 
};
class Scope
{
public:
	stack<ScopeNode*> scope;  
	SymTableNode* FindID(const char* idname,bool ntype=1);
	//ntype==1ʱ�������в�
	//ntype==0ʱ�������Լ����ڲ�
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
	//ntype==1 ʱ������brotherͬʱ��������ptr->child ����  ptr->brother
	//���Ǵ��м俪ʼ������Ȼ�����child  ��brother�Ӵ�
	ParseNode* SearchBrotherFirst(ParseNode* ptr,int sym,bool ntype=1);
public:
	Semantic():parseTree(0){initTy();}
	~Semantic(){}
	void OutToCmd(){symTable.OutToCmd();}	
	SymTable* OutSymTable(){return &symTable;}
public:
	bool setParseTree(ParseTree* ptr){ return (parseTree=ptr)!=NULL;}
	bool SemScanner();//�����������
private:
	void _Program    (ParseNode* ptr);//��Program���з���

	void _DeclarePart(ParseNode* ptr,int layer,int &offset);
	void _ProgramBody(ParseNode* ptr);

	//�������ֵĸ��ֿ鲿��
	void _TypeDecpart(ParseNode* ptr,int layer);
	void _VarDecpart (ParseNode* ptr,int layer,int &offset);
	void _ProcDecpart(ParseNode* ptr,int layer);

	//��������
	void _TypeDecList(ParseNode* ptr,int layer);//���������б�
	void _TypeDef(ParseNode* ptr ,typeIR*& tIR,int layer);//�������� 	
	void _StructureType(ParseNode* ptr ,typeIR*& tIR,int layer);//�Զ�����������	
	void _ArrayType(ParseNode* ptr ,typeIR*& tIR,int layer);//��������

	//��������
	void _VarDecList(ParseNode* ptr,int layer,int &offset);//�������������������
	void _VarIdList(ParseNode* ptr,int layer,int &offset,typeIR* tIR);//ͬһ���͵ı��������б�

	//������������
	void _ProcDec(ParseNode* ptr,int layer,bool mid=0);//��������
	void _ParamList(ParseNode* ptr,int layer,int& size,ParamTable*& param);//�����б�
	void _ParamDecList(ParseNode* ptr,int layer,int& size,ParamTable*& param);//���������б�
	void _FormList(ParseNode* ptr,int layer,int& size,ParamTable*& param,typeIR* tIR,bool ntype);//ͬһ���͵��β��б�

	//�����岿��
	void _StmList		(ParseNode* ptr);//�������
	void _ConditionalStm(ParseNode* ptr);//�������
	void _LoopStm		(ParseNode* ptr);//ѭ�����
	void _InputStm		(ParseNode* ptr);//�������
	void _OutputStm		(ParseNode* ptr);//������
	void _ReturnStm		(ParseNode* ptr);//�������
	void _OtherStm		(ParseNode* ptr);//������䣺��ֵ��䡢�����������

	void _RelExp		(ParseNode* ptr);//�������ʽ
	void _Invar			(ParseNode* ptr);//������������ֵ�ı���read(invar)
	void _Exp			(ParseNode* ptr,typeIR* &tIR,int &ntype);//�������ʽ
		//��ntype==0ʱ������Ҫ��������ж������ߵôӺ���õ������ͽ������ݴ���
		//��ntype==1ʱ������������Ҫ��tIR����������
		//��ntype==2ʱ��������EXP���У�Ҫ��tIR���Զ��������γ����Ƽ���ϵ����Ϊ����
	void _VariMore		(ParseNode* ptr,SymTableNode* sym,typeIR* &tIR);//��¼��������ĸ�����Ϣ
	void _Term			(ParseNode* ptr,typeIR* &tIR,int &ntype);//���ʽ��һ��
	void _OtherTerm		(ParseNode* ptr,typeIR* &tIR,int &ntype);//���ʽ������
	void _Variable		(ParseNode* ptr,typeIR* &tIR,int &ntype);//�������ʽ

	void _AssignmentRest(ParseNode* ptr,SymTableNode* sym);//��ֵ����������
	void _CallStmRest	(ParseNode* ptr,SymTableNode* sym);//��������������
	void _ActParamList	(ParseNode* ptr,ParamTable* param);//���ù���ʱ��ʵ���б�
};
