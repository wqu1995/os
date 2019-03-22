#ifndef SCHEDULER
#define SCHEDULER
#include <deque>
#include "process.h"
using std::deque;
using namespace PS;

namespace SCH{
	class Scheduler{
	public:
		string name;
		virtual void add_process(Process *proc)=0;
		virtual Process* get_next_process()=0;
		virtual void print_q()=0;
	};

	class FScheduler: public Scheduler{
		deque<Process*> run_q;
	public:
		FScheduler();
		void add_process(Process *proc);
		Process* get_next_process();
		void print_q();
	};
}
#endif