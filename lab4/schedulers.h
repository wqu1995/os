#ifndef SCHEDULER
#define SCHEDULER
#include <deque>
#include <vector>
#include "utils.h"
using std::deque;
using std::vector;

namespace SCH{
/*	class sorted_q{
	public:
		sorted_q(request_t* request);
		request_t *io;
		sorted_q *next;
	};*/

	class Scheduler{
	public:
		string name;
		virtual void add_io(request_t* request) = 0;
		virtual request_t* get_next_io() = 0;
		virtual int get_size()=0;
	};

	class FScheduler: public Scheduler{
		deque<request_t*> run_q;
	public:
		FScheduler();
		void add_io(request_t* request);
		request_t* get_next_io();
		int get_size();
	};

	class SScheduler: public Scheduler{
		vector<request_t*> run_q;
	public:
		SScheduler();
		void add_io(request_t* request);
		request_t* get_next_io();
		int get_size();
	};

	class LScheduler: public Scheduler{
		vector<request_t*> run_q;
	public:
		LScheduler();
		void add_io(request_t* request);
		request_t* get_next_io();
		int get_size();
	};

	class CLScheduler: public Scheduler{
		vector<request_t*> run_q;
	public:
		CLScheduler();
		void add_io(request_t* request);
		request_t* get_next_io();
		int get_size();
	};

	class FLScheduler: public Scheduler{
		vector<request_t*> *activate;
		vector<request_t*> *add;
	public:
		FLScheduler();
		void add_io(request_t* request);
		request_t* get_next_io();
		int get_size();
	};
}
#endif