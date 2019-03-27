#include "utils.h"

int rand_count;
int *myrands;
int max_prio = 4;
int ofs = 0;
void init_rands(char *s){
	ifstream input(s);
	if(!input){
		printf("failed to open random file\n");
		exit(EXIT_FAILURE);
	}
	input>>rand_count;
	myrands = (int*)calloc(rand_count,sizeof(int));
	int temp;
	int i = 0;
	while(input>>temp)
		myrands[i++] = temp;
}
int myrandom(int burst){
	int res = 1 + (myrands[ofs] % burst); 
	if(ofs == rand_count-1)
		ofs = 0;
	else
		ofs++;
	return res;
}