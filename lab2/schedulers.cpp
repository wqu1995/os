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

LScheduler::LScheduler(){
	name = "LCFS";
}
void LScheduler::add_process(Process *proc){
	run_q.push_front(proc);
}
Process* LScheduler::get_next_process(){
	if(run_q.size()==0)
		return NULL;
	Process *res = run_q.front();
	run_q.pop_front();
	return res;
}
void LScheduler::print_q(){
	printf("SCHED (%lu):  ", run_q.size());
	for(Process* proc:run_q){
		printf("%d:%d  ",proc->pid, proc->state_ts);
	}
	printf("\n");
}
sorted_q::sorted_q(Process *ps){
	proc = ps;
	next = NULL;
}

SRScheduler::SRScheduler(){
	name = "SRTF";
	run_q = NULL;
	count = 0;
}
void SRScheduler::add_process(Process *proc){
	sorted_q *temp = new sorted_q(proc);
	count++;
	sorted_q *current = run_q;
	if(run_q==NULL || current->proc->cpu_rem > proc->cpu_rem){
		temp->next = current;
		run_q = temp;
		return;
	}
	else{
		while(current->next != NULL && current->next->proc->cpu_rem <= proc->cpu_rem){
			current = current->next;
		}
		temp->next = current->next;
		current->next = temp;
	}
}
Process* SRScheduler::get_next_process(){
	if(run_q==NULL)
		return NULL;
	sorted_q *res = run_q;
	run_q = run_q->next;
	count--;
	return res->proc;
}
void SRScheduler::print_q(){
	printf("SCHED (%d):  ", count);
	sorted_q *current = run_q;
	while(current !=NULL){
		printf("%d:%d  ",current->proc->pid, current->proc->state_ts);
		current = current->next;
	}
	printf("\n");
}