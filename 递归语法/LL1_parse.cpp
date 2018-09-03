#include"parse.h"
//current=current->next
void LL1_Parse::GetNextToken()
{
	do
	{
		current=current->next;
		if(NULL==current)
		{
			exit(0);
		}
	}
	while(current->token.Lex==ERROR1);//ֱ���鵽���һ��������Ϣ
}
//��������
void LL1_Parse::error(const char *ch)
{
	ErrorNum++;
	cout<<"Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,ch;
}

//��ȡ���ռ�����ĳһ�ռ����¶�Ӧ��ת�����ʽP
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
	else//��û���κ�һ������ʽ���Զ�Ӧ֮ʱ������������Ϣ
	//	printf("Error %d Line %d :syntax error ",ErrorNum,current->token.LineNum);
		error(SES[ptr->parse.Symbol-LOW]);//����δ�ҵ�����ʽ�������һ��������Ϣ
}
//LL1�������е��ռ���������
void LL1_Parse::LL1_T(ParseNode* ptr)
{
	if(ptr->parse.Symbol==current->token.Lex)
	{
		if(ptr->parse.Symbol==INTC||ptr->parse.Symbol==ID)
			//������޷����������߳����ַ����Լ���ʶ����������Ϣ�����������Ϣ
			strcpy(ptr->parse.Parse_Str,current->token.SemStr);
		GetNextToken();
	}
	else
	{
		string str="ȱ��";
		str+=GetStr(ptr->parse.Symbol);
		error(str.c_str());
	}
}
bool LL1_Parse::parseScanner()//�﷨����������
{
	TokenList* list;
	if(list=lexScanner.outTokenList())
			current=list->Front();
	if(0==current)//���Ϊ�㣬��Ϊ��
		return 0;
	if(current->token.Lex==ERROR1)//����Token������ȡһ���ǳ����current���
		GetNextToken();
	//�γ��������
	ParseNode*ptr=new ParseNode(Program,current->token.LineNum,ParseStr[Program-LOW]);//������Program��ʼ
	parseTree.root=ptr;
	ParseStack.push(ptr);
	while(!ParseStack.empty())//�������ջ�ǿգ����������
	{
		ptr=ParseStack.top();
		ptr->parse.LineNum=current->token.LineNum;
		ParseStack.pop();
		if(ptr->parse.Symbol>=LOW)//��Ϊ���ռ���ʱ
			LL1_N(ptr);//���÷��ռ���������
		else 
			LL1_T(ptr);//�����ռ���ƥ�亯��
	}
	return 1;
}