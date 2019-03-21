#ifndef ERROR_H
#define ERROR_H
#include <string>
using std::string;

void __parseerror(int errcode, int linenum, int lineoffset);
void __not_used_warning(int m, string s);
void __multi_def_err();
void __abs_exceed_err();
void __not_def_err(string s);
void __rel_exceed_err();
void __usec_exceed_err();
void __uselist_notused_warning(int m, string s);
void __val_toobig_warning(int m, string s, int v, int max);
void __opcode_err();
void __imm_val_error();

#endif