#include "incident.h"
#include "setting.h"
#include "cell.h"
#include "arc.h"
#include "base.h"

//extern std::vector<cell> cells;
//extern std::vector<arc> owner->arcs;
//extern int present_clock;
//extern setting owner->settings;

incident::incident():
	id(0),distance(0.0),start_time(0),end_time(0),max_flow(0.0){}

incident::incident(int aa,float dist,int st,int et,float mf ):
	at_arc(aa),distance(dist),start_time(st),end_time(et),max_flow(mf){

	float len = owner->arcs[at_arc].get_cell_length();
	at_cell = (int)ceil(distance / len) + owner->arcs[at_arc].get_first_cell() - 1;
	start_clock = (int)floor((start_time - owner->settings.start_time) * 1.0 / owner->settings.clock_tick) + 1;
	end_clock = (int)floor((end_time - owner->settings.start_time) * 1.0 / owner->settings.clock_tick) + 1;
	id = size + 1;
	size++;

}


int incident::size = 0;
