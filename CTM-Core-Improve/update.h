#ifndef _update_H

#define _update_H

#include <ctime>
#include <cstdio>

#include "base.h"
#include "setting.h"
#include "arc.h"
#include "cell.h"
#include "demand.h"
#include "intersection.h"
#include "node.h"
#include "coordinate.h"
#include "incident.h"
#include "debug.h"
#include "ivector.h"

//extern int			present_clock;
//extern setting		settings;
//extern std::vector<node> nodes;
//extern std::vector<arc> arcs;
//extern std::vector<cell> cells;
//extern int origin_set[MAX_ORIGIN_CELL],normal_set[MAX_NORMAL_CELL],
//	diverge_set[MAX_DIVERGE_CELL],merge_set[MAX_MERGE_CELL],destination_set[MAX_DESTINATION_CELL];
//
//extern int origin_size,normal_size,diverge_size,merge_size,destination_size;
//extern float		exist_vehicle[MAX_CLOCK][MAX_CELL];
//extern float diverge_flow[MAX_DIVERGE_CELL][MAX_ADJ_CELL];
//extern int index_diverge_cell[MAX_CELL];
//extern std::vector<intersection> intersections;
//extern incident		incidents[MAX_INCIDENT];
//extern bool			omega[MAX_CLOCK][MAX_INTERSECTION][MAX_PHASE];
//extern debug		*Log;
//extern float		delay_record[MAX_CLOCK];


void modify_control(const int& I, const int& t0, const int& t1, const int& P){
	for( int i = t0; i <= t1; i++ ){
		omega[i][I][P] = true;
		for( int j = 0; j < intersections[I].get_num_phases(); ++j ){
			if( j == P ) continue;
			omega[i][I][j] = false;
		}
	}
}

void set_control_off( const int& I,const int& st,const int& et )
{
	for( int it = st; it <= et; ++it )
	{
		for( int ip = 0; ip < intersections[I].get_num_phases(); ++ip )
			omega[it][I][ip] = false;
	}
}

inline void set_on_all(){
	memset(omega,true,sizeof(omega));
}

void update_flow( ) {
	//cell *cl = cells;
	std::vector<cell>& cl = cells;

	for( int i = 0; i < origin_size; ++i )
		cl[origin_set[i]].origin_update_flow();

	for( int i = 0; i < diverge_size; ++i )
		cl[diverge_set[i]].diverge_update_flow();

	for( int i = 0; i < merge_size; ++i )
		cl[merge_set[i]].merge_update_flow();

	for( int i = 0; i < normal_size; ++i )
		cl[normal_set[i]].normal_update_flow();

	for( int i = 0; i < destination_size; ++i )
		cl[destination_set[i]].destination_update_flow();

}

void update_event(){
	for( int i = 1; i <= incident::size; ++i ){
		incidents[i].occur();
	}
}

float update_occupation() {
	float delay = 0.0;
	//for (int i = 1; i <= cell::size; ++i) {
	for (int i = 1; i < cells.size(); ++i) 
		if (cells[i].get_type() != destination && cells[i].get_type() != origin) 
			delay += cells[i].move_vehicle();
	return delay;
}

//float simulate( float* vehicle,float *result,int st,int et ){
//	float delay = 0.0;
//
//	//Start simulation.
//	for( int it = st+1; it <= et; ++it ){
//		present_clock = it;
//		update_event();
//		update_flow();
//		delay += update_occupation();
//	}
//
//	return delay*settings.clock_tick;
//}

//float simulate( int st,int et ){
//	float delay = 0.0;
//
//	//Start simulation.
//	for( int it = st; it <= et; ++it ){
//		present_clock = it;
//		update_event();
//		update_flow();
//		delay += update_occupation();
//	}
//
//	return delay*settings.clock_tick;
//}

//float simulate( float* vehicle,float *result,int st,int et ){
//	float delay = 0.0;
//	float temp = 0;
//
//	//Start simulation.
//	for( int it = st+1; it <= et; ++it ){
//
//		present_clock = it;
//		for( int i = 1; i <= incident::size; ++i ){
//			incidents[i].occur();
//		}
//		update_flow();
//		for( int i = 1; i <= cell::size; ++i ){
//			temp = exist_vehicle[it-1][i] - cells[i].out_flow;
//			exist_vehicle[it][i] = temp + cells[i].in_flow;
//			if(cells[i].get_type()!=destination&&cells[i].get_type()!=origin)
//				delay += temp;
//		}
//	}
//
//	return delay*settings.clock_tick;
//}

float simulate( int st, int et ){
	float delay = 0.0f;
	float temp = 0.0f;
	float pre_delay = 0.0f;

	//Start simulation.
	for( int it = st; it <= et; ++it ){

		present_clock = it;
		for( int i = 1; i <= incident::size; ++i ){
			incidents[i].occur();
		}
		update_flow();
		//for( int i = 1; i <= cell::size; ++i ){
		for (int i = 1; i < cells.size(); ++i) {
			temp = exist_vehicle[it-1][i] - cells[i].out_flow;
			exist_vehicle[it][i] = temp + cells[i].in_flow;
			if(cells[i].get_type()!=destination&&cells[i].get_type()!=origin)
				delay += temp;
		}
		delay_record[it] = delay - pre_delay;
		pre_delay = delay;
	}

	return delay*settings.clock_tick;
}

#endif

