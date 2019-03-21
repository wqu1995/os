#include <stdio.h>
#include "parser.h"

int main(int argc, char const *argv[]){
	if(argc == 1){
		cout<<"Expected argument after options"<<endl;
		exit(EXIT_FAILURE);
	}

	parse1(argv[1]);
	cout<<endl<<endl;
	parse2(argv[1]);
	cout<<endl<<endl;
	check_defined();
	return 0;
}