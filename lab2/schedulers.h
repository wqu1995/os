#ifndef SCHEDULER
#define SCHEDULER
#include <deque>
#include <queue>
#include "process.h"
#include <string>
using std::to_string;
using std::deque;
using std::priority_queue;
using namespace PS;


namespace SCH{
	class sorted_q{
	public:
		sorted_q(Process* proc);
		Process *proc;
		sorted_q *next;
	};

	class Scheduler{
	public:
		int quantum;
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

	class LScheduler: public Scheduler{
		deque<Process*> run_q;
	public:
		LScheduler();
		void add_process(Process *proc);
		Process* get_next_process();
		void print_q();
	};

	class SRScheduler: public Scheduler{
		sorted_q* run_q;
		int count;
	public:
		SRScheduler();
		void add_process(Process *proc);
		Process* get_next_process();
		void print_q();
	};

	class RRScheduler: public Scheduler{
		deque<Process*> run_q;
	public:
		RRScheduler(int q);
		void add_process(Process *proc);
		Process* get_next_process();
		void print_q();
	};

	class PRScheduler: public Scheduler{
		sorted_q* *active_q;
		sorted_q* *expired_q;
		int maxprio;
	public:
		PRScheduler(int q, int maxp);
		void add_process(Process *proc);
		Process* get_next_process();
		void print_q();
	};
}
#endif