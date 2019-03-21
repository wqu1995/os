#include "token.h"

using namespace TK;

Tokens::Tokens(string fname){
	input.open(fname.c_str());
	if(!input){
		cout<<"Fail to open file "<<fname<<"\n";
		exit (EXIT_FAILURE);
	}
	if(!input.eof()){
		linec = 1;
		offset = 0;
		getline(input,line);
		current_end_l = 1;
		current_end_o = line.length();
	}
	last_end_l = 0;
	last_end_o = 0;
	tok_leng = 0;
}
Tokens::~Tokens(){
	input.close();
}
string Tokens::get_token(){
	tokens.str(line);
	tokens.seekg(offset);

	string rst;

	while(rst.empty()){
		while(!tokens.eof()){
			while(tokens.peek()==' ' || tokens.peek()=='\t' || tokens.peek()=='\n'){
				tokens.seekg(++offset);
			}
			if(tokens.eof()){
				if(!input.eof()){
					getline(input,line);
					linec++;
					last_end_o = current_end_o;
					last_end_l = current_end_l;
					current_end_l = linec;
					current_end_o = line.length();
					tokens.str(line);
					offset = 0;
					tokens.clear();
				}
				else{
					char_linec = last_end_l;
					char_offset = last_end_o+1;
					return "";
				}
			}
			else{
				char_offset = offset+1;
				char_linec = linec;
				tokens>>rst;
				offset = tokens.tellg();

				if(tokens.eof()){
					if(!input.eof()){
						getline(input,line);
						linec++;
						offset = 0;
						last_end_o = current_end_o;
						last_end_l = current_end_l;
						current_end_l = linec;
						current_end_o = line.length();
					}
				}
				tokens.clear();
				tok_leng = rst.length();
				return rst;
			}
		}
	}
}