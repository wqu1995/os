#ifndef PROCESS
#define PROCESS
#include "utils.h"
#include "types.h"
#include <vector>

using std::vector;

namespace PS{
	class Process{
	public:
		int pid;
		int arr_time;
		int total_cpu;
		int cpu_burst;
		int current_cpu_time;
		int io_burst;
		int state_ts;
		process_state_t current_state;
		int s_prio;
		int d_prio;
		int io_wait;
		int cpu_wait;
		int cpu_rem;
		int io_until;
		Process(int i, int at, int tc, int cb, int ib);
	};
}

#endif