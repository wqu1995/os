#include "error.h"


void __parseerror(int errcode, int linenum, int lineoffset){
	static string errstr[]={
		"NUM_EXPECTED",
		"SYM_EXPECTED", 
		"ADDR_EXPECTED", 
		"SYM_TOO_LONG",
		"TOO_MANY_DEF_IN_MODULE",
		"TOO_MANY_USE_IN_MODULE",
		"TOO_MANY_INSTR"
	};
	printf("Parse Error line %d offset %d: %s\n", linenum, lineoffset, errstr[errcode].c_str());
	exit(EXIT_FAILURE);
}
void __not_used_warning(int m, string s){
	printf("Warning: Module %d: %s was defined but never used\n", m,s.c_str());
}

void __multi_def_err(){
	printf(" Error: This variable is multiple times defined; first value used");
}

void __abs_exceed_err(){
	printf("Error: Absolute address exceeds machine size; zero used");
}

void __not_def_err(string s){
	printf("Error: %s is not defined; zero used", s.c_str());
}

void __rel_exceed_err(){
	printf("Error: Relative address exceeds module size; zero used");
}

void __usec_exceed_err(){
	printf("Error: External address exceeds length of uselist; treated as immediate");
}

void __uselist_notused_warning(int m, string s){
	printf("\nWarning: Module %d: %s appeared in the uselist but was not actually used", m, s.c_str());
}

void __val_toobig_warning(int m, string s, int v, int max){
	printf("Warning: Module %d: %s too big %d (max=%d) assume zero relative\n",m, s.c_str(), v, max);
}

void __opcode_err(){
	printf(" Error: Illegal opcode; treated as 9999");
}

void __imm_val_error(){
	printf(" Error: Illegal immediate value; treated as 9999");
}