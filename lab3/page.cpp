#include "page.h"
using namespace PG;

FPager::FPager(){
	counter = 0;
}

frame_t* FPager::select_victim_frame(){
	if(counter == frame_table.size())
		counter = 0;
	printf("ASELECT %d\n", counter);
	frame_t* temp =frame_table[counter++];
	return temp;
}

RPager::RPager(){}

frame_t* RPager::select_victim_frame(){
	int index = myrandom(frame_table.size());
	//printf("index: %d\n", index);
	//printf("ASELECT %d\n", index);
	frame_t* temp = frame_table[index];
	return temp;
}

CPager::CPager(){
	counter = 0;
}

frame_t* CPager::select_victim_frame(){
	frame_t* temp = NULL;
	while(temp == NULL){
		//printf("%d, %d\n",counter, frame_table.size());
		if(proc_list[frame_table[counter]->pid]->page_table[frame_table[counter]->vpage].reference){
			proc_list[frame_table[counter]->pid]->page_table[frame_table[counter]->vpage].reference = 0;
		}
		else{
		//	printf("in here\n");
			temp = frame_table[counter];
		}
		//printf("%d, %d, %d, %d\n", counter, frame_table[counter]->pid, frame_table[counter]->vpage, proc_list[frame_table[counter]->pid]->page_table[frame_table[counter]->vpage].reference);
		if(counter == frame_table.size()-1){
			counter = 0;
		}
		else
			counter++;
	}
	return temp;
}