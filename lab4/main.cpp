#include "utils.h"
#include "schedulers.h"
#include <deque>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
using namespace PARSER;
using namespace SCH;
using std::deque;

Parser *parser;
Scheduler *sch;
deque<request_t*> request_in;
deque<request_t*> request_done;
int counter = 0;
int total_move = 0;

void init_events(){
	string s= parser->get_next_line();
	int arr, track;
	int i = 0;
	while(!s.empty()){
		request_t* temp = (request_t*)calloc(1, sizeof(request_t));
		sscanf(s.c_str(), "%d %d", &arr, &track);
		temp->id = i++;
		temp->arr_time = arr;
		temp->track = track;
		request_in.push_back(temp);
		request_done.push_back(temp);
		s = parser->get_next_line();
	}
}

void check_arrive(){

}

void simulation(){
	request_t* current_io = NULL;
	
	//printf("TRACE\n");
	for(;;){
		//printf("%d, %d\n", counter, current_track);
		//check arrive
		//printf("current track %d\n", current_track);
		if(request_in.size() != 0 && request_in.front()->arr_time == counter){
			//printf("%d:\t%d add %d\n", counter, request_in.front()->id, request_in.front()->track);
			sch->add_io(request_in.front());
			request_in.pop_front();
		}
		//check finish		
		if(current_io != NULL && current_track == current_io->track){
			current_io->fin_time = counter;
			//printf("%d:\t%d finish %d\n", counter, current_io->id, current_io->fin_time- current_io->arr_time );
			//request_done.push_back(current_io);
			current_io = NULL;
			if(current_track == 0)
				direction = 1;
		}
		//inc track
		if(current_io != NULL && current_track != current_io->track){
			current_track+=direction;
			total_move++;
		}
		//start new io
		while(current_io == NULL && sch->get_size() != 0){
			//printf("in start new\n");
			if((current_io = sch->get_next_io()) != NULL){
				current_io->start_time = counter;
				//printf("%d:\t%d issue %d %d\n", counter, current_io->id, current_io->track, current_track);
				if(current_io->track < current_track){
					direction = -1;
					current_track += direction;
					total_move++;
				}
				else if(current_io->track > current_track){
					direction = 1;
					current_track += direction;
					total_move++;
				}
				else{
					//track is already at position
					current_io->fin_time = counter;
					//printf("%d:\t%d finish %d\n", counter, current_io->id, current_io->fin_time- current_io->arr_time );
					//request_done.push_back(current_io);
					current_io = NULL;
				}

				
			}
		}
		if(current_io == NULL && sch->get_size()==0 && request_in.size()==0)
			break;
		counter++;
	}
}

void print_result(){
	double i = 0.0;
	double total_turnaround = 0.0;
	double total_wait = 0.0;
	int maxwait = 0;
	for(request_t* out : request_done){
		printf("%5d: %5d %5d %5d\n",out->id, out->arr_time, out->start_time, out->fin_time);
		total_turnaround += out->fin_time - out->arr_time;
		int wait_time = out->start_time - out->arr_time;
		total_wait += wait_time;
		if(wait_time>maxwait)
			maxwait = wait_time;
		i++;
	}
	printf("SUM: %d %d %.2lf %.2lf %d\n", counter, total_move, total_turnaround/i, total_wait/i, maxwait);
}
int main(int argc, char *argv[]){
	opterr = 0;
	int c;
	char *algo = NULL;

	while((c = getopt(argc, argv, "s:")) != -1){
		switch(c){
			case 's':
				algo = optarg;
				break;
			case '?':
				printf("usage: ./iosched –s<schedalgo>[options] <inputfile>\n");
				exit(EXIT_FAILURE);
				break;
			default:
				printf("usage: ./iosched –s<schedalgo>[options] <inputfile>\n");
				exit(EXIT_FAILURE);
				break;
		}
	}
	if(!algo || strlen(algo)>1){
		printf("usage: ./iosched –s<schedalgo>[options] <inputfile>\n");
		exit(EXIT_FAILURE);
	}

	char *source;

	if(argv[optind] == NULL){
		printf("usage: ./iosched –s<schedalgo>[options] <inputfile>\n");
		exit(EXIT_FAILURE);
	}
	else{
		source = argv[optind];
	}

	switch(algo[0]){
		case 'i':
			sch = new FScheduler();
			break;
		case 'j':
			sch = new SScheduler();
			break;
		case 's':
			sch = new LScheduler();
			break;
		case 'c':
			sch = new CLScheduler();
			break;
		case 'f':
			sch = new FLScheduler();
			break;
		default:
			printf("supported algos: ijscf\n");
			exit(EXIT_FAILURE);
	}
	parser = new Parser(source);
	init_events();
	simulation();
	print_result();
	return 0;
}