#include "cell.h"
#include "setting.h"
#include <cfloat>
#include <climits>
#include "debug.h"

extern arc arcs[];
extern float exist_vehicle[MAX_CLOCK][MAX_CELL];
extern int present_clock;
extern setting settings;
extern cell cells[MAX_CELL];
extern float diverge_flow[MAX_DIVERGE_CELL][MAX_ADJ_CELL];
extern int index_diverge_cell[MAX_CELL];
extern debug *Log;
extern short index_next_cell[MAX_CELL][MAX_CELL];


using Cell::merge;

cell::cell() : id(0), on_arc(0), in_flow(-1.0), out_flow(-1.0) {
	/*if( !signals.empty() ) signals.clear();*/
	if( !previous_cell.empty() ) previous_cell.clear();
	if( !next_cell.empty() ) next_cell.clear();
	if( !turning.empty() ) turning.clear();
	if( !diverge_coeff.empty() ) diverge_coeff.clear();
	temp_origin_demand_id = -1;
	num_demand = 0;
	on_intersection = -1;
	for( int i = 0; i < MAX_ADJ_CELL; ++i )
		if( !at_phase[i].empty() ) at_phase[i].clear();
}

cell::cell( int i,int arc,cell_type t,float len ):
	id(i),type(t),on_arc(arc),length(len){
		Assert(size<MAX_CELL);
		char str[256];
		size++;
		sprintf( str,"Create Cell#%03d successfully",size );
		Log->process( str );
		max_speed = arcs[arc].get_max_speed();
		max_flow = arcs[arc].get_max_flow()*settings.clock_tick;
		jam_density = arcs[arc].get_jam_density();
		delta = arcs[arc].get_delta();
		/*if( !signals.empty() ) signals.clear();*/
		if( !previous_cell.empty() ) previous_cell.clear();
		if( !next_cell.empty() ) next_cell.clear();
		if( !turning.empty() ) turning.clear();
		if( !diverge_coeff.empty() ) diverge_coeff.clear();
		max_vehicle = length*jam_density;
		temp_origin_demand_id = -1;
		num_demand = 0;
		on_intersection = -1;
		for( int i = 0; i < MAX_ADJ_CELL; ++i )
			if( !at_phase[i].empty() ) at_phase[i].clear();
	}



void cell::add_demand( const int& clock,const float& traffic ){
	char str[256];
	sprintf( str,"Origin Cell#%03d Added Demand(Start Time:%03d, Traffic:%5.2lf)",id,clock,traffic );
	Log->process( str );
	if( type != origin ) type = origin;
	if( temp_origin_demand_id < 0 ) temp_origin_demand_id = temp_origin_demand_size++;
	temp_origin_demand[temp_origin_demand_id][num_demand++] = demand( clock,traffic );
}

void cell::add_next_cell( const int& i ){
	if( i < 0 ){
		Log->throws("In cell::add_next_cell( int i ) : the i out range." );
		exit(1);
	}
	for( int ii = 0; ii < next_cell.top; ++ii ) 
		if( next_cell.a[ii] == i ) return;
	next_cell.push_back(i);
	index_next_cell[id][i] = next_cell.top - 1;
	if( get_type() == normal && next_cell.size() > 1 ){
		set_type( diverge );
	}
	/*if( signals.size() < next_cell.size() ) 
		signals.push_back(1);*/
}


void cell::add_previous_cell( const int& i ){
	if( i < 0 ){
		Log->throws("In cell::add_previous_cell( int i ) : the i out range." );
		exit(1);
	}
	for( int ii = 0; ii < (int)previous_cell.size(); ++ii ) 
		if( previous_cell[ii] == i ) return;
	previous_cell.push_back(i);

	if( get_type() == normal && previous_cell.size() > 1 ){
		set_type( Cell::merge );
	}
}


void cell::add_diverge_coeff( const float& g ){
	diverge_coeff.push_back(g);
	if( get_type() == normal && next_cell.size() > 1 ){
		set_type( diverge );
	}
	/*if( signals.size() < diverge_coeff.size() )
		signals.push_back(1);*/
}

void cell::add_next_cell( const int& i,const float& g ){
	next_cell.push_back(i);
	index_next_cell[id][i] = next_cell.top - 1;
	if( get_type() == normal && next_cell.size() > 1 ){
		set_type( diverge );
	}
	diverge_coeff.push_back(g);
	/*if( signals.size() < next_cell.size() ) 
		signals.push_back(1);*/
}

void cell::add_next_cell( const int& i,const float& g,const int& t ){
	if( i < 0 ){
		Log->throws("In cell::add_previous_cell( int i,float g,int t ) : the i out range." );
		exit(1);
	}
	next_cell.push_back(i);
	index_next_cell[id][i] = next_cell.top - 1;
	diverge_coeff.push_back(g);
	if( get_type() == normal && next_cell.size() > 1 ){
		set_type( diverge );
	}
	turning.push_back(t);
	/*if( signals.size() < next_cell.size() ) 
		signals.push_back(1);*/
}


int cell::size = 0;
