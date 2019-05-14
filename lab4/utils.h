#ifndef UTILS
#define UTILS
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
using std::vector;
using std::string;
using std::ifstream;

extern int current_track;
extern int direction;

namespace PARSER{
	class Parser{
		ifstream input;
		string line;
	public:
		Parser(string fname);
		~Parser();
		string get_next_line();
	};
}

typedef struct{
	int id;
	int arr_time;
	int track;
	int start_time;
	int fin_time;
}request_t;

#endif