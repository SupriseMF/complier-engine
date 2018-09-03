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
	while(current->token.Lex==ERROR1);//直到查到最后一个词义信息
}
//出错处理函数
void LL1_Parse::error(const char *ch)
{
	ErrorNum++;
	cout<<"Error %d Line %d :syntax error :%s",ErrorNum,current->token.LineNum,ch;
}

//获取非终极符在某一终极符下对应的转向产生式P
int* LL1_Parse::GetP(int N,int T)
{
	for(int i=1;i<=104;i++)
	{
		if(P[i][0]==N)
			//如果为N非终极符导出的产生式，
			//则查询T是否在对应的预测集中，如是，返回此产生式
		{
			for(int j=0;PreSets[i][j]!=-1;j++)
				if(PreSets[i][j]==T)
					return P[i];
		}
	}
	return 0;
}
//取得从Symbol取得语义信息字串
char* LL1_Parse::GetStr(int Sym)
{
	if(Sym<LOW)
		return LEXSTR[Sym];
	return ParseStr[Sym-LOW];
}
//LL1分析法中的非终极符处理函数
void LL1_Parse::LL1_N(ParseNode* ptr)
{
	int* p=GetP(ptr->parse.Symbol,current->token.Lex);//获得产生式
	ParseNode* p0;
	if(p)
	{
		if(p[1]==-1)
			return ;
		p0=ptr->child=new ParseNode(p[1],current->token.LineNum,GetStr(p[1]));
		stack<ParseNode*> s;
		s.push(p0);//先将第一个儿子结点压栈
		for(int i=2;p[i]!=-1;i++)//将产生式构成一个子树
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
	else//当没有任何一个产生式可以对应之时，产生出错信息
	//	printf("Error %d Line %d :syntax error ",ErrorNum,current->token.LineNum);
		error(SES[ptr->parse.Symbol-LOW]);//出果未找到产生式，则产生一个出错信息
}
//LL1分析法中的终极符处理函数
void LL1_Parse::LL1_T(ParseNode* ptr)
{
	if(ptr->parse.Symbol==current->token.Lex)
	{
		if(ptr->parse.Symbol==INTC||ptr->parse.Symbol==ID)
			//如果是无符号整数或者常量字符，以及标识符，语义信息保存其词义信息
			strcpy(ptr->parse.Parse_Str,current->token.SemStr);
		GetNextToken();
	}
	else
	{
		string str="缺少";
		str+=GetStr(ptr->parse.Symbol);
		error(str.c_str());
	}
}
bool LL1_Parse::parseScanner()//语法分析主程序
{
	TokenList* list;
	if(list=lexScanner.outTokenList())
			current=list->Front();
	if(0==current)//如果为零，则为空
		return 0;
	if(current->token.Lex==ERROR1)//当首Token出错，读取一个非出错的current结点
		GetNextToken();
	//形成树根结点
	ParseNode*ptr=new ParseNode(Program,current->token.LineNum,ParseStr[Program-LOW]);//程序以Program开始
	parseTree.root=ptr;
	ParseStack.push(ptr);
	while(!ParseStack.empty())//如果分析栈非空，则继续分析
	{
		ptr=ParseStack.top();
		ptr->parse.LineNum=current->token.LineNum;
		ParseStack.pop();
		if(ptr->parse.Symbol>=LOW)//当为非终极符时
			LL1_N(ptr);//调用非终极符处理函数
		else 
			LL1_T(ptr);//调用终极符匹配函数
	}
	return 1;
}