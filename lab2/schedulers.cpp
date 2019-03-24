#include "schedulers.h"
using namespace SCH;

FScheduler::FScheduler(){
	name = "FCFS";
	quantum = 10000;
}
void FScheduler::add_process(Process *proc){
	proc->current_state = STATE_READY;
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
	quantum = 10000;
}
void LScheduler::add_process(Process *proc){
	proc->current_state = STATE_READY;
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
	quantum = 10000;
}
void SRScheduler::add_process(Process *proc){
	proc->current_state = STATE_READY;
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

RRScheduler::RRScheduler(int q){
	quantum = q;
	name = "RR "+to_string(quantum);
}
void RRScheduler::add_process(Process *proc){
	proc->current_state = STATE_READY;
	run_q.push_back(proc);
}
Process* RRScheduler::get_next_process(){
	if(run_q.size()==0)
		return NULL;
	Process *res = run_q.front();
	run_q.pop_front();
	return res;
}
void RRScheduler::print_q(){
	printf("SCHED (%lu):  ", run_q.size());
	for(Process* proc:run_q){
		printf("%d:%d  ",proc->pid, proc->state_ts);
	}
	printf("\n");
}

PRScheduler::PRScheduler(int q, int maxp){
	quantum = q;
	maxprio = maxp;
	name = "PRIO "+to_string(quantum);
	active_q = new sorted_q*[maxprio];
	expired_q = new sorted_q*[maxprio];
	for(int i = 0; i<maxprio; i++){
		active_q[i] = NULL;
		expired_q[i] = NULL;
	}
}

void PRScheduler::add_process(Process *proc){
	if(proc->current_state == STATE_BLOCKED)
		proc->d_prio = proc->s_prio-1;
	else if(proc->current_state == STATE_PREEMPTED)
		proc->d_prio--;
	sorted_q *current;
	sorted_q *temp = new sorted_q(proc);
	if(proc->d_prio == -1){
		//insert expired q
		proc->d_prio = proc->s_prio-1;
		current = expired_q[proc->d_prio];
		if(current==NULL){
			expired_q[proc->d_prio] = temp;
			return;
		}
	}
	else{
		current = active_q[proc->d_prio];
		if(current==NULL){
			active_q[proc->d_prio] = temp;
			return;
		}
	}

	proc->current_state = STATE_READY;
	

	while(current->next != NULL){
		current = current->next;
	}

	current->next = temp;
}

Process* PRScheduler::get_next_process(){
	Process *res = NULL;
	for(int i = maxprio-1; i>=0; i--){
		if(active_q[i] != NULL){
			res = active_q[i]->proc;
			active_q[i] = active_q[i]->next;
			return res;
		}
	}
	printf("switched queues\n");
	sorted_q** temp = expired_q;
	expired_q = active_q;
	active_q = temp;
	for(int i = maxprio-1; i>=0; i--){
		if(active_q[i] != NULL){
			res = active_q[i]->proc;
			active_q[i] = active_q[i]->next;
			return res;
		}
	}
	return NULL;
}

void PRScheduler::print_q(){
	printf("{ ");
	for(int i = maxprio-1; i>=0; i--){
		printf("[");
		sorted_q *current = active_q[i];
		if(current != NULL){
			while(current->next!= NULL){
				printf("%d,", current->proc->pid);
				current = current->next;
			}
			printf("%d", current->proc->pid);
		}
		printf("]");

	}
	printf("} : { ");
	for(int i = maxprio-1; i>=0; i--){
		printf("[");
		sorted_q *current = expired_q[i];
		if(current != NULL){
			while(current->next!= NULL){
				printf("%d,", current->proc->pid);
				current = current->next;
				
			}
			printf("%d", current->proc->pid);
		}
		printf("]");

	}
	printf("} :");
	printf("\n");
}