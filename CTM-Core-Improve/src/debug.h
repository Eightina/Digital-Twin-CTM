#ifndef _debug_H

#define _debug_H

#include <iostream>
#include <fstream>
#include <istream>
#include <ostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <cstdlib>

using namespace std;

class debug{
public:
	FILE *err;
	char filename[128];
	char tmpstr[256];
	int warn_times;
	debug();
	debug( char *name );
	~debug();
	void process( char *str, int present_clock);
	void warning( char *str, int present_clock);
	void throws( char *str, int present_clock);
	char *get_str();
};


#endif
