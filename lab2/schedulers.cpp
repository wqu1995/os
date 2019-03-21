#include "schedulers.h"
using namespace SCH;


void FScheduler::add_process(Process *proc){
	run_q.push_back(proc);
}
Process* FScheduler::get_next_process(){
	Process *res = run_q.front();
	run_q.pop_front();
	return res;
}