
#include "arc.h"
#include "cell.h"
#include "node.h"
#include "debug.h"
#include <math.h>
//#include <iostream>

//extern std::vector<node> owner->nodes;
//extern setting owner->settings;
//extern std::vector<cell> owner->cells;
////extern int gg[];
//extern debug* owner->Log;
////#include "arc.h"

arc::arc(): id(0), up_node(0), down_node(0), max_speed(0.0),
			max_flow(0.0), jam_density(0.0), delta(0.0) {
			//std::cout<<gg[0]<<std::endl;
}

arc::arc(int i, int un, int dn, float ms,
			float mf, float jd, float del,
			float l, float cl):
		id(i), up_node(un), down_node(dn), max_speed(ms),
			max_flow(mf), jam_density(jd), delta(del),
			length(l), cell_length(cl) {
	//if ( delta < 0.0 ) {
	//	delta = max_flow * owner->settings.clock_tick /
	//		(jam_density * max_speed - owner->settings.clock_tick * max_flow);
	//}
	//size++;
	//owner->nodes[un].set_arc(i);
	//owner->nodes[dn].set_arc(i);

	num_cell = (int)floor( length/cell_length );

	//create_cell();
}

//void arc::create_cell() {
//	if (owner->cells.empty()) owner->cells.push_back(cell(owner));
//	Assert(owner->cells.size() <= MAX_CELL + 1);
//	float LL = length;
//	first_cell = owner->cells.size();
//	last_cell = first_cell + num_cell - 1;
//	sprintf(owner->Log->get_str(),"Arc#%03d: First Cell #%03d, Last Cell #%03d",id,first_cell,last_cell );
//	owner->Log->process(owner->Log->get_str());
//	int tmp_cell_size;
//	while ( LL >= 2*cell_length ){
//		LL -= cell_length;
//		tmp_cell_size = owner->cells.size();
//		//owner->cells[tmp_cell_size] = cell( tmp_cell_size,id,normal,cell_length );
//		//
//		owner->cells.emplace_back(owner, tmp_cell_size, id, normal, cell_length);
//		char str[256];
//		sprintf(str, "Create Cell#%03d successfully", tmp_cell_size);
//		owner->Log->process(str);
//	}
//
//	if ( owner->nodes[down_node].get_type() == 2 ){
//		tmp_cell_size = owner->cells.size();
//		owner->cells.emplace_back(owner, tmp_cell_size, id, destination, LL);
//		char str[256];
//		sprintf(str, "Create Cell#%03d successfully", tmp_cell_size);
//		owner->Log->process(str);
//	} else {
//		tmp_cell_size = owner->cells.size();
//		owner->cells.emplace_back(owner, tmp_cell_size, id, normal, LL );
//		char str[256];
//		sprintf(str, "Create Cell#%03d successfully", tmp_cell_size);
//		owner->Log->process(str);
//	}
//
//	if ( owner->nodes[up_node].get_type() == 1 ){
//		owner->cells[first_cell].set_type( origin );
//	}
//
//	for( int i = first_cell + 1; i <= last_cell; ++i ){
//		owner->cells[i].add_previous_cell( i-1 );
//	}
//	for( int i = first_cell; i < last_cell; ++i ){
//		owner->cells[i].add_next_cell( i+1 );
//	}
//}

//int arc::size = 0;
