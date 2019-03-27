#include "event.h"

using namespace EVT;
using namespace EVTQ;

/***********
definition of an event*/
event::event(trans_state_t ts, Process* proc, int time){
	transition = ts;
	evt_proc = proc;
	evt_time = time;
	next = NULL;
}

Event_Q::Event_Q(){
	e = NULL;
}

/************
put event in the correct position of the queue*/
void Event_Q::put_event(trans_state_t ts, Process* proc, int time){
	event *temp = new event(ts, proc, time);

	event *current = e;
	if(current == NULL || current->evt_time > time){
		//head
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

/****************
return the next event */
event* Event_Q::get_event(){
	if(e == NULL)
		return NULL;
	event *res = e;
	e = e->next;
	return res;
}

/*************
return the next event time*/
int Event_Q::get_next_event_time(){
	if(e == NULL)
		return -1;
	return e->evt_time;
}

/****************
print event from the queue*/
void Event_Q::print_event(){
	event *current = e;
	while(current != NULL){
		printf("%d:%d ",current->evt_time, current->evt_proc->pid );
		current = current->next;
	}
}

/****************
print event from the queue*/
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

/****************
return next event pid*/
int Event_Q::get_next_event_pid(){
	if(e == NULL)
		return -1;
	return e->evt_proc->pid;
}

/*******************
remove proc's next event from the queue*/
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


/******************
return the next event that belongs to proc*/
event* Event_Q::get_next_proc_event(Process* proc){
	if(e == NULL)
		return NULL;
	event * current = e;
	while(current->evt_proc->pid != proc->pid)
		current = current->next;
	return current;
}