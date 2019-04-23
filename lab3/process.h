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


typedef struct {
	unsigned long umaps;
	unsigned long maps;
	unsigned long ins;
	unsigned long outs;
	unsigned long fins;
	unsigned long fouts;
	unsigned long zeros;
	unsigned long segv;
	unsigned long segprot;
}proc_stats;
extern PS::Process** proc_list;
extern proc_stats* pstats;

#endif