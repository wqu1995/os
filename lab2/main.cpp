#include "utils.h"
#include "process.h"
#include "event.h"
#include "schedulers.h"

using namespace EVTQ;
using namespace PS;
using namespace SCH;

vector<Process*>proc_list;
Event_Q *evt_q;
Scheduler *sch;
int current_time = 0;
int prev_time = 0;


void init_process(string s){
	ifstream input(s.c_str());
	if(!input){
		printf("failed to open source file\n");
		exit(EXIT_FAILURE);
	}
	int at, tc, cb, ib;
	int i = 0;
	while(input>>at>>tc>>cb>>ib){
		Process *temp = new Process(i, at, tc, cb, ib);
		proc_list.push_back(temp);
		i++;
		evt_q->put_event(TRANS_TO_READY, temp, at);
	}
	evt_q->print_event();
}

void simulation(){
	event *evt;
	bool call_sch;
	Process* current_running_proc = NULL;
	while((evt = evt_q->get_event())){
		Process *proc = evt->evt_proc;
		current_time = evt->evt_time;
		prev_time = current_time-proc->state_ts;
		switch(evt->transition){
			case TRANS_TO_READY:
				break;
			case TRANS_TO_RUN:
				break;
			case TRANS_TO_BLOCK:
				break;
		}
		delete evt;
		evt = NULL;

		if(call_sch){
			if(evt_q->get_next_event_time() == current_time)
				continue;
			call_sch = false;
			if(current_running_proc == NULL){
				current_running_proc = sch->get_next_process();
				if(current_running_proc == NULL)
					evt_q->put_event(TRANS_TO_RUN, current_running_proc, current_time);

			}
		}
	}
}

int main(int argc, char const *argv[]){
	init_rands("rfile");
	evt_q = new Event_Q();
	sch = new FScheduler();
	init_process(argv[1]);

	for(Process* proc : proc_list){
		printf("%d\n",proc->pid );
	}
	return 0;
}