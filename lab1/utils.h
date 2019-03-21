#ifndef UTILS_H
#define UTILS_H
#include <regex>
#include "error.h"
#include "symbol.h"

using std::regex;
using namespace S;
using namespace US;

extern int var_count;
extern int global_addr;
extern int module_count;
extern Symbol sym_table[256];
extern Usymbol use_table[16];


int int_check(string s);
int addr_check(string s);
int sym_check(string s);
int sym_length_check(string s);
void check_defined();
#endif