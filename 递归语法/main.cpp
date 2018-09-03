#include"parse.h"
int main()
{
	while(1)
	{	
		LexScanner scan;
		LL1_Parse parse;
	    string str;
		cout<<"please input the sourcefile name:"<<endl;
		cin>>str;			
		parse.scanner(str.c_str(),"1.txt",3);	
		parse.parseScanner();
		parse.OutParseTree()->OutToCmd();
	}
	return 1;
}