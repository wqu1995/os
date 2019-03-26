#include "event.h"

using namespace EVT;
using namespace EVTQ;

event::event(trans_state_t ts, Process* proc, int time){
	transition = ts;
	evt_proc = proc;
	evt_time = time;
	next = NULL;
}

Event_Q::Event_Q(){
	e = NULL;
}

void Event_Q::put_event(trans_state_t ts, Process* proc, int time){
	event *temp = new event(ts, proc, time);

	event *current = e;
	if(current == NULL || current->evt_time > time){
		temp->next = current;
		e = temp;
		return;
	}
	else{
		while(current->next != NULL && current->next->evt_time <=time){
			current = current->next;
		}
		temp->next = current->next;
		current->next = temp;
	}
}

event* Event_Q::get_event(){
	if(e == NULL)
		return NULL;
	event *res = e;
	e = e->next;
	return res;
}
int Event_Q::get_next_event_time(){
	if(e == NULL)
		return -1;
	return e->evt_time;
}
void Event_Q::print_event(){
	event *current = e;
	while(current != NULL){
		printf("%d:%d ",current->evt_time, current->evt_proc->pid );
		current = current->next;
	}
}

void Event_Q::print_eventX(){
	event *current = e;
	while(current != NULL){
		printf("%d:%d:", current->evt_time, current->evt_proc->pid);
		switch(current->transition){
			case TRANS_TO_READY:
				printf("READY  "); 
				break;
			case TRANS_TO_RUN:
				printf("RUNNG  ");
				break;
			case TRANS_TO_BLOCK:
				printf("BLOCK  ");
				break;
			case TRANS_TO_PREEMPT:
				printf("PREEMPT  ");
				break;
		}
		current = current->next;
	}
}

int Event_Q::get_next_event_pid(){
	if(e == NULL)
		return -1;
	return e->evt_proc->pid;
}

event* Event_Q::remove_event(Process* proc){
	if(e == NULL)
		return NULL;
	if(e->evt_proc->pid == proc->pid){
		//head
		event *temp = e;
		e = e->next;
		return temp;
	}

	event *current = e;
	while(current->next != NULL && current->next->evt_proc->pid != proc->pid)
		current = current->next;
	if(current->next == NULL)
		return NULL;
	event *temp = current->next;
	current->next = current->next->next;
	return temp;

}

event* Event_Q::get_next_proc_event(Process* proc){
	if(e == NULL)
		return NULL;
	event * current = e;
	while(current->evt_proc->pid != proc->pid)
		current = current->next;
	return current;
}