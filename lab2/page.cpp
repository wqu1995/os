#include "page.h"
using namespace PG;

FPager::FPager(){
	counter = 0;
};

frame_t* FPager::select_victim_frame(){
	printf("in here\n");
	if(counter == frame_table.size())
		counter = 0;
	printf("ASELECT %d\n", counter);
	frame_t* temp =frame_table[counter++];
}