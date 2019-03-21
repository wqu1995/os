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
	if(current == NULL){
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
	event *res = e;
	e = e->next;
	return res;
}
int Event_Q::get_next_event_time(){
	return e->evt_time;
}
void Event_Q::print_event(){
	event *current = e;
	while(current != NULL){
		printf("%d:%d ",current->evt_time, current->evt_proc->pid );
		current = current->next;
	}
	printf("\n");
}