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
		int io_burst;
		int state_ts;
		process_state_t current_state;
		int s_prio;
		int d_prio;
		Process(int i, int at, int tc, int cb, int ib);
	};
}

#endif