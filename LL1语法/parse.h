#include"scanner.h"
#include<stack>
#define LINE (int(UNDER)+1)
//���ռ���
typedef enum Non_ultimate_Symbol
{
	    Program=LINE,//�ܳ���
		ProgramHead,ProgramName,
		DeclarePart,
		TypeDecpart,TypeDec,TypeDecList,TypeDecMore,TypeId,
		TypeDef,BaseType,StructureType,ArrayType,Low,Top,RecType,FieldDecList,FieldDecMore,IdList,IdMore,
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
//����ʽ�ܼ���������ж�Ӧ����
extern int *P[];
//����ʽԤ�⼯����
extern int *PreSets[];
//������Ϣ�б�
extern char* SES[];
//�﷨��Ϣ�ṹ��
struct Parse
{
	int Symbol;//�ռ�������ռ�����ţ�LEX,N_Symbol
	int	LineNum;
	char Parse_Str[SEM_MaxSize];// �﷨������Ϣ
};
string format(const char* ch);//һ����ʽ���������
string space_format(int layer);//һ���ո���ʽ����
//�﷨�����
struct ParseNode
{
	Parse parse;
	ParseNode* brother;
	ParseNode* child;
	ParseNode():brother(NULL),child(NULL){}
	ParseNode(int symbol,int linenum,char *str):brother(NULL),child(NULL)
	{
		parse.Symbol=symbol,parse.LineNum=linenum;
		strcpy(parse.Parse_Str,str);
	}
	void OutToFile(FILE* file,int layer,bool type);
	~ParseNode(){if(brother) delete brother;  if(child) delete child;}
};

//�﷨����
class ParseTree
{
public:
	ParseNode* root;//���ĸ����
	ParseTree():root(NULL){};
	~ParseTree(){if(root) delete root;}
	void OutToFile(char* savefile);
};
class Recursive_Parse
{
private:
	ParseTree parseTree;
	LexScanner lexScanner;
	TokenNode* current;
	TokenList ErrorList;
	int ErrorNum;
	//�ж�һ���ռ����Ƿ�����ĳ����ʽ��Ԥ�⼯��
	bool Check_T_P(LEX T,int *p);
	//�ж��ռ����Ƿ�ƥ��ɹ�
	bool Match(ParseNode* &ptr,LEX T);
	void GetNextToken();
public:
	Recursive_Parse():current(NULL),ErrorNum(NULL){};
	~Recursive_Parse(){}; 
	ParseTree* OutParseTree(){return &parseTree;}
	TokenList* scanner(const char* openfile,const char * savefile)
	{return  lexScanner.scanner(openfile,savefile); }
	bool parseScanner();

private:
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
	/****��д���ϸ�������ʱע���ж�Ԥ�⼯��Ӧ�Ĳ���ʽ�����һ�����ռ����ж������ʽ��ͨ����һ��
	����ʽ�ĺ�������Ԥ�⼯���жϣ�Ȼ����ѡ����ʵ�����ʽ
	*****/
};

extern int getlex(const char *ch);
extern string format(const char* ch);