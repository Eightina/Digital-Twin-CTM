#ifndef _incident_H

#define _incident_H

#include "setting.h"
#include "cell.h"
#include "arc.h"
#include "base.h"

//extern std::vector<cell> owner->cells;
//extern std::vector<arc> owner->arcs;
//extern int owner->present_clock;
//extern setting settings;

class simulation;

class incident{
public:
	static int size;
	incident();
	incident(int aa,float dist,int st,int et,float mf );
	inline void occur();
private:
	//simulation* owner;
	int id;
	int at_arc;
	float distance;
	int at_cell;
	int start_time,end_time;
	float max_flow;
	int start_clock,end_clock;
};


inline void incident::occur(){
	//if( owner->present_clock == start_clock )
	//	owner->cells[at_cell].set_max_flow(max_flow);
	//if( owner->present_clock == end_clock )
	//	owner->cells[at_cell].set_max_flow( owner->arcs[at_arc].get_max_flow() );
}

#endif

