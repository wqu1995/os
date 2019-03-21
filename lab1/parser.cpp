#include "parser.h"

void parse1(string fname){
	Tokens tok(fname);

	string token = tok.get_token();
	while(!token.empty()){
		module_count++;
		if(int_check(token) ==0)
			__parseerror(0, tok.char_linec, tok.char_offset);
		int defcount;
		defcount = stoi(token);
		if(defcount>16)
			__parseerror(4, tok.char_linec, tok.char_offset);
		//cout<<defcount<<endl;
		for(int i = 0; i< defcount; i++){
			bool mult_def = false;
			string name = tok.get_token();
			if(sym_check(name)==0){
				__parseerror(1, tok.char_linec, tok.char_offset);
			}
			if(sym_length_check(name) == 0){
				__parseerror(3, tok.char_linec, tok.char_offset);
			}

			for(int i=0; i< var_count; i++){
				if(name.compare(sym_table[i].name)==0){
					//__multi_def_err();
					sym_table[i].mult_def = true;
					mult_def = true;
					token = tok.get_token();
					if(int_check(token) == 0)
						__parseerror(0, tok.char_linec, tok.char_offset);
					break;
				}
			}
			if(!mult_def){
				token = tok.get_token();
				if(int_check(token) == 0)
					__parseerror(0, tok.char_linec, tok.char_offset);
				int val = stoi(token);
				sym_table[var_count++] = Symbol(name, val+global_addr, module_count, val);
			}

		}
		token = tok.get_token();
		if(int_check(token) ==0)
			__parseerror(0, tok.char_linec, tok.char_offset);
		int usecount;
		usecount = stoi(token);
		//cout<<usecount<<" "<<tok.char_linec<<" "<<tok.char_offset<<endl;
		if(usecount>16)
			__parseerror(5, tok.char_linec, tok.char_offset);
		//cout<<usecount<<endl;
		while(usecount>0){
			token = tok.get_token();
			if(sym_check(token)==0){
				__parseerror(1, tok.char_linec, tok.char_offset);
			}
			if(sym_length_check(token) == 0){
				__parseerror(3, tok.char_linec, tok.char_offset);
			}
			usecount--;
		}
		token = tok.get_token();
		if(int_check(token) ==0)
			__parseerror(0, tok.char_linec, tok.char_offset);
		int codecount;
		codecount = stoi(token);
		if(global_addr+codecount > 512)
			__parseerror(6, tok.char_linec, tok.char_offset);
		//cout<<codecount<<endl;
		for(int i = 0; i< codecount; i++){
			token = tok.get_token();
			if(addr_check(token) == 0)
				__parseerror(2, tok.char_linec, tok.char_offset);
			token = tok.get_token();
			if(int_check(token) ==0)
				__parseerror(0, tok.char_linec, tok.char_offset);
		}
		for(int i = var_count- defcount; i< var_count; i++){
			if(sym_table[i].rel_val>=codecount){
				__val_toobig_warning(sym_table[i].module, sym_table[i].name, sym_table[i].rel_val, codecount-1);
				sym_table[i].val = 0+global_addr;
			}
		}
		global_addr+=codecount;
		token = tok.get_token();
	}
	cout<<"Symbol Table";
	for(int i = 0; i< var_count; i++){
		cout<<endl<<sym_table[i].name<<"="<<sym_table[i].val;
		if(sym_table[i].mult_def)
			__multi_def_err();
	}


}

void parse2(string fname){
	global_addr=0;
	module_count=0;
	cout<<"Memory Map";
	Tokens tok(fname);
	int counter = 0;

	string token = tok.get_token();
	while(!token.empty()){
		module_count++;
		int defcount;
		defcount = stoi(token);
		//cout<<defcount<<endl;
		while(defcount>0){
			tok.get_token();
			tok.get_token();
			defcount--;
		}

		int usecount;
		usecount = stoi(tok.get_token());
		//cout<<usecount<<endl;
		for(int i = 0; i<usecount; i++){
			string name = tok.get_token();
			use_table[i]= Usymbol(name, module_count);
		}

		int codecount;
		codecount = stoi(tok.get_token());
		//cout<<codecount<<endl;
		for(int c = 0; c<codecount; c++){
			string instr_type = tok.get_token();
			int instr_val = stoi(tok.get_token());
			if(instr_val >=10000){
				int opcode = 9;
				int operand = 999;
				if(instr_type.compare("I")==0){
					printf("\n%03d: %d%03d", counter++, opcode,operand);
					__imm_val_error();
				}
				else{
					printf("\n%03d: %d%03d", counter++, opcode,operand);
					__opcode_err();
				}
			}
			else{
				int opcode = instr_val/1000;
				int operand = instr_val%1000;
				//cout<<instr_val<<" "<<operand<<endl;

				if(instr_type.compare("I")==0){
					printf("\n%03d: %d%03d", counter++, opcode,operand);
				}
				else if(instr_type.compare("A")==0){
					if(operand>=512){
						operand = 0;
						printf("\n%03d: %d%03d ", counter++, opcode,operand);
						__abs_exceed_err();
					}
					else{
						printf("\n%03d: %d%03d", counter++, opcode,operand);
					}
				}
				else if(instr_type.compare("R")==0){
					if(operand>=codecount){
						operand = 0;
						printf("\n%03d: %d%03d ", counter++, opcode,operand+global_addr);
						__rel_exceed_err();

					}
					else{
						printf("\n%03d: %d%03d", counter++, opcode,operand+global_addr);
					}
				}
				else if(instr_type.compare("E")==0){
					//cout<<use_table[operand]<<endl;
					if(operand >= usecount){
						printf("\n%03d: %d%03d ", counter++, opcode,operand);
						__usec_exceed_err();
					}
					else{
						bool found = false;
						for(int u=0; u<var_count; u++){
							if(sym_table[u].name.compare(use_table[operand].name) == 0){
								printf("\n%03d: %d%03d", counter++, opcode,sym_table[u].val);
								sym_table[u].used = true;
								use_table[operand].used = true;
								found = true;
								break;
							}
						}
						if(!found){
							use_table[operand].used = true;
							operand = 0;
							printf("\n%03d: %d%03d ", counter++, opcode,operand);
							__not_def_err(use_table[operand].name);
						}
					}

					//printf("%03d: %d%03d\n", i++, opcode,sym_table[operand].val);
				}				
			}


		}

		for(int i = 0; i<usecount; i++){
			if(!use_table[i].used)
				__uselist_notused_warning(use_table[i].module, use_table[i].name);

		}
		global_addr+=codecount;

		token = tok.get_token();
	}
}