#include "cell.h"
#include "setting.h"
#include <cfloat>
#include <climits>
#include "debug.h"
#include "simulation.h"

using Cell::merge;

cell::cell(simulation* cellowner) : owner(cellowner), id(0), on_arc(0), in_flow(-1.0), out_flow(-1.0) {
	/*if( !signals.empty() ) signals.clear();*/
	if (!previous_cell.empty()) previous_cell.clear();
	if (!next_cell.empty()) next_cell.clear();
	if (!turning.empty()) turning.clear();
	if (!diverge_coeff.empty()) diverge_coeff.clear();
	temp_origin_demand_id = -1;
	num_demand = 0;
	on_intersection = -1;
	for (int i = 0; i < MAX_ADJ_CELL; ++i)
		if (!at_phase[i].empty()) at_phase[i].clear();
}

cell::cell(simulation* cellowner, int i, int arc, cell_type t, float len) :
	owner(cellowner), id(i), type(t), on_arc(arc), length(len) {
		//Assert(cells.size()<MAX_CELL);
		
		max_speed = owner->arcs[arc].get_max_speed();
		max_flow = owner->arcs[arc].get_max_flow()*(owner->settings.clock_tick);
		jam_density = owner->arcs[arc].get_jam_density();
		delta = owner->arcs[arc].get_delta();
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

void cell::origin_update_flow() { in_flow = owner->origin_demand[owner->present_clock][temp_origin_demand_id]; }

void cell::set_max_flow(const float& mf) { max_flow = mf * owner->settings.clock_tick; }

/* 
* This function defines the connection relationship
* between this cell and one of its successor during different phases.
* This relationship is saved by array cell.at_phase 
*/
void cell::add_at_phase(const int& cell_id, const int& phase_id) {
	int i = 0;
	while (next_cell[i] != cell_id) ++i;
	at_phase[i].push_back(phase_id);
}

// S(t)
float cell::send_flow(int cn) {
	if (type != diverge)
		return Min(max_flow, owner->exist_vehicle[owner->present_clock - 1][id]);
	
	int i = owner->index_next_cell[id][cn];

	if (on_intersection < 0)
		return Min(max_flow * diverge_coeff[i], owner->diverge_flow[owner->index_diverge_cell[id]][i]);

	else {
		if (at_phase[i].empty())
			return Min(max_flow * diverge_coeff[i], owner->diverge_flow[owner->index_diverge_cell[id]][i]);
		else {
			for (int j = 0; j < at_phase[i].top; ++j)
				if (owner->omega[owner->present_clock][on_intersection][at_phase[i][j]])
					return Min(max_flow * diverge_coeff[i], owner->diverge_flow[owner->index_diverge_cell[id]][i]);
		}
	}
	return 0.0;
}

// R(t)
float cell::receive_flow() {
	return Min(max_flow, delta * (max_vehicle - owner->exist_vehicle[owner->present_clock - 1][id]));
}


void cell::set_out_flow(const float& out, int next_cell_id) {
	if (type == diverge) {
		int i = owner->index_next_cell[id][next_cell_id];
		
		// calculate the flow between this diverge cell and one of its successors, which has id next_cell_id
		owner->diverge_flow[owner->index_diverge_cell[id]][i] -= out;
		owner->diverge_flow[owner->index_diverge_cell[id]][i] += in_flow * diverge_coeff[i];
		out_flow += out;
	}
	else {
		out_flow = out;
	}
}

// y_ij(t)
void cell::diverge_update_flow() {
	//In flow.
	in_flow = Min(owner->cells[previous_cell[0]].send_flow(id), receive_flow());
	owner->cells[previous_cell[0]].set_out_flow(in_flow, id);
	out_flow = 0.0;
}

void cell::merge_update_flow() {

	float sum_send_flow = 0.0, pre_send_flow, this_receive_flow, part_in_flow;

	for (int i = 0; i < (int)previous_cell.size(); ++i) {
		//Assert( previous_cell[i]>=0 );
		sum_send_flow += owner->cells[previous_cell[i]].send_flow(id);
	}
	in_flow = 0.0;
	for (int i = 0; i < (int)previous_cell.size(); ++i) {
		pre_send_flow = owner->cells[previous_cell[i]].send_flow(id);
		this_receive_flow = pre_send_flow / sum_send_flow * receive_flow();
		part_in_flow = Min(pre_send_flow, this_receive_flow);
		in_flow += part_in_flow;
		//Set flow for previous cell.
		owner->cells[previous_cell[i]].set_out_flow(part_in_flow, id);
	}
}

void cell::destination_update_flow() {
	in_flow = owner->cells[previous_cell[0]].send_flow();
	out_flow = send_flow();
	owner->cells[previous_cell[0]].set_out_flow(in_flow, id);
}

void cell::normal_update_flow() {
	//Assert( previous_cell[0]>=0 );
	in_flow = Min(owner->cells[previous_cell[0]].send_flow(id), receive_flow());
	owner->cells[previous_cell[0]].set_out_flow(in_flow, id);
}

//inline void cell::set_signals_on( const int& next_cell_id ){
//	for( int i = 0; i < next_cell.size(); ++i ){
//		if( next_cell[i] == next_cell_id ){
//			signals[i] = 1; break;
//		}
//	}
//}

//inline void cell::set_signals_off( int next_cell_id ){
//	for( int i = 0; i < next_cell.size(); ++i ){
//		if( next_cell[i] == next_cell_id ){
//			signals[i] = 0; break;
//		}
//	}
//}

float cell::move_vehicle() {

	float delay = owner->exist_vehicle[owner->present_clock - 1][id] - out_flow;
	owner->exist_vehicle[owner->present_clock][id] = delay + in_flow;
	//Log flows and states.
	//sprintf(Log->get_str(),"Cell#%04d: In Flow(%7.2lf ), Out Flow(%7.2lf ), Exist vehicles (%7.2lf ), Delay (%7.2lf )",
	//	id,get_in_flow(),get_out_flow(), exist_vehicle[owner->present_clock][id], delay );
	//Log->process(Log->get_str());
	//if(get_in_flow() - max_flow > settings.epsilon )
	//	Log->warning("Exceed maximum flow! ^^^^^^");
	//if(get_out_flow() - max_flow > settings.epsilon )
	//	Log->warning("Exceed maximum flow!\t\t   ^^^^^^");
	//if(exist_vehicle[owner->present_clock][id] - max_vehicle > settings.epsilon)
	//	Log->warning("Exceed maximum vehicle!\t\t\t\t\t    ^^^^^^");

	return delay;
}

// add demand to simulation->temp_origin_demand
// demand temp_origin_demand[MAX_ORIGIN_CELL][MAX_CLOCK]	
void cell::add_demand( const int& clock,const float& traffic ){ 
	char str[256];
	sprintf( str,"Origin Cell#%03d Added Demand(Start Time:%03d, Traffic:%5.2lf)",id,clock,traffic );
	//owner->Log->process( str, owner->present_clock);
	if( type != origin ) type = origin;
	if( temp_origin_demand_id < 0 ) temp_origin_demand_id = owner->temp_origin_demand_size++;
	owner->temp_origin_demand[temp_origin_demand_id][num_demand++] = demand( clock,traffic );
}

void cell::add_next_cell( const int& i ){
	if( i < 0 ){
		//owner->Log->throws("In cell::add_next_cell( int i ) : the i out range.", owner->present_clock);
		exit(1);
	}
	for( int ii = 0; ii < next_cell.top; ++ii ) 
		if( next_cell.a[ii] == i ) return;
	next_cell.push_back(i);
	owner->index_next_cell[id][i] = next_cell.top - 1;
	if( get_type() == normal && next_cell.size() > 1 ){
		set_type( diverge );
	}
	/*if( signals.size() < next_cell.size() ) 
		signals.push_back(1);*/
}


void cell::add_previous_cell( const int& i ){
	if( i < 0 ){
		//owner->Log->throws("In cell::add_previous_cell( int i ) : the i out range.", owner->present_clock);
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
	owner->index_next_cell[id][i] = next_cell.top - 1;
	if( get_type() == normal && next_cell.size() > 1 ){
		set_type( diverge );
	}
	diverge_coeff.push_back(g);
	/*if( signals.size() < next_cell.size() ) 
		signals.push_back(1);*/
}

void cell::add_next_cell( const int& i,const float& g,const int& t ){
	if( i < 0 ){
		//owner->Log->throws("In cell::add_previous_cell( int i,float g,int t ) : the i out range.", owner->present_clock);
		exit(1);
	}
	next_cell.push_back(i);
	owner->index_next_cell[id][i] = next_cell.top - 1;
	diverge_coeff.push_back(g);
	if( get_type() == normal && next_cell.size() > 1 ){
		set_type( diverge );
	}
	turning.push_back(t);
	/*if( signals.size() < next_cell.size() ) 
		signals.push_back(1);*/
}


//int cell::size = 0;
