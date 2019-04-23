#include "utils.h"
#include "process.h"
#include "page.h"
#include <iostream>
#include <vector>
#include <queue>
using namespace PARSER;
using namespace PS;
using namespace PG;
using std::cout;
using std::stoi;
using std::vector;
using std::deque;

Parser *parser;
deque<int> frame_free_list;
Pager *pager;
int proc_num;
int frame_number;
char pager_type = 'w';
unsigned long c_switch = 0;
unsigned long c_exit = 0;
unsigned long rw_ins = 0;
unsigned long long cost = 0;


void exit_proc(Process* proc){
	for(int i = 0; i< PAGE_NUM; i++){
		if(proc->page_table[i].vaild){
			frame_t* temp = frame_table[proc->page_table[i].f_index];
			printf(" UNMAP %d:%d\n", temp->pid, temp->vpage);
			pstats[temp->pid].umaps++;
			if(proc->page_table[i].filemapped && proc->page_table[i].modified){
				printf(" FOUT \n");
				pstats[temp->pid].fouts++;
			}
			temp->vaild = 0;
			temp->pid = -1;
			temp->vpage = -1;
			temp->age = 0;
			frame_free_list.push_back(proc->page_table[i].f_index);


		}
		proc->page_table[i].pagedout = 0;
		proc->page_table[i].vaild = 0;
	}
}

void print_page_table(Process * proc){
	printf("PT[%d]: ", proc->pid);
	for(int i = 0; i< 64; i++){
		if(proc->page_table[i].vaild){
			printf("%d:", i);
			if(proc->page_table[i].reference)
				printf("R");
			else
				printf("-");
			if(proc->page_table[i].modified)
				printf("M");
			else
				printf("-");
			if(proc->page_table[i].pagedout)
				printf("S ");
			else
				printf("- ");
		}
		else{
			if(proc->page_table[i].pagedout)
				printf("# ");
			else
				printf("* ");
		}
	}
	printf("\n");
}

void print_all_page_tables(){
	for(int i = 0; i< proc_num; i++)
		print_page_table(proc_list[i]);
}

void print_frame_table(){
	printf("FT: ");
	for(frame_t* frame : frame_table){
		if(frame->vaild)
			printf("%d:%d ", frame->pid, frame->vpage);
		else
			printf("* ");
	}
	printf("\n");
}

void print_proc_stats(){
	for(int i =0; i< proc_num; i++){
		proc_stats temp = pstats[i];
		printf("PROC[%d]: U=%lu M=%lu I=%lu O=%lu FI=%lu FO=%lu Z=%lu SV=%lu SP=%lu\n", 
			i, temp.umaps, temp.maps, temp.ins, temp.outs,
			temp.fins, temp.fouts, temp.zeros,
			temp.segv, temp.segprot);
		//unmap
		cost+=400*temp.umaps;
		//maps
		cost+=400*temp.maps;
		//in
		cost+=3000*temp.ins;
		//out
		cost+=3000*temp.outs;
		//fin
		cost+=2500*temp.fins;
		//fout
		cost+=2500*temp.fouts;
		//zero
		cost+=150*temp.zeros;
		//segv
		cost+=240*temp.segv;
		//segprot
		cost+=300*temp.segprot;
	}
	//rw_access
	cost+=rw_ins;
	//c_switch
	cost+=121*c_switch;
	//exit
	cost+=175*c_exit;

	printf("TOTALCOST %lu %lu %lu %llu\n", instr_num, c_switch, c_exit, cost);

}

void init_frames(){
	for(int i = 0; i< frame_number; i++){
		frame_t *temp = (frame_t*)calloc(1, sizeof(frame_t));
		temp->vaild = 0;
		temp->fid = i;
		temp->pid = -1;
		temp->vpage = -1;
		temp->age = 0;
		frame_table.push_back(temp);
		frame_free_list.push_back(i);
	}
}

frame_t* get_frame_from_list(){
	if(frame_free_list.size() == 0)
		return NULL;
	int i = frame_free_list.front();
	frame_free_list.pop_front();
	frame_table[i]->vaild = 1;
	return frame_table[i];
}

void init_process(){
	string s = parser->get_next_line();
	proc_num = stoi(s);
	proc_list = (Process**)malloc(proc_num*sizeof(Process*));
	pstats = (proc_stats*)calloc(proc_num, sizeof(proc_stats));
	for(int i = 0; i< proc_num; i++){
		int vma_n = stoi(parser->get_next_line());
		Process *temp = new Process(i, vma_n);
		for(int j = 0; j< vma_n; j++){
			int s, e, wp, mmap;
			sscanf(parser->get_next_line().c_str(), "%d%d%d%d",&s,&e,&wp,&mmap);
			temp->set_vma(j, s,e,wp,mmap);
		}
		proc_list[i] = temp;
	}
}


int vaildate_vpage(Process* cur_proc, int vpage){
	for(int i = 0; i<cur_proc->segment_num; i++){
		if(vpage >=cur_proc->segments[i].start && vpage <=cur_proc->segments[i].end)
			return i;
	}
	return -1;
}

frame_t* get_frame(){
	frame_t *frame = get_frame_from_list();
	if(frame == NULL){
		frame = pager->select_victim_frame();
		printf(" UNMAP %d:%d\n",frame->pid, frame->vpage);
		pstats[frame->pid].umaps++;

		proc_list[frame->pid]->page_table[frame->vpage].vaild = 0;
		if(proc_list[frame->pid]->page_table[frame->vpage].modified){
			if(proc_list[frame->pid]->page_table[frame->vpage].filemapped){
				printf(" FOUT\n");
				pstats[frame->pid].fouts++;
			}
			else{
				printf(" OUT\n");
				pstats[frame->pid].outs++;
				proc_list[frame->pid]->page_table[frame->vpage].pagedout = 1;
			}
			proc_list[frame->pid]->page_table[frame->vpage].modified = 0;

		}

	}

	return frame;
}

int pgfault_handler(Process* cur_proc, int vpage){
	int vma;
	if((vma = vaildate_vpage(cur_proc, vpage)) != -1){
		cur_proc->page_table[vpage].filemapped = cur_proc->segments[vma].filemapped;
		cur_proc->page_table[vpage].w_protect = cur_proc->segments[vma].w_protect;

		frame_t *newframe = get_frame();
		if(cur_proc->page_table[vpage].pagedout){
			printf(" IN\n");
			pstats[cur_proc->pid].ins++;
		}
		else if(cur_proc->page_table[vpage].filemapped){
			printf(" FIN\n");
			pstats[cur_proc->pid].fins++;
		}
		else {
			printf(" ZERO\n");
			pstats[cur_proc->pid].zeros++;
		}
		cur_proc->page_table[vpage].f_index = newframe->fid;
		newframe->pid = cur_proc->pid;
		newframe->vpage = vpage;
		if(pager_type == 'a')
			newframe->age = 0;
		else if(pager_type = 'w')
			newframe->age = instr_num;
		printf(" MAP %d\n", cur_proc->page_table[vpage].f_index);
		pstats[cur_proc->pid].maps++;
		return 1;
	}
	else{
		printf(" SEGV\n");
		pstats[cur_proc->pid].segv++;
		return -1;
	}
}


int get_next_instruction(char* op, int* vpage){
	string s = parser->get_next_line();
	if(!s.empty()){
		sscanf(s.c_str(),"%c %d", op, vpage);
		//instr_num++;
		return 1;
	}
	return 0;
}

void simulation(){
	char operation;
	int vpage;
	Process *cur_proc = NULL;
	while(get_next_instruction(&operation, &vpage)){
		printf("%lu: ==> %c %d\n",instr_num, operation, vpage);
		switch(operation){
			case 'c':{
				c_switch++;
				cur_proc = proc_list[vpage];
				break;
			}
			case 'r':{
				rw_ins++;
				pte_t *pte = &cur_proc->page_table[vpage];
				if(!pte->vaild){
					if(pgfault_handler(cur_proc, vpage)==-1)
						break;
				}
				pte->vaild = 1;
				pte->reference = 1;
				print_all_page_tables();
				print_frame_table();

				break;
			}
			case 'w':{
				rw_ins++;
				pte_t *pte = &cur_proc->page_table[vpage];
				if(!pte->vaild){
					if(pgfault_handler(cur_proc, vpage)==-1)
						break;
					pte->vaild = 1;

				}
				if(pte->w_protect){
					printf(" SEGPROT\n");
					pstats[cur_proc->pid].segprot++;
					pte->reference = 1;
					pte->modified = 0;
				}
				else{
					pte->reference = 1;
					pte->modified = 1;
				}
				print_all_page_tables();
				print_frame_table();

				break;
			}
			case 'e':{
				c_exit++;
				printf("EXIT current process %d\n", cur_proc->pid);
				exit_proc(cur_proc);
				cur_proc = NULL;
				break;
			}
			default:
				printf("illegal operation\n");
				exit(EXIT_FAILURE);
		}
		instr_num++;
	}
}

int main(int argc, char *argv[]){
	parser = new Parser(argv[1]);
	frame_number = atoi(argv[2]);
	pager = new WPager();

	init_rands(argv[3]);
	init_process();
	init_frames();
	simulation();

	print_all_page_tables();
	print_frame_table();
	print_proc_stats();

	//pte_t *test = (pte_t*)calloc(1, sizeof(pte_t));
	//printf("%ld\n", sizeof(pte_t));

/*	string rst = parser->get_next_line();
	while(!rst.empty()){
		cout<<rst<<"\n";
		rst = parser->get_next_line();
	}*/
	return 0;
}