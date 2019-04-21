#include "process.h"
using namespace PS;

Process** proc_list;

Process::Process(int p, int vma_n){
	pid = p;
	segment_num = vma_n;
	segments = (vma_t*)calloc(vma_n, sizeof(vma_t));
	page_table = (pte_t*)calloc(PAGE_NUM, sizeof(pte_t));
}
void Process::set_vma(int i, int s, int e, int wp, int fm){
	segments[i].start = s;
	segments[i].end = e;
	segments[i].w_protect = wp;
	segments[i].filemapped = fm;

}