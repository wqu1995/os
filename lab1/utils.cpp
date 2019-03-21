#include "utils.h"
using namespace S;

int var_count = 0;
int global_addr = 0;
int module_count = 0;
Symbol sym_table[256];
Usymbol use_table[16];

int int_check(string s){
	if(s.length() == 0)
		return 0;
	for(int i = 0; i<s.length(); i++)
		if(!isdigit(s[i]))
			return 0;
	return 1;
}

int addr_check(string s){
	if(s.length()==0 || s.length()>1)
		return 0;
	if(s[0] == 'A' || s[0] == 'E' || s[0] == 'I' || s[0] == 'R')
		return 1;
	return 0;
}

int sym_check(string s){
	regex e("[a-zA-Z][a-zA-Z0-9]*");
	if(s.length()==0)
		return 0;
	if(regex_match(s, e))
		return 1;
	return 0;
}

int sym_length_check(string s){
	if(s.length() >16)
		return 0;
	return 1;
}

void check_defined(){
	for(int i = 0; i< var_count; i++){
		if(!sym_table[i].used)
			__not_used_warning(sym_table[i].module, sym_table[i].name);
	}
}