#include "utils.h"
#include "process.h"
#include "page.h"
#include <vector>
#include <queue>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

using namespace PARSER;
using namespace PS;
using namespace PG;
using std::stoi;
using std::vector;
using std::deque;

Parser *parser;
deque<int> frame_free_list;
Pager *pager;
int proc_num;
int frame_number = 0;
unsigned long c_switch = 0;
unsigned long c_exit = 0;
unsigned long rw_ins = 0;
unsigned long long cost = 0;


void exit_proc(Process* proc){
	for(int i = 0; i< PAGE_NUM; i++){
		if(proc->page_table[i].vaild){
			frame_t* temp = frame_table[proc->page_table[i].f_index];
			if(ops->O) printf(" UNMAP %d:%d\n", temp->pid, temp->vpage);
			pstats[temp->pid].umaps++;
			if(proc->page_table[i].filemapped && proc->page_table[i].modified){
				if(ops->O) printf(" FOUT \n");
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
		if(ops->O) printf(" UNMAP %d:%d\n",frame->pid, frame->vpage);
		pstats[frame->pid].umaps++;

		proc_list[frame->pid]->page_table[frame->vpage].vaild = 0;
		if(proc_list[frame->pid]->page_table[frame->vpage].modified){
			if(proc_list[frame->pid]->page_table[frame->vpage].filemapped){
				if(ops->O) printf(" FOUT\n");
				pstats[frame->pid].fouts++;
			}
			else{
				if(ops->O) printf(" OUT\n");
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
			if(ops->O) printf(" IN\n");
			pstats[cur_proc->pid].ins++;
		}
		else if(cur_proc->page_table[vpage].filemapped){
			if(ops->O) printf(" FIN\n");
			pstats[cur_proc->pid].fins++;
		}
		else {
			if(ops->O) printf(" ZERO\n");
			pstats[cur_proc->pid].zeros++;
		}
		cur_proc->page_table[vpage].f_index = newframe->fid;
		newframe->pid = cur_proc->pid;
		newframe->vpage = vpage;
		newframe->age = 0;
		newframe->time = instr_num;
		if(ops->O) printf(" MAP %d\n", cur_proc->page_table[vpage].f_index);
		pstats[cur_proc->pid].maps++;
		return 1;
	}
	else{
		if(ops->O) printf(" SEGV\n");
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
		if(ops->O) printf("%lu: ==> %c %d\n",instr_num, operation, vpage);
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
				if(ops->y)	print_all_page_tables();
				if(ops->f)	print_frame_table();

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
					if(ops->O) printf(" SEGPROT\n");
					pstats[cur_proc->pid].segprot++;
					pte->reference = 1;
					pte->modified = 0;
				}
				else{
					pte->reference = 1;
					pte->modified = 1;
				}
				if(ops->y)	print_all_page_tables();
				if(ops->f)	print_frame_table();

				break;
			}
			case 'e':{
				c_exit++;
				if(ops->O) printf("EXIT current process %d\n", cur_proc->pid);
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
	opterr = 0;
	int c;
	char *algo = NULL;
	char *options = NULL;

	while((c=getopt(argc,argv, "a:o:f:"))!=-1){
		switch(c){
			case 'a':
				algo = optarg;
				break;
			case 'o':
				options = optarg;
				break;
			case 'f':
				frame_number = atoi(optarg);
				break;
			case '?':
				printf("usage: ./mmu [-a<algo>] [-o<options>] [–f<num_frames>] inputfile randomfile\n");
				exit(EXIT_FAILURE);
				break;
			default:
				printf("usage: ./mmu [-a<algo>] [-o<options>] [–f<num_frames>] inputfile randomfile\n");
				exit(EXIT_FAILURE);
				break;
		}
	}
	if(!algo || !options || !frame_number || strlen(algo)>1){
		printf("usage: ./mmu [-a<algo>] [-o<options>] [–f<num_frames>] inputfile randomfile\n");
		exit(EXIT_FAILURE);
	}

	char *source;
	char *rfile;

	if(argv[optind] == NULL){
		printf("usage: ./mmu [-a<algo>] [-o<options>] [–f<num_frames>] inputfile randomfile\n");
		exit(EXIT_FAILURE);
	}
	else{
		source = argv[optind];
		optind++;
	}
	if(argv[optind] == NULL){
		printf("usage: ./mmu [-a<algo>] [-o<options>] [–f<num_frames>] inputfile randomfile\n");
		exit(EXIT_FAILURE);
	}
	else{
		rfile = argv[optind];
		optind++;
	}
	switch(algo[0]){
		case 'f':
			pager = new FPager();
			break;
		case 'r':
			pager = new RPager();
			break;
		case 'c':
			pager = new CPager();
			break;
		case 'e':
			pager = new EPager();
			break;
		case 'a':
			pager = new APager();
			break;
		case 'w':
			pager = new WPager();
			break;
		default:
			printf("supported algos: frceaw\n");
			exit(EXIT_FAILURE);
	}
	ops = (option_t*) calloc(1, sizeof(option_t));
	int i = 0;
	while(i != strlen(options)){
		switch(options[i]){
			case 'O':
				ops->O = 1;
				break;
			case 'P':
				ops->P = 1;
				break;
			case 'F':
				ops->F = 1;
				break;
			case 'S':
				ops->S = 1;
				break;
			case 'y':
				ops->y = 1;
				break;
			case 'f':
				ops->f = 1;
				break;
			case 'a':
				ops->a = 1;
				break;
			default:
				printf("supported options: OPFS\n");
				exit(EXIT_FAILURE);
		}
		i++;
	}

	parser = new Parser(source);
	init_rands(rfile);
	init_process();
	init_frames();
	simulation();
	
	if(ops->P)
		print_all_page_tables();
	if(ops->F)
		print_frame_table();
	if(ops->S)
		print_proc_stats();


	return 0;
}