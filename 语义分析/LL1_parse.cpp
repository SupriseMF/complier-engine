#include"parse.h"
int getlex(const char *ch)
{
	for(int i=0;strcmp(LEXSTR[i],"$")!=0;i++)
	{
		if(strcmp(ch,LEXSTR[i])==0)
			return i;
	}
	return -1;
}
#define SPACESTR "              "
#define SLEN strlen(SPACESTR)
string format(const char* ch)
{
	string str(ch);
	int l=strlen(ch);
	for(int i=l;i<SLEN;i++)
		str+=' ';
	str[SLEN]+=0;
	return str;
}
string space_format(int layer)
{
	string str;
	for(int i=0;i<layer;i++)
	  str+=SPACESTR;
	return str;
}
ParseNode* ParseTree::SearchChildFirst(ParseNode* ptr,int sym,bool ntype)
{
	if(ptr==NULL)
		return NULL;
	if(ptr->parse.Symbol==sym)
		return ptr;
	ParseNode* p;
	p=SearchChildFirst(ptr->child,sym,ntype);
	if(p!=NULL)
		return p;
	if(ntype)
	{
		p=SearchChildFirst(ptr->brother,sym,ntype);
		return p;
	}
	return NULL;
}
ParseNode* ParseTree::SearchBrotherFirst(ParseNode* ptr,int sym,bool ntype)
{
	if(ptr==NULL)
		return NULL;
	if(ptr->parse.Symbol==sym)
		return ptr;
	ParseNode* p;
	p=SearchBrotherFirst(ptr->brother,sym,ntype);
	if(p!=NULL)
		return p;
	if(ntype)
	{
		p=SearchBrotherFirst(ptr->child,sym,ntype);
		return p;
	}
	return NULL;
}
void LL1_Parse::GetNextToken()
{
	do
	{
		current=current->next;
		if(NULL==current)
		{
			throw 0;
			exit(0);
		}
	}
	while(current->token.Lex==ERROR1);
}
int* LL1_Parse::GetP(int N,int T)
{
	for(int i=1;i<=104;i++)
	{
		if(P[i][0]==N)
			//���ΪN���ռ��������Ĳ���ʽ��
			//���ѯT�Ƿ��ڶ�Ӧ��Ԥ�⼯�У����ǣ����ش˲���ʽ
		{
			for(int j=0;PreSets[i][j]!=-1;j++)
				if(PreSets[i][j]==T)
					return P[i];
		}
	}
	return 0;
}
//ȡ�ô�Symbolȡ��������Ϣ�ִ�
char* LL1_Parse::GetStr(int Sym)
{
	if(Sym<LOW)
		return LEXSTR[Sym];
	return ParseStr[Sym-LOW];
}
//LL1�������еķ��ռ���������
void LL1_Parse::LL1_N(ParseNode* ptr)
{
	int* p=GetP(ptr->parse.Symbol,current->token.Lex);//��ò���ʽ
	ParseNode* p0;
	if(p)
	{
		if(p[1]==-1)
			return ;
		p0=ptr->child=new ParseNode(p[1],current->token.LineNum,GetStr(p[1]));
		stack<ParseNode*> s;
		s.push(p0);//�Ƚ���һ�����ӽ��ѹջ
		for(int i=2;p[i]!=-1;i++)//������ʽ����һ������
		{
			p0=p0->brother=new ParseNode(p[i],current->token.LineNum,GetStr(p[i]));
			s.push(p0);
		}
		while(!s.empty())
		{
			ParseStack.push(s.top());
			s.pop();
		}
	}
}
void LL1_Parse::LL1_T(ParseNode* ptr)
{
	if(ptr->parse.Symbol==current->token.Lex)
	{
		if(ptr->parse.Symbol==INTC||ptr->parse.Symbol==ID)
			strcpy(ptr->parse.Parse_Str,current->token.SemStr);
		GetNextToken();
	}
	else
	{
		string str="ȱ��";
		str+=GetStr(ptr->parse.Symbol);
	}
}
bool LL1_Parse::parseScanner()
{
	TokenList* list;
	if(list=lexScanner.outTokenList())
			current=list->Front();
	if(0==current)
		return 0;
	if(current->token.Lex==ERROR1)
		GetNextToken();
	ParseNode*ptr= new ParseNode(Program,current->token.LineNum,ParseStr[Program-LOW]);
	parseTree.root=ptr;
	ParseStack.push(ptr);
	while(!ParseStack.empty())
	{
		ptr=ParseStack.top();
		ptr->parse.LineNum=current->token.LineNum;
		ParseStack.pop();
		if(ptr->parse.Symbol>=LOW)
			this->LL1_N(ptr);
		else 
			this->LL1_T(ptr);
	}
	return 1;
}