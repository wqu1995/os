#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

using std::string;
using std::stringstream;
using std::ifstream;
using std::cout;
namespace TK{
	class Tokens{
		ifstream input;
		string line;
		stringstream tokens;
		int offset;
		int linec;
		int tok_leng;
		int current_end_l;
		int current_end_o;
		int last_end_l;
		int last_end_o;

	public:
		int char_offset;
		int char_linec;
		Tokens(string fname);
		~Tokens();

		string get_token();
	};
}
#endif