#include"semantic.h"
#include<iostream>
using namespace std;
int main()
{
	while(1)
	{	
		LexScanner scan;
		LL1_Parse parse0;
		Semantic sem;
		string str;
		cout<<"please input the sourcefile name:"<<endl;
		cin>>str;
	    parse0.scanner(str.c_str(),"1.txt",3);
		parse0.parseScanner();//Óï·¨Ê÷
		parse0.OutParseTree();
		sem.setParseTree(parse0.OutParseTree());
		if(sem.SemScanner())
		{
			sem.OutToCmd();
		}
	}
	return 1;
}