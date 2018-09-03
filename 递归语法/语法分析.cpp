/*******************************************************
**���ļ�Ϊparse.cpp,��ҪΪһЩ���ݵĶ��壬���������ַ���
**���飬����ʽ���飬����ʽԤ�⼯���飬�������ַ�������
*******************************************************/
#include"parse.h"
char* ParseStr[]=
{
	"Program",
		/**����ͷ**/
		"ProgramHead","ProgramName",
		/**��������**/
		"DeclarePart","TypeDecpart",
		/**��������**/
		"TypeDec","TypeDecList","TypeDecMore","TypeId",
		/**����***/
		"TypeDef","BaseType","StructureType","ArrayType","Low","Top","RecType",
		"FieldDecList","FieldDecMore","IdList","IdMore",
		/**��������**/
		"VarDecpart","VarDec","VarDecList","VarDecMore","VarIdList","VarIdMore",
		/**��������**/
		"ProDecpart","ProDec","ProDecMore","ProName",
		/**��������**/
		"ParamList","ParamDecList","ParamMore","Param","FormList","FidMore",
		/**�����е���������**/
		"ProcDecPart",
		/**������**/
		"ProcBody",
		/**��������**/
		"ProgramBody",
		/**�������**/
		"StmList","StmMore",
		/**���**/
		"Stm","AssCall",
		/**��ֵ���**/
		"AssignmentRest",
		/**�������**/
		"ConditionalStm",
		/**ѭ�����**/
		"LoopStm",
		/**�������**/
		"InputStm","Invar",
		/**������**/
		"OutputStm",
		/**�������**/
		"ReturnStm",
		/**���̵������**/
		"CallStmRest","ActParamList","ActParamMore",
		/**�������ʽ**/
		"RelExp","OtherRelE",
		/**�������ʽ**/
		"Exp","OtherTerm",
		/**��**/
		"Term","OtherFactor",
		/**����**/
		"Factor","Variable","VariMore","FieldVar","FieldVarMore","CmpOp","AddOp","MultOp"
};
//SyntaxErrorStrȡ��д����ĸΪSES-----�������ַ�������
char* SES[]=
{
		"����ͷ����ȱ�ٱ�����:program!",
		/**����ͷ**/
		"����ͷ����ȱ�ٱ�����:program!","����ͷ������������Ӧ��Ϊ��ʶ��!",
		/**��������**/
		"�����������ֳ���!","���������������ֳ���!",
		/**��������**/
		"������������!","���������б����!","���������б����!","�޴����ͱ�ʶ��������ID����!",
		/**����***/
		"���Ͷ������!","�������ͳ���!","�ṹ�����ͳ���!","�������ͳ���!","�����½�Ӧ��Ϊ�޷�������!",
		"�����Ͻ�Ӧ��Ϊ�޷�������!","��¼���ͳ���!",
		"����������!","�������б����!","Ӧ��Ϊ��ʶ��!","Ӧ��Ϊ��ʶ��!",
		/**��������**/
		"������������!","������������!","���������б����!","���������б����!",
		"������ʶ���б����ӦΪ��ʶ��!","������ʶ���б����!",
		/**��������**/
		"������������!","������������!","���������б����!","�������Ƴ���Ӧ�ñ�ʶ��",
		/**��������**/
		"���̲����б����!","���̲��������б����!","���̲��������б����!",
		"���̲����б����!","���̲��������б����Ӧ��Ϊ��ʶ��!","���̲��������б����!",
		/**�����е���������**/
		"���̵��������ֳ���!",
		/**������**/
		"���������!",
		/**��������**/
		"�����������!",
		/**�������**/
		"������г���!","������г���!",
		/**���**/
		"�Ƿ����!","�������������߸�ֵ������!",
		/**��ֵ���**/
		"��ֵ������!",
		/**�������**/
		"����������!",
		/**ѭ�����**/
		"ѭ��������!",
		/**�������**/
		"����������!","�������Ӧ��Ϊ��ʶ��!",
		/**������**/
		"���������!",
		/**�������**/
		"����������!",
		/**���̵������**/
		"���̵���������!","���̵��ò�������!","���̵��ò����б����!",
		/**�������ʽ**/
		"�������ʽ����!","�Ƚ����������!",
		/**�������ʽ**/
		"�������ʽ����!","�������ʽ����!",
		/**��**/
		"�������ʽ������!","�������ʽ������!",
		/**����**/
		"���ӳ���ӦΪ�޷����������߱�����ʶ��!","�������ʽ��������,ӦΪ��ʶ��!",
		"�Ƿ�����������!","���������ӦΪ��ʶ��!","���������ӦΪ��ʶ��!",
		"�Ƚ����������!","�Ӽ����������!","�˳����������!"
};
//����ʽ���£�
/*�ܳ���*/
int P1[]={Program,ProgramHead,DeclarePart,ProgramBody,-1};
/**����ͷ**/
int P2[]={ProgramHead,PROGRAM,ProgramName,-1};
int P3[]={ProgramName,ID,-1};
/**��������**/
int P4[]={DeclarePart,TypeDecpart,VarDecpart,ProcDecpart,-1};
/**��������**/
int P5[]={TypeDecpart,-1};
int P6[]={TypeDecpart,TypeDec,-1};
int P7[]={TypeDec,TYPE,TypeDecList,-1};
int P8[]={TypeDecList,TypeId,EQ,TypeDef,getlex(";"),TypeDecMore,-1};
int P9[]={TypeDecMore,-1};
int P10[]={TypeDecMore,TypeDecList,-1};
int P11[]={TypeId,ID,-1};
/**����**/
int P12[]={TypeDef,	BaseType,-1};
int P13[]={TypeDef,	StructureType,-1};
int P14[]={TypeDef,	ID,-1};
int P15[]={BaseType,	INTEGER,-1};
int P16[]={BaseType,	CHAR1,-1};
int P17[]={StructureType,	ArrayType,-1};
int P18[]={StructureType,	RecType,-1};
int P19[]={ArrayType,	ARRAY,getlex("["),Low,getlex(".."),Top,getlex("]"),OF,BaseType,-1};
int P20[]={Low,	INTC,-1};
int P21[]={Top,	INTC,-1};
int P22[]={RecType,	RECORD,FieldDecList,END,-1};
int P23[]={FieldDecList,	BaseType,IdList,getlex(";"),FieldDecMore,-1};
int P24[]={FieldDecList,	ArrayType,IdList,getlex(";"),FieldDecMore,-1};
int P25[]={FieldDecMore,	-1};
int P26[]={FieldDecMore,	FieldDecList,-1};
int P27[]={IdList,	ID,IdMore,-1};
int P28[]={IdMore,	-1};
int P29[]={IdMore,	getlex(","),IdList,-1};
/**��������**/
int P30[]={VarDecpart,	-1};
int P31[]={VarDecpart,	VarDec,-1};
int P32[]={VarDec,	VAR,VarDecList,-1};
int P33[]={VarDecList,	TypeDef,VarIdList,getlex(";"),VarDecMore,-1};
int P34[]={VarDecMore,	-1};
int P35[]={VarDecMore,	VarDecList,-1};
int P36[]={VarIdList,	ID,VarIdMore,-1};
int P37[]={VarIdMore,	-1};
int P38[]={VarIdMore,	getlex(","),VarIdList,-1};
/**��������**/
int P39[]={ProcDecpart,	-1};
int P40[]={ProcDecpart,	ProcDec,-1};
int P41[]={ProcDec,	PROCEDURE,ProcName,getlex("("),ParamList,getlex(")"),getlex(";"),
	ProcDecPart,ProcBody,ProcDecMore,-1};
int P42[]={ProcDecMore,	-1};
int P43[]={ProcDecMore,	ProcDec,-1};
int P44[]={ProcName,	ID,-1};
/**��������**/
int P45[]={ParamList,	-1};
int P46[]={ParamList,	ParamDecList,-1};
int P47[]={ParamDecList,	Param,ParamMore,-1};
int P48[]={ParamMore,	-1};
int P49[]={ParamMore,	getlex(";"),ParamDecList,-1};
int P50[]={Param,	TypeDef,FormList,-1};
int P51[]={Param,	VAR,TypeDef,FormList,-1};
int P52[]={FormList,	ID,FidMore,-1};
int P53[]={FidMore,	-1};
int P54[]={FidMore,	getlex(","),FormList,-1};
/**�����е���������**/
int P55[]={ProcDecPart,	DeclarePart,-1};
/**������**/
int P56[]={ProcBody,	ProgramBody,-1};
/**��������**/
int P57[]={ProgramBody,	BEGIN,StmList,END,-1};
/**�������**/
int P58[]={StmList,	Stm,StmMore,-1};
int P59[]={StmMore,	-1};
int P60[]={StmMore,	getlex(";"),StmList,-1};
/**���**/
int P61[]={Stm,	ConditionalStm,-1};
int P62[]={Stm,	LoopStm,-1};
int P63[]={Stm,	InputStm,-1};
int P64[]={Stm,	OutputStm,-1};
int P65[]={Stm,	ReturnStm,-1};
int P66[]={Stm,	ID,AssCall,-1};
int P67[]={AssCall,	AssignmentRest,-1};
int P68[]={AssCall,	CallStmRest,-1};
/**��ֵ���**/
int P69[]={AssignmentRest,	VariMore,getlex(":="),Exp,-1};
/**�������**/
int P70[]={ConditionalStm,	IF,RelExp,THEN,StmList,ELSE,StmList,FI,-1};
/**ѭ�����**/
int P71[]={LoopStm,	WHILE,RelExp,DO,StmList,ENDWH,-1};
/**�������**/
int P72[]={InputStm,	READ,getlex("("),Invar,getlex(")"),-1};
int P73[]={Invar,	ID,-1};
/**������**/
int P74[]={OutputStm,	WRITE,getlex("("),Exp,getlex(")"),-1};
/**�������**/
int P75[]={ReturnStm,	RETURN,-1};
/**���̵������**/
int P76[]={CallStmRest,	getlex("("),ActParamList,getlex(")"),-1};
int P77[]={ActParamList,	-1};
int P78[]={ActParamList,	Exp,ActParamMore,-1};
int P79[]={ActParamMore,	-1};
int P80[]={ActParamMore,	getlex(","),ActParamList,-1};
/**�������ʽ**/
int P81[]={RelExp,	Exp,OtherRelE,-1};
int P82[]={OtherRelE,	CmpOp,Exp,-1};
/**��ʽ���ʽ**/
int P83[]={Exp,	Term,OtherTerm,-1};
int P84[]={OtherTerm,	-1};
int P85[]={OtherTerm,	AddOp,Exp,-1};
/**��**/
int P86[]={Term,	Factor,OtherFactor,-1};
int P87[]={OtherFactor,	-1};
int P88[]={OtherFactor,	MultOp,Term,-1};
/**����**/
int P89[]={Factor,	getlex("("),Exp,getlex(")"),-1};
int P90[]={Factor,	INTC,-1};
int P91[]={Factor,	Variable,-1};
int P92[]={Variable,	ID,VariMore,-1};
int P93[]={VariMore,	-1};
int P94[]={VariMore,	getlex("["),Exp,getlex("]"),-1};
int P95[]={VariMore,	getlex("."),FieldVar,-1};
int P96[]={FieldVar,	ID,FieldVarMore,-1};
int P97[]={FieldVarMore,	-1};
int P98[]={FieldVarMore,	getlex("["),Exp,getlex("]"),-1};
int P99[]={CmpOp,	getlex("<"),-1};
int P100[]={CmpOp,	getlex("="),-1};
int P101[]={AddOp,	getlex("+"),-1};
int P102[]={AddOp,	getlex("-"),-1};
int P103[]={MultOp,	getlex("*"),-1};
int P104[]={MultOp,	getlex("/"),-1};

//����ʽ�ܼ���������ж�Ӧ����
int *P[]=
{
	0,P1,P2,P3,P4,P5,P6,P7,P8,P9,
	P10,P11,P12,P13,P14,P15,P16,P17,P18,P19,
	P20,P21,P22,P23,P24,P25,P26,P27,P28,P29,
	P30,P31,P32,P33,P34,P35,P36,P37,P38,P39,
	P40,P41,P42,P43,P44,P45,P46,P47,P48,P49,
	P50,P51,P52,P53,P54,P55,P56,P57,P58,P59,
	P60,P61,P62,P63,P64,P65,P66,P67,P68,P69,
	P70,P71,P72,P73,P74,P75,P76,P77,P78,P79,
	P80,P81,P82,P83,P84,P85,P86,P87,P88,P89,
	P90,P91,P92,P93,P94,P95,P96,P97,P98,P99,
	P100,P101,P102,P103,P104
};
//����ʽ��ӦԤ�⼯����
int PreSet1[]={PROGRAM,-1};
int PreSet2[]={PROGRAM,-1};
int PreSet3[]={ID,-1};
int PreSet4[]={TYPE,VAR,PROCEDURE,BEGIN,-1};
int PreSet5[]={VAR,PROCEDURE,BEGIN,-1};
int PreSet6[]={TYPE,-1};
int PreSet7[]={TYPE,-1};
int PreSet8[]={ID,-1};
int PreSet9[]={VAR,PROCEDURE,BEGIN,-1};
int PreSet10[]={ID,-1};
int PreSet11[]={ID,-1};
int PreSet12[]={INTEGER,CHAR1,-1};
int PreSet13[]={ARRAY,RECORD,-1};
int PreSet14[]={ID,-1};
int PreSet15[]={INTEGER,-1};
int PreSet16[]={CHAR1,-1};
int PreSet17[]={ARRAY,-1};
int PreSet18[]={RECORD,-1};
int PreSet19[]={ARRAY,-1};
int PreSet20[]={INTC,-1};
int PreSet21[]={INTC,-1};
int PreSet22[]={RECORD,-1};
int PreSet23[]={INTEGER,CHAR1,-1};
int PreSet24[]={ARRAY,-1};
int PreSet25[]={END,-1};
int PreSet26[]={INTEGER,CHAR1,ARRAY,-1};
int PreSet27[]={ID,-1};
int PreSet28[]={getlex(";"),-1};
int PreSet29[]={COMMA,-1};
int PreSet30[]={PROCEDURE,BEGIN,-1};
int PreSet31[]={VAR,-1};
int PreSet32[]={VAR,-1};
int PreSet33[]={INTEGER,CHAR1,ARRAY,RECORD,ID,-1};
int PreSet34[]={PROCEDURE,BEGIN,-1};
int PreSet35[]={INTEGER,CHAR1,ARRAY,RECORD,ID,-1};
int PreSet36[]={ID,-1};
int PreSet37[]={getlex(";"),-1};
int PreSet38[]={COMMA,-1};
int PreSet39[]={BEGIN,-1};
int PreSet40[]={PROCEDURE,-1};
int PreSet41[]={PROCEDURE,-1};
int PreSet42[]={BEGIN,-1};
int PreSet43[]={PROCEDURE,-1};
int PreSet44[]={ID,-1};
int PreSet45[]={getlex(")"),-1};
int PreSet46[]={INTEGER,CHAR1,ARRAY,RECORD,ID,VAR,-1};
int PreSet47[]={INTEGER,CHAR1,ARRAY,RECORD,ID,VAR,-1};
int PreSet48[]={getlex(")"),-1};//ע������  ������Ϊ�д�����Ϊ"("
int PreSet49[]={getlex(";"),-1};
int PreSet50[]={INTEGER,CHAR1,ARRAY,RECORD,ID,-1};
int PreSet51[]={VAR,-1};
int PreSet52[]={ID,-1};
int PreSet53[]={getlex(";"),getlex(")"),-1};
int PreSet54[]={COMMA,-1};
int PreSet55[]={TYPE,VAR,PROCEDURE,BEGIN,-1};
int PreSet56[]={BEGIN,-1};
int PreSet57[]={BEGIN,-1};
int PreSet58[]={ID,IF,WHILE,RETURN,READ,WRITE,END,getlex(";"),-1};//ע������
int PreSet59[]={ELSE,FI,END,ENDWH,-1};
int PreSet60[]={getlex(";"),-1};
int PreSet61[]={IF,-1};
int PreSet62[]={WHILE,-1};
int PreSet63[]={READ,-1};
int PreSet64[]={WRITE,-1};
int PreSet65[]={RETURN,-1};
int PreSet66[]={ID,-1};
int PreSet67[]={getlex("["),DOT,getlex(":="),-1};
int PreSet68[]={getlex("("),-1};
int PreSet69[]={getlex("["),DOT,getlex(":="),-1};
int PreSet70[]={IF,-1};
int PreSet71[]={WHILE,-1};
int PreSet72[]={READ,-1};
int PreSet73[]={ID,-1};
int PreSet74[]={WRITE,-1};
int PreSet75[]={RETURN,-1};
int PreSet76[]={getlex("("),-1};
int PreSet77[]={getlex(")"),-1};
int PreSet78[]={getlex("("),INTC,ID,-1};
int PreSet79[]={getlex(")"),-1};
int PreSet80[]={COMMA,-1};
int PreSet81[]={getlex("("),INTC,ID,-1};
int PreSet82[]={getlex("<"),getlex("="),-1};
int PreSet83[]={getlex("("),INTC,ID,-1};
int PreSet84[]={getlex("<"),getlex("="),getlex("]"),THEN,ELSE,FI,DO,ENDWH,
	getlex(")"),END,getlex(";"),COMMA,-1};
int PreSet85[]={getlex("+"),getlex("-"),-1};
int PreSet86[]={getlex("("),INTC,ID,-1};
int PreSet87[]={getlex("+"),getlex("-"),getlex("<"),getlex("="),getlex("]"),THEN,ELSE,FI,DO,ENDWH,
	getlex(")"),END,getlex(";"),COMMA,-1};
int PreSet88[]={getlex("*"),getlex("/"),-1};
int PreSet89[]={getlex("("),-1};
int PreSet90[]={INTC,-1};
int PreSet91[]={ID,-1};
int PreSet92[]={ID,-1};
int PreSet93[]={getlex(":="),getlex("*"),getlex("/"),getlex("+"),getlex("-"),getlex("<"),
	getlex("="),getlex("]"),THEN,ELSE,FI,DO,ENDWH,
	getlex(")"),END,getlex(";"),COMMA,-1};
int PreSet94[]={getlex("["),-1};
int PreSet95[]={DOT,-1};
int PreSet96[]={ID,-1};
int PreSet97[]={getlex(":="),getlex("*"),getlex("/"),getlex("+"),getlex("-"),getlex("<"),
	getlex("="),getlex("]"),THEN,ELSE,FI,DO,ENDWH,
	getlex(")"),END,getlex(";"),COMMA,-1};
int PreSet98[]={getlex("["),-1};
int PreSet99[]={getlex("<"),-1};
int PreSet100[]={getlex("="),-1};
int PreSet101[]={getlex("+"),-1};
int PreSet102[]={getlex("-"),-1};
int PreSet103[]={getlex("*"),-1};
int PreSet104[]={getlex("/"),-1};
//����ʽԤ�⼯�������£�
int *PreSets[]=
{
	0,PreSet1,PreSet2,PreSet3,PreSet4,PreSet5,PreSet6,PreSet7,PreSet8,PreSet9,
	PreSet10,PreSet11,PreSet12,PreSet13,PreSet14,PreSet15,PreSet16,PreSet17,PreSet18,PreSet19,
	PreSet20,PreSet21,PreSet22,PreSet23,PreSet24,PreSet25,PreSet26,PreSet27,PreSet28,PreSet29,
	PreSet30,PreSet31,PreSet32,PreSet33,PreSet34,PreSet35,PreSet36,PreSet37,PreSet38,PreSet39,
	PreSet40,PreSet41,PreSet42,PreSet43,PreSet44,PreSet45,PreSet46,PreSet47,PreSet48,PreSet49,
	PreSet50,PreSet51,PreSet52,PreSet53,PreSet54,PreSet55,PreSet56,PreSet57,PreSet58,PreSet59,
	PreSet60,PreSet61,PreSet62,PreSet63,PreSet64,PreSet65,PreSet66,PreSet67,PreSet68,PreSet69,
	PreSet70,PreSet71,PreSet72,PreSet73,PreSet74,PreSet75,PreSet76,PreSet77,PreSet78,PreSet79,
	PreSet80,PreSet81,PreSet82,PreSet83,PreSet84,PreSet85,PreSet86,PreSet87,PreSet88,PreSet89,
	PreSet90,PreSet91,PreSet92,PreSet93,PreSet94,PreSet95,PreSet96,PreSet97,PreSet98,PreSet99,
	PreSet100,PreSet101,PreSet102,PreSet103,PreSet104
};

