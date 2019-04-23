#include "utils.h"
using namespace PARSER;

int rand_count;
int *myrands;
int ofs = 0;
vector<frame_t*> frame_table;
unsigned long instr_num = 0;
option_t* ops;

void init_rands(char *s){
	ifstream input(s);
	if(!input){
		printf("failed to open random file\n");
		exit(EXIT_FAILURE);
	}
	input>>rand_count;
	myrands = (int*)calloc(rand_count,sizeof(int));
	int temp;
	int i = 0;
	while(input>>temp)
		myrands[i++] = temp;
}
int myrandom(int burst){
	int res =(myrands[ofs] % burst); 
	//printf("b:%d, o:%d, v:%d, r:%d\n",burst, ofs, myrands[ofs], res);
	if(ofs == rand_count-1)
		ofs = 0;
	else
		ofs++;
	return res;
}

Parser::Parser(string fname){
	input.open(fname.c_str());
	if(!input){
		cout<<"Fail to open file "<<fname<<"\n";
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


int get_frame_index(frame_t* frame){
	int i;
	for(i = 0; i< frame_table.size(); i++){
		if(frame->vaild && frame->pid == frame_table[i]->pid && frame->vpage == frame_table[i]->vpage){
			//printf("pid: %d, vpage: %d i: %d\n",frame->pid, frame->vpage, i);
			return i;
		}
	}
	return i-1;
}

