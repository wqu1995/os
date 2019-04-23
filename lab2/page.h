#ifndef PAGE
#define PAGE
#include "utils.h"
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
}
#endif