#include <ctype.h>
#include <unistd.h>
#include <string.h>
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

/******************
Function for initialize the process from the source file*/
void init_process(char* s){
	ifstream input(s);
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

/****************
Simulates DES*/
void simulation(){
	//printf("ShowEventQ:  ");
	//evt_q->print_event();
	//printf("\n");
	event *evt;
	bool call_sch;
	Process* current_running_proc = NULL;
	Process *longest_io_proc = NULL;

	while((evt = evt_q->get_event())){
		//Start getting events from the event queue
		Process *proc = evt->evt_proc; //current event process
		current_time = evt->evt_time;	//current time
		prev_time = current_time-proc->state_ts;	//how long did the process stay in the previous stage.
		switch(evt->transition){
			case TRANS_TO_READY:{
				proc->state_ts = current_time;
				if(proc->current_state == STATE_PREEMPTED){
					//printf("%d %d %d: RUNNG -> READY  cb=%d rem=%d prio=%d\n",current_time, proc->pid, prev_time, proc->current_cpu_time, proc->cpu_rem, proc->d_prio);

				}
				else{
					/*if(proc->current_state == STATE_CREATED)
						printf("");
						//printf("%d %d %d: CREATED -> READY\n",current_time, proc->pid, prev_time);
					else */
					if(proc->current_state == STATE_BLOCKED){
						proc->io_until = 0;
						//printf("%d %d %d: BLOCK -> READY\n",current_time, proc->pid, prev_time);

					}
					if(longest_io_proc != NULL && longest_io_proc->pid == proc->pid){
						//find the process with longest io time
						longest_io_proc = NULL;
						int temp_long = 0;
						for(Process *procx : proc_list){
							if(procx->current_state == STATE_BLOCKED && procx->io_until>temp_long){
								longest_io_proc = procx;
								temp_long = procx->io_until;
							}
						}
					}
				}

				//add process to the scheduler
				sch->add_process(proc, current_running_proc, evt_q);
				call_sch = true;

				break;

			}
			case TRANS_TO_RUN:{
				proc->current_state = STATE_RUNNING;
				proc->state_ts = current_time;
				proc->cpu_wait +=prev_time;
				int cb;

				if(proc->current_cpu_time != 0){
					//process comes from a preempt state, thus it has not finish its cpu burst yet
					cb = proc->current_cpu_time;
				}
				else{
					//process comes from a create or block state, so we generate a new cpu burst
					cb = min(proc->cpu_rem, myrandom(proc->cpu_burst));
					proc->current_cpu_time = cb;
					//cpu_in_use tracks the total time that processor is in use
					cpu_in_use+=cb;

				}
				//printf("%d %d %d: READY -> RUNNG cb=%d rem=%d prio=%d\n",current_time, proc->pid, prev_time, cb, proc->cpu_rem, proc->d_prio);
				if(cb <=sch->quantum){
					//cpu burst is less than quantum, so the process is not scheduled to preemption.

					//printf("  AddEvent(%d:%d:%s):  ", current_time+cb, proc->pid, "BLOCK");
					//evt_q->print_eventX();
						evt_q->put_event(TRANS_TO_BLOCK, proc, current_time+cb);
					//printf("==>  ");
					//evt_q->print_eventX();
					//printf("\n");
				}
				else{
					//cpu burst exceeds the amount of quantum each process allows. 
					cb = sch->quantum;

					//printf("  AddEvent(%d:%d:%s):  ", current_time+cb, proc->pid, "PREEMPT");
					//evt_q->print_eventX();
						evt_q->put_event(TRANS_TO_PREEMPT, proc, current_time+cb);
					//printf("==>  ");
					//evt_q->print_eventX();
					//printf("\n");
				}

				proc->cpu_rem -= cb;
				
				break;
			}
			case TRANS_TO_BLOCK:{
				if(current_running_proc->pid == proc->pid) 
					current_running_proc = NULL;

				proc->state_ts = current_time;
				proc->current_cpu_time = 0;
				if(proc->cpu_rem == 0){
					//process finishes its job
					proc->current_state = STATE_FINISHED;
					//printf("%d %d %d: Done\n", current_time, proc->pid, prev_time);
				}
				else{
					proc->current_state = STATE_BLOCKED;
					int ib = myrandom(proc->io_burst);
					proc->io_wait+= ib;
					proc->io_until = current_time+ib;
					//keep track how long is the io in use
					if(longest_io_proc == NULL){
						io_in_use+=ib;
						longest_io_proc = proc;
					}
					else if(longest_io_proc->io_until < proc->io_until){
						io_in_use+=proc->io_until - longest_io_proc->io_until;
						longest_io_proc = proc;
					}
					//printf("%d %d %d: RUNNG -> BLOCK ib=%d rem=%d\n",current_time, proc->pid, prev_time, ib, proc->cpu_rem);
					//printf("  AddEvent(%d:%d:%s):  ", current_time+ib, proc->pid, "READY");
					//evt_q->print_eventX();
						evt_q->put_event(TRANS_TO_READY, proc, current_time+ib);
					//printf("==>  ");
					//evt_q->print_eventX();
					//printf("\n");

				}
				call_sch = true;
				break;
			}
			case TRANS_TO_PREEMPT:{
				if(current_running_proc->pid == proc->pid)
					current_running_proc = NULL;
				//if process is preempted in middle of its running state, it's total cpu time should be updated
				proc->current_cpu_time -=prev_time;

				proc->current_state = STATE_PREEMPTED;
				evt_q->put_event(TRANS_TO_READY, proc, current_time);
			}
		}
		delete evt;
		evt = NULL;

		if(call_sch){
			if(evt_q->get_next_event_time() == current_time)
				continue;
			call_sch = false;

			if(current_running_proc == NULL){
				//sch->print_q();

				current_running_proc = sch->get_next_process();
				if(current_running_proc == NULL)
					continue;
				
				//printf("  AddEvent(%d:%d:%s):  ", current_time, current_running_proc->pid, "RUNNG");
				//evt_q->print_eventX();
					evt_q->put_event(TRANS_TO_RUN, current_running_proc, current_time);

				//printf("==>  ");
				//evt_q->print_eventX();
				//printf("\n");
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

int main(int argc, char *argv[]){
	opterr = 0;
	int c;
	char *s_val = NULL;
	int quantum = -1;
	while((c = getopt(argc, argv, "s:"))!=-1){
		switch(c){
			case 's':
				s_val = optarg;
				break;
			case '?':
				printf("usage: <program> [-s<schedspec>] inputfile randfile\n");
				exit(EXIT_FAILURE);
				break;
			default:
				printf("usage: <program> [-s<schedspec>] inputfile randfile\n");
				exit(EXIT_FAILURE);
				break;
		}
	}

	char *source;
	char *rfile;

	if(argv[optind] == NULL){
		printf("usage: <program> [-s<schedspec>] inputfile randfile\n");
		exit(EXIT_FAILURE);
	}
	else{
		source = argv[optind];
		optind++;
	}
	if(argv[optind] == NULL){
		printf("usage: <program> [-s<schedspec>] inputfile randfile\n");
		exit(EXIT_FAILURE);
	}
	else{
		rfile = argv[optind];
		optind++;
	}

	char s_type;
	char *pos;
	sscanf(s_val, "%c%*d%*d", &s_type);
	switch(s_type){
		case 'F':
			sch = new FScheduler();
			break;
		case 'L':
			sch = new LScheduler();
			break;
		case 'S':
			sch = new SRScheduler();
			break;
		case 'R':
			sscanf(s_val, "%*c%d:%*d", &quantum);
			sch = new RRScheduler(quantum);
			break;
		case 'P':
			if((pos = strchr(s_val, ':')) != NULL){
				sscanf(s_val, "%*c%d:%d", &quantum, &max_prio);
			}
			else{
				sscanf(s_val, "%*c%d:%*d", &quantum);
				max_prio = 4;
			}
			sch = new PRScheduler(quantum, max_prio);
			break;
		case 'E':
			if((pos = strchr(s_val, ':')) != NULL){
				sscanf(s_val, "%*c%d:%d", &quantum, &max_prio);
			}
			else{
				sscanf(s_val, "%*c%d:%*d", &quantum);
				max_prio = 4;
			}
			sch = new PPRScheduler(quantum, max_prio);
			break;
		default:
			printf("Accepted options: FLSRPE\n");
			exit(EXIT_FAILURE);
	}

	init_rands(rfile);
	evt_q = new Event_Q();
	init_process(source);
	simulation();
	print_result();

	for(Process* proc : proc_list)
		delete proc;
	
	return 0;
}