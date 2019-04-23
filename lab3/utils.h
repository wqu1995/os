#ifndef UTILS
#define UTILS
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
using std::vector;
using std::string;
using std::ifstream;
using std::cout;

#define VAILD_BIT 1
#define W_PROTECT_BIT 1
#define MODIFIED_BIT 1
#define REFERENCED_BIT 1
#define PAGEDOUT_BIT 1
#define FILE_MAPPED_BIT 1
#define INDEX_BITS 7
#define UNUSED_BITS 20
#define PAGE_NUM 64


typedef struct{
	unsigned vaild : VAILD_BIT;
	unsigned w_protect : W_PROTECT_BIT;
	unsigned modified : MODIFIED_BIT;
	unsigned reference : REFERENCED_BIT;
	unsigned pagedout : PAGEDOUT_BIT;
	unsigned filemapped : FILE_MAPPED_BIT;
	unsigned f_index : INDEX_BITS;
	unsigned unused : UNUSED_BITS;
}__attribute__((packed)) pte_t;

typedef struct{
	int start;
	int end;
	int w_protect;
	int filemapped;
}vma_t;

typedef struct{
	int vaild;
	int fid;
	int pid;
	int vpage;
	unsigned age;
	unsigned time;
}frame_t;

typedef struct {
	unsigned O : 1;
	unsigned P : 1;
	unsigned F : 1;
	unsigned S : 1;
	unsigned y : 1;
	unsigned f : 1;
	unsigned a : 1;
	unsigned etc : 21;
}__attribute__((packed))option_t;

extern vector<frame_t*> frame_table;
extern int *myrands;
extern int max_prio;
extern unsigned long instr_num;
extern option_t* ops;


namespace PARSER{
	class Parser{
		ifstream input;
		string line;
	public:
		Parser(string fname);
		~Parser();
		string get_next_line();
	};
}


void init_rands(char *s);
int myrandom(int burst);

int get_frame_index(frame_t* frame);
#endif