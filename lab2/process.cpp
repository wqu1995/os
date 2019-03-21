#include "process.h"
using namespace PS;

Process::Process(int i, int at, int tc, int cb, int ib){
	pid = i;
	arr_time = at;
	total_cpu = tc;
	cpu_burst = cb;
	io_burst = ib;
	current_state = STATE_CREATED;
	s_prio = myrandom(max_prio);
	d_prio = s_prio-1;
	state_ts = at;
}

