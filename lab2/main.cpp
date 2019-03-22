#include "utils.h"
#include "process.h"
#include "event.h"
#include "schedulers.h"

using namespace EVTQ;
using namespace PS;
using namespace SCH;
using std::min;

vector<Process*>proc_list;
Event_Q *evt_q;
Scheduler *sch;
int current_time = 0;
int prev_time = 0;
int proc_count=0;
double cpu_in_use = 0;
double io_in_use = 0;


void init_process(string s){
	ifstream input(s.c_str());
	if(!input){
		printf("failed to open source file\n");
		exit(EXIT_FAILURE);
	}
	int at, tc, cb, ib;
	while(input>>at>>tc>>cb>>ib){
		Process *temp = new Process(proc_count, at, tc, cb, ib);
		proc_list.push_back(temp);
		proc_count++;
		evt_q->put_event(TRANS_TO_READY, temp, at);
	}
//	evt_q->print_event();
}

void simulation(){
	printf("ShowEventQ:  ");
	evt_q->print_event();
	event *evt;
	bool call_sch;
	Process* current_running_proc = NULL;
	Process *longest_io_proc = NULL;
	while((evt = evt_q->get_event())){
		Process *proc = evt->evt_proc;
		current_time = evt->evt_time;
		prev_time = current_time-proc->state_ts;
		switch(evt->transition){
			case TRANS_TO_READY:{
				if(proc->current_state == STATE_CREATED)
					printf("%d %d %d: CREATED -> READY\n",current_time, proc->pid, prev_time);
				else if(proc->current_state == STATE_BLOCKED){
					proc->io_until = 0;
					printf("%d %d %d: BLOCK -> READY\n",current_time, proc->pid, prev_time);
				}
				
				proc->current_state = STATE_READY;
				proc->state_ts = current_time;

				if(longest_io_proc != NULL && longest_io_proc->pid == proc->pid){
					longest_io_proc = NULL;
					int temp_long = 0;
					for(Process *procx : proc_list){
						if(procx->current_state == STATE_BLOCKED && procx->io_until>temp_long){
							longest_io_proc = procx;
							temp_long = procx->io_until;
						}
					}
				}
				sch->add_process(proc);
				call_sch = true;

				break;

			}
			case TRANS_TO_RUN:{
				proc->current_state = STATE_RUNNING;
				proc->state_ts = current_time;
				proc->cpu_wait +=prev_time;
				int cb = min(proc->cpu_rem, myrandom(proc->cpu_burst));
				cpu_in_use+=cb;
				printf("%d %d %d: READY -> RUNNG cb=%d rem=%d prio=%d\n",current_time, proc->pid, prev_time, cb, proc->cpu_rem, proc->d_prio);
				proc->cpu_rem -= cb;
				
				printf("  AddEvent(%d:%d:%s):  ", current_time+cb, proc->pid, "BLOCK");
				evt_q->print_eventX();
					evt_q->put_event(TRANS_TO_BLOCK, proc, current_time+cb);
				printf("==>  ");
				evt_q->print_eventX();
				printf("\n");
				break;
			}
			case TRANS_TO_BLOCK:{
				if(current_running_proc->pid == proc->pid)
					current_running_proc = NULL;
				proc->state_ts = current_time;
				if(proc->cpu_rem == 0){
					proc->current_state = STATE_FINISHED;
					printf("%d %d %d: Done\n", current_time, proc->pid, prev_time);
				}
				else{
					proc->current_state = STATE_BLOCKED;
					int ib = myrandom(proc->io_burst);
					proc->io_wait+= ib;
					proc->io_until = current_time+ib;
					if(longest_io_proc == NULL){
						io_in_use+=ib;
						longest_io_proc = proc;
					}
					else if(longest_io_proc->io_until < proc->io_until){
						io_in_use+=proc->io_until - longest_io_proc->io_until;
						longest_io_proc = proc;
					}
					printf("%d %d %d: RUNNG -> BLOCK ib=%d rem=%d\n",current_time, proc->pid, prev_time, ib, proc->cpu_rem);
					printf("  AddEvent(%d:%d:%s):  ", current_time+ib, proc->pid, "READY");
					evt_q->print_eventX();
						evt_q->put_event(TRANS_TO_READY, proc, current_time+ib);
					printf("==>  ");
					evt_q->print_eventX();
					printf("\n");

				}
				call_sch = true;
				break;
			}
		}
		delete evt;
		evt = NULL;

		if(call_sch){
			if(evt_q->get_next_event_time() == current_time)
				continue;
			call_sch = false;
			if(current_running_proc == NULL){
				sch->print_q();
				current_running_proc = sch->get_next_process();
				if(current_running_proc == NULL)
					continue;
				
				printf("  AddEvent(%d:%d:%s):  ", current_time, current_running_proc->pid, "RUNNG");
				evt_q->print_eventX();
					evt_q->put_event(TRANS_TO_RUN, current_running_proc, current_time);

				printf("==>  ");
				evt_q->print_eventX();
				printf("\n");
			}
		}
	}
}

void print_result(){
	printf("%s\n",sch->name.c_str());
	double total_turnaround = 0;
	double total_cpu_wait;
	for(Process* proc : proc_list){
		total_turnaround+=proc->state_ts-proc->arr_time;
		total_cpu_wait += proc->cpu_wait;
		printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n",
			proc->pid,
			proc->arr_time, proc->total_cpu, proc->cpu_burst, proc->io_burst, proc->s_prio,
			proc->state_ts,proc->state_ts-proc->arr_time,
			proc->io_wait, proc->cpu_wait);
	}
	double cpu_util = cpu_in_use/current_time*100.0;
	double io_util = io_in_use/current_time*100.00;
	double throughput = (double)proc_count/(double)current_time*100.00;
	printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n", 
		current_time,
		cpu_util,
		io_util,
		total_turnaround/proc_count,
		total_cpu_wait/proc_count,
		throughput
		);
}

int main(int argc, char const *argv[]){
	init_rands("rfile");
	evt_q = new Event_Q();
	sch = new FScheduler();
	init_process(argv[1]);
	simulation();
	print_result();
	return 0;
}