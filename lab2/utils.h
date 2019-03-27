#ifndef UTILS
#define UTILS
#include <iostream>
#include <fstream>

using std::string;
using std::stringstream;
using std::ifstream;

extern int *myrands;
extern int max_prio;

void init_rands(char *s);
int myrandom(int burst);
#endif