#include"parse.h"
int main()
{
	while(1)
	{	
		LexScanner scan;
		Recursive_Parse parse;
	    string str;
		cout<<"please input the sourcefile name:"<<endl;
		cin>>str;			
     	parse.scanner(str.c_str(),"1.txt");
		parse.parseScanner();
		parse.OutParseTree()->OutToFile("parse.txt");
		printf("\n��鿴parse.txt�е��﷨��!\n");
	}
	return 1;
}