#include "schedulers.h"
using namespace SCH;

FScheduler::FScheduler(){
	name = "FCFS";
}
void FScheduler::add_process(Process *proc){
	run_q.push_back(proc);
}
Process* FScheduler::get_next_process(){
	if(run_q.size()==0)
		return NULL;
	Process *res = run_q.front();
	run_q.pop_front();
	return res;
}
void FScheduler::print_q(){
	printf("SCHED (%lu):  ", run_q.size());
	for(Process* proc:run_q){
		printf("%d:%d  ",proc->pid, proc->state_ts);
	}
	printf("\n");
}