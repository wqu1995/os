#ifndef SCHEDULER
#define SCHEDULER
#include <deque>
#include "process.h"
using std::deque;
using namespace PS;

namespace SCH{
	class Scheduler{
	public:
		virtual void add_process(Process *proc)=0;
		virtual Process* get_next_process()=0;
	};

	class FScheduler: public Scheduler{
		deque<Process*> run_q;
		string name = "FCFS";
	public:
		//FScheduler();
		void add_process(Process *proc);
		Process* get_next_process();
	};
}
#endif