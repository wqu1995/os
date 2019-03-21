#include "symbol.h"

using namespace S;
using namespace US;

Symbol::Symbol(){}
Symbol::Symbol(string n, int v, int m, int rv){
	name = n;
	val = v;
	module = m;
	used = false;
	rel_val = rv;
	mult_def = false;
}


Usymbol::Usymbol(){}
Usymbol::Usymbol(string n, int m){
	name = n;
	module = m;
	used = false;
}