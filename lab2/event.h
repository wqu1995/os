#ifndef EVENT
#define EVENT
#include "types.h"
#include "process.h"

using namespace PS;

namespace EVT{
	class event{
	public:
		trans_state_t transition;
		Process* evt_proc;
		int evt_time;
		event *next;
		event(trans_state_t ts, Process* proc, int time);
	};
}
using namespace EVT;
namespace EVTQ{
	class Event_Q{
		event* e;
	public:
		Event_Q();
		event* get_event();
		void put_event(trans_state_t ts, Process* proc, int time);
		int get_next_event_time();
		void print_event();
		void print_eventX();
		int get_next_event_pid();
		event* remove_event(Process* proc);
		event* get_next_proc_event(Process* proc);
	};
}


#endif