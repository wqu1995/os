#ifndef PAGE
#define PAGE
#include "utils.h"
#include "process.h"
#include <queue>
using std::deque;

namespace PG{
	class Pager{
	public:
		virtual frame_t* select_victim_frame()=0;
	};

	class FPager: public Pager{
		int counter;
	public:
		FPager();
		frame_t* select_victim_frame();
	};

	class RPager: public Pager{
	public:
		RPager();
		frame_t* select_victim_frame();
	};

	class CPager: public Pager{
		int counter;
	public:
		CPager();
		frame_t* select_victim_frame();
	};

	class EPager: public Pager{
		int counter;
		int ins;
	public:
		EPager();
		frame_t* select_victim_frame();
	};

	class APager: public Pager{
		int counter;
	public:
		APager();
		frame_t* select_victim_frame();
	};

	class WPager: public Pager{
		int counter;
		int tau;
	public:
		WPager();
		frame_t* select_victim_frame();
	};
}
#endif