#ifndef _incident_H

#define _incident_H

#include "setting.h"
#include "cell.h"
#include "arc.h"
#include "base.h"

extern std::vector<cell> cells;
extern arc	arcs[MAX_ARC];
extern int present_clock;
extern setting settings;

class incident{
public:
	static int size;
	incident();
	incident( int aa,float dist,int st,int et,float mf );
	inline void occur();
private:
	int id;
	int at_arc;
	float distance;
	int at_cell;
	int start_time,end_time;
	float max_flow;
	int start_clock,end_clock;
};


inline void incident::occur(){
	if( present_clock == start_clock )
		cells[at_cell].set_max_flow(max_flow);
	if( present_clock == end_clock )
		cells[at_cell].set_max_flow( arcs[at_arc].get_max_flow() );
}

#endif

