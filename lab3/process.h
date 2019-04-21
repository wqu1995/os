#ifndef PROCESS
#define PROCESS

#include "utils.h"

namespace PS{
	class Process{
	public:
		int pid;
		int segment_num;
		pte_t* page_table;
		vma_t* segments;
		Process(int p, int vma_n);
		void set_vma(int i, int s, int e, int wp, int fm);
	};
}

extern PS::Process** proc_list;

#endif