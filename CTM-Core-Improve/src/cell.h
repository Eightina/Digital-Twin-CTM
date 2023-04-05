#ifndef _cell_H

#define _cell_H

#include "base.h"
#include "arc.h"
#include "demand.h"
//#include <set>
#include <vector>
#include "setting.h"
//#include <assert.h>
#include "debug.h"
#include "ivector.h"


//extern std::vector<arc> arcs;
//extern float exist_vehicle[MAX_CLOCK][MAX_CELL];
//extern int owner->present_clock;
//extern setting settings;
//extern float diverge_flow[MAX_DIVERGE_CELL][MAX_ADJ_CELL];
//extern int index_diverge_cell[MAX_CELL];
//extern float origin_demand[MAX_CLOCK][MAX_ORIGIN_CELL];
//extern demand temp_origin_demand[MAX_ORIGIN_CELL][MAX_CLOCK];
//extern int temp_origin_demand_size;
//extern bool omega[MAX_CLOCK][MAX_INTERSECTION][MAX_PHASE];
//extern debug *Log;
//extern short index_next_cell[MAX_CELL][MAX_CELL];
//extern std::vector<cell> owner->cells;

//using std::set;
//using std::vector;

class simulation;

namespace Cell{
	enum cell_type{
		origin = 1,destination,diverge,normal,merge
	};
};

using Cell::cell_type;
using Cell::merge;
using Cell::origin;
using Cell::destination;
using Cell::diverge;
using Cell::normal;


class cell{
public:
	
	cell(simulation* cellowner);
	cell(simulation* cellowner, int i, int arc, cell_type t, float len);
	//static int size;

	int			get_id()					const{ return id;			}
	cell_type	get_type()					const{ return type;			}
	int			get_on_arc()				const{ return on_arc;		}
	float		get_max_speed()				const{ return max_speed;	}
	float		get_max_flow()				const{ return max_flow;		}
	float		get_jam_density()			const{ return jam_density;	}
	float		get_delta()					const{ return delta;		}
	float		get_length()				const{ return length;		}
	float		get_in_flow()				const{ return in_flow;		}
	float		get_out_flow()				const{ return out_flow;		}
	float		get_max_vehicle()			const{ return max_vehicle;	}
	int			get_num_demand()			const{ return num_demand;	}
	int			get_temp_origin_demand_id()	const{ return temp_origin_demand_id; }

	void origin_update_flow();
	void set_max_flow(const float& mf);
	void add_at_phase(const int& cell_id, const int& phase_id);
	void diverge_update_flow();
	void merge_update_flow();
	void destination_update_flow();
	void normal_update_flow();

	void set_type(cell_type t){	type = t;	}
	void set_max_speed( const float& ms ){ max_speed = ms; }
	void set_jam_density( const float& jd ){ jam_density = jd; }
	void set_delta( const float& del ){ delta = del; }
	void set_length( const float& len ){ length = len; }
	void set_in_flow( const float& in ){ in_flow = in; }
	void set_on_intersection( const int& id ){ on_intersection = id; }

	inline void set_out_flow( const float& out, int next_cell_id = -1);
	void add_previous_cell( const int& i );
	void add_next_cell( const int& i );
	void add_diverge_coeff( const float& g );

	void add_next_cell( const int& i, const float& g );
	void add_next_cell( const int& i, const float& g, const int& t );

	void add_demand( const int& clock, const float& traffic );

	inline float send_flow(int cn = 0);
	inline float receive_flow();

	/*inline void set_signals_on( int next_cell_id );
	inline void set_signals_off( int next_cell_id );*/
	//float move_vehicle();
	float move_vehicle();
	
	float	in_flow,
			out_flow;
private:
	simulation* owner;

	int			id;
	cell_type	type;
	int			on_arc;
	float		length;
	float		max_speed,
				max_flow,
				jam_density,delta;
	
	ivector<int>		previous_cell, next_cell,
						/*signals,*/turning;
	ivector<float>	diverge_coeff;

	

	float	max_vehicle;

	int temp_origin_demand_id, num_demand;
	/*
	// for ordinary cell, temp_origin_demand_id = -1
	// for source cell, this variable is its demand array index
	// demand array is a member of a simulation object -> demand temp_origin_demand[MAX_ORIGIN_CELL][MAX_CLOCK]
	// here temp_origin_demand_id corresponds to i in temp_origin_demand[i][j]
	// while num_demand corresponds to j in temp_origin_demand[i][j]
	// this means multiple demands can be add to one cell
	*/

	int on_intersection;

	ivector<int> at_phase[MAX_ADJ_CELL];
	/*
	* This 2d array defines the connection relationship
	* between this cell and its successors during different phases.
	* at_phase[i][j]
	* i is the id of its successor, and at_phase[i] saves the id of phases
	* during which traffic flow can go through the connection between this cell and its successor
	*/
};



#endif


