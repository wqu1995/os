#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
using std::string;

namespace S{
	class Symbol{
	public:
		string name;
		int val;
		int module;
		bool used;
		int rel_val;
		bool mult_def;

		Symbol();
		Symbol(string n, int v, int m, int rv);
	};
}

namespace US{
	class Usymbol{
	public:
		string name;
		bool used;
		int module;

		Usymbol();
		Usymbol(string n, int m);
	};
}
#endif