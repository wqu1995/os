#ifndef SCHEDULER
#define SCHEDULER
#include <deque>
#include <queue>
#include "process.h"
#include <string>
#include "event.h"
using std::to_string;
using std::deque;
using std::priority_queue;
using namespace PS;
using namespace EVTQ;


namespace SCH{

	/********************
	queue implemented as a linked list*/
	class sorted_q{
	public:
		sorted_q(Process* proc);
		Process *proc;
		sorted_q *next;
	};

	//base class
	class Scheduler{
	public:
		int quantum;
		string name;
		virtual void add_process(Process *proc, Process *current_running_proc, Event_Q *evt_q)=0;
		virtual Process* get_next_process()=0;
		virtual void print_q()=0;
	};

	//first come first serve
	class FScheduler: public Scheduler{
	protected:
		deque<Process*> run_q;
	public:
		FScheduler();
		virtual void add_process(Process *proc, Process *current_running_proc, Event_Q *evt_q);
		Process* get_next_process();
		void print_q();
	};

	//last come first serve
	class LScheduler: public FScheduler{
		//deque<Process*> run_q;
	public:
		LScheduler();
		void add_process(Process *proc, Process *current_running_proc, Event_Q *evt_q);
		//Process* get_next_process();
	//	void print_q();
	};

	//shortest remaining time first 
	class SRScheduler: public Scheduler{
		sorted_q* run_q;
		int count;
	public:
		SRScheduler();
		void add_process(Process *proc, Process *current_running_proc, Event_Q *evt_q);
		Process* get_next_process();
		void print_q();
	};

	//round robin
	class RRScheduler: public FScheduler{
		//deque<Process*> run_q;
	public:
		RRScheduler(int q);
		void add_process(Process *proc, Process *current_running_proc, Event_Q *evt_q);
		//Process* get_next_process();
		//void print_q();
	};

	//Priority Scheduler
	class PRScheduler: public Scheduler{
	protected:
		sorted_q* *active_q;
		sorted_q* *expired_q;
		int maxprio;
	public:
		PRScheduler();
		PRScheduler(int q, int maxp);
		virtual void add_process(Process *proc, Process *current_running_proc, Event_Q *evt_q);
		Process* get_next_process();
		void print_q();
	};

	//preemptive Priority Scheduler
	class PPRScheduler: public PRScheduler{
	public:
		PPRScheduler(int q, int maxp);
		void add_process(Process *proc, Process *current_running_proc, Event_Q *evt_q);
		//Process* get_next_process();
		//void print_q();
	};
}
#endif