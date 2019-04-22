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

EPager::EPager(){
	counter = 0;
	ins = 0;
}

frame_t* EPager::select_victim_frame(){
	int c1 = -1;
	int c2 = -1;
	int c3 = -1;
	int c4 = -1;
	int c = -1;
	int frame_index = -1;
	int scanned = 0;
	int reset = 0;
	if((instr_num-ins) >=50){
		ins = instr_num;
		reset = 1;
	}
	
	int i = counter;
	printf("ASELECT: hand=%2d %d ", i, reset);
	do{
		pte_t temp = proc_list[frame_table[i]->pid]->page_table[frame_table[i]->vpage];
		//printf("\nvpage: %d, r: %d, m: %d\n",frame_table[i]->vpage, temp.reference, temp.modified );
		if(c1 == -1 && !temp.reference && !temp.modified){
			//printf("c1 found\n");
			c1 = i;
			if(!reset){
				scanned++;
				break;
			}
		}
		else if(c2 == -1 && !temp.reference && temp.modified)
			c2 = i;
		else if(c3 == -1 && temp.reference && !temp.modified)
			c3 = i;
		else if(c4 == -1 && temp.reference && temp.modified)
			c4 = i;

		if(reset)
			proc_list[frame_table[i]->pid]->page_table[frame_table[i]->vpage].reference =0;
		if(i == frame_table.size()-1)
			i = 0;
		else
			i++;
		scanned++;
	}while (i != counter);

	if(i == frame_table.size()-1)
		counter = 0;
	else
		counter = i+1;
	if(c1 != -1){
		c = 0;
		frame_index = c1;
	}
	else if(c2 != -1){
		c = 1;
		frame_index = c2;
	}
	else if(c3 != -1){
		c = 2;
		frame_index = c3;
	}
	else if(c4 != -1){
		c = 3;
		frame_index = c4;
	}
	printf("| %d %d %d\n",c, frame_index, scanned );
	if(frame_index == frame_table.size()-1)
		counter = 0;
	else 
		counter = frame_index+1;
	return frame_table[frame_index];
}

APager::APager(){
	counter = 0;
}

frame_t* APager::select_victim_frame(){
	unsigned min = 0xFFFFFFFF;
	int index;

	int i = counter;
	if(i == 0)
		printf("ASELECT %d-%ld | ",i, frame_table.size()-1 );
	else
		printf("ASELECT %d-%d | ",i, i-1 );

	do{
		frame_table[i]->age >>= 1;
		if(proc_list[frame_table[i]->pid]->page_table[frame_table[i]->vpage].reference)
			frame_table[i]->age |= 0x80000000;
		proc_list[frame_table[i]->pid]->page_table[frame_table[i]->vpage].reference =0;

		printf("%d:%x ", i, frame_table[i]->age);
		if(frame_table[i]->age < min){
			index = i;
			min = frame_table[i]->age;
		}

		if(i == frame_table.size()-1)
			i = 0;
		else
			i++;

	}while(i != counter);

	printf(" | %d\n", index);

	if(index == frame_table.size()-1)
		counter = 0;
	else 
		counter = index+1;
	return frame_table[index];
}

WPager::WPager(){
	counter = 0;
	tau = 50;
}

frame_t* WPager::select_victim_frame(){
	int index;
	unsigned min = 0xFFFFFFFF;
	int scanned = 0;

	int i = counter;
	if(i == 0)
		printf("ASELECT %d-%ld | ",i, frame_table.size()-1 );
	else
		printf("ASELECT %d-%d | ",i, i-1 );

	do{
		scanned++;
		printf("%d(%d %d:%d %d) ", i, proc_list[frame_table[i]->pid]->page_table[frame_table[i]->vpage].reference,
			frame_table[i]->pid, frame_table[i]->vpage, frame_table[i]->age);
		if(proc_list[frame_table[i]->pid]->page_table[frame_table[i]->vpage].reference){
			proc_list[frame_table[i]->pid]->page_table[frame_table[i]->vpage].reference = 0;
			frame_table[i]->age = instr_num;
		}
		int t_age = instr_num - frame_table[i]->age;
		//printf("age: %d ", t_age);
		if(!proc_list[frame_table[i]->pid]->page_table[frame_table[i]->vpage].reference && t_age >= tau){
			index = i;
			printf("STOP(%d) ", scanned);
			break;
		}
		else{
			if(frame_table[i]->age < min){
				index = i;
				min = frame_table[i]->age;
			}
		}
		if(i == frame_table.size()-1)
			i = 0;
		else
			i++;
	}while(i != counter);

	printf("| %d\n", index);

	if(index == frame_table.size()-1)
		counter = 0;
	else 
		counter = index+1;
	return frame_table[index];
}