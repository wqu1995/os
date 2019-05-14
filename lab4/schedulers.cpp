#include "schedulers.h"
#include <cstdlib>
using namespace SCH;

/*sorted_q::sorted_q(request_t *request){
	io = request;
	next = NULL;
}*/

FScheduler::FScheduler(){
	name = "FCFS";
}
void FScheduler::add_io(request_t* io){
	run_q.push_back(io);
}
request_t* FScheduler::get_next_io(){
	if(run_q.size()==0)
		return NULL;
	request_t* res = run_q.front();
	run_q.pop_front();
	return res;
}

int FScheduler::get_size(){
	return run_q.size();
}


SScheduler::SScheduler(){
	name = "SSTF";
}
void SScheduler::add_io(request_t* io){
	run_q.push_back(io);
}
request_t* SScheduler::get_next_io(){
	if(run_q.size()==0)
		return NULL;
	unsigned min = 0xFFFFFFFF;
	int index = -1;
	int i = 0;
	for(request_t* req : run_q){
		unsigned x =abs(req->track - current_track);
		if(x < min){
			min = x;
			index = i;
		}
		i++;
	}

	request_t* res = run_q[index];
	run_q.erase(run_q.begin()+index);
	return res;
}

int SScheduler::get_size(){
	return run_q.size();
}


LScheduler::LScheduler(){
	name = "LOOK";
}
void LScheduler::add_io(request_t* io){
	run_q.push_back(io);
}
request_t* LScheduler::get_next_io(){
	if(run_q.size() == 0)
		return NULL;
	unsigned min = 0xFFFFFFFF;
	int index = -1;
	int i;
	int x = 0;
	if(direction == 1){
		//pos
		i = 0;
		for(request_t* req : run_q){

			x = req->track - current_track;
			if(x < min && x >= 0){
				min = x;
				index = i;
			}
			i++;
		}

		if(index == -1){
			i = 0;
			for(request_t* req : run_q){

				x = current_track - req->track;
				if(x < min && x >= 0){
					min = x;
					index = i;
				}
				i++;
			}
		}
	}
	else if(direction == -1){
		i = 0;
		for(request_t* req : run_q){

			x = current_track - req->track;
			if(x<min && x >= 0){
				min = x;
				index = i;
			}
			i++;
		}

		if(index == -1){
			i = 0;
			for(request_t* req : run_q){

				x = req->track - current_track;
				if(x < min && x >= 0){
					min = x;
					index = i;
				}
				i++;
			}
		}
	}
	request_t* res = run_q[index];
	run_q.erase(run_q.begin()+index);
	return res;
}
int LScheduler::get_size(){
	return run_q.size();
}

CLScheduler:: CLScheduler(){
	name = "CLOOK";
}
void CLScheduler::add_io(request_t* io){
	run_q.push_back(io);
}
request_t* CLScheduler::get_next_io(){
	if(run_q.size() == 0)
		return NULL;
	unsigned min = 0xFFFFFFFF;
	int index = -1;
	int i;
	int x = 0;
		//pos
		i = 0;
		for(request_t* req : run_q){

			x = req->track - current_track;
			if(x < min && x >= 0){
				min = x;
				index = i;
			}
			i++;
		}

		if(index == -1){
			i = 0;
			int max = -1;
			for(request_t* req : run_q){
				x = current_track - req->track;
				if(x > max){
					max = x;
					index = i;
				}
				i++;
			}
		}

	request_t* res = run_q[index];
	run_q.erase(run_q.begin()+index);
	return res;
}
int CLScheduler::get_size(){
	return run_q.size();
}

FLScheduler::FLScheduler(){
	name = "FLOOK";
	activate = new vector<request_t*>();
	add = new vector<request_t*>();
}
void FLScheduler::add_io(request_t* io){
	add->push_back(io);
}
request_t* FLScheduler::get_next_io(){
	if(add->size() == 0 && activate->size() == 0)
		return NULL;
	if(activate->size() == 0){
		vector<request_t*> *temp = activate;
		activate = add;
		add = temp;
	}
	unsigned min = 0xFFFFFFFF;
	int index = -1;
	int i;
	int x = 0;
	if(direction == 1){
		//pos
		i = 0;
		for(request_t* req : *activate){

			x = req->track - current_track;
			if(x < min && x >= 0){
				min = x;
				index = i;
			}
			i++;
		}

		if(index == -1){
			i = 0;
			for(request_t* req : *activate){

				x = current_track - req->track;
				if(x < min && x >= 0){
					min = x;
					index = i;
				}
				i++;
			}
		}
	}
	else if(direction == -1){
		i = 0;
		for(request_t* req : *activate){

			x = current_track - req->track;
			if(x<min && x >= 0){
				min = x;
				index = i;
			}
			i++;
		}

		if(index == -1){
			i = 0;
			for(request_t* req : *activate){

				x = req->track - current_track;
				if(x < min && x >= 0){
					min = x;
					index = i;
				}
				i++;
			}
		}
	}
	request_t* res = (*activate)[index];
	activate->erase(activate->begin()+index);
	return res;
}

int FLScheduler::get_size(){
	return add->size()+activate->size();
}