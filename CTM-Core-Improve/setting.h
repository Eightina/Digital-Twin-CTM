
#ifndef _setting_H

#define _setting_H

#include <math.h>

class setting{
public:
	
	setting();
	setting( int, int, int, float, int, int );

	// start_time or end_time [ 0,24*60*60 ]
	int start_time,end_time;
	// clock_tick [ 1,10 ]
	int clock_tick;
	// epsilon [ 0.0001,0.01 ]
	float epsilon;
	// initial cell occupation [ 0,100 ]   in percentage
	int initial_occupation;
	// initial control strategy { 0,1 
	int initial_control;
	
	float cell_length_factor;

	int max_ticks;
	int get_max_ticks() { return max_ticks; }

	// the yellow time
	int yellow_time;
	// the yellow clock ticks
	int yellow_ticks;

};

#endif
