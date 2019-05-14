#include "utils.h"
using namespace PARSER;

int current_track = 0;
int direction = 1;

Parser::Parser(string fname){
	input.open(fname.c_str());
	if(!input){
		printf("Failed to open file %s\n", fname.c_str());
		exit (EXIT_FAILURE);
	}
}
Parser::~Parser(){
	input.close();
}
string Parser::get_next_line(){
	string rst;
	while(getline(input, rst)){
		if(rst.at(0) != '#')
			return rst;
	}
	return "";
}