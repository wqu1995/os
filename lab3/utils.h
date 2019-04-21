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
}frame_t;

extern vector<frame_t*> frame_table;
extern int *myrands;
extern int max_prio;

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