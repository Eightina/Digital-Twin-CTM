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


extern arc arcs[];
extern float exist_vehicle[MAX_CLOCK][MAX_CELL];
extern int present_clock;
extern setting settings;
extern float diverge_flow[MAX_DIVERGE_CELL][MAX_ADJ_CELL];
extern int index_diverge_cell[MAX_CELL];
extern float origin_demand[MAX_CLOCK][MAX_ORIGIN_CELL];
extern demand temp_origin_demand[MAX_ORIGIN_CELL][MAX_CLOCK];
extern int temp_origin_demand_size;
extern bool omega[MAX_CLOCK][MAX_INTERSECTION][MAX_PHASE];
extern debug *Log;
extern short index_next_cell[MAX_CELL][MAX_CELL];

//using std::set;
//using std::vector;
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
	
	cell();
	cell( int i,int arc,cell_type t,float len );
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

	void origin_update_flow(){	in_flow = origin_demand[present_clock][temp_origin_demand_id];    }

	void set_type(cell_type t){	type = t;	}
	void set_max_speed( const float& ms ){ max_speed = ms; }
	void set_max_flow( const float& mf ){ max_flow = mf*settings.clock_tick; }
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

	inline void diverge_update_flow();
	inline void merge_update_flow();
	inline void destination_update_flow();
	inline void normal_update_flow();

	inline float send_flow(int cn = 0);
	inline float receive_flow();

	/*inline void set_signals_on( int next_cell_id );
	inline void set_signals_off( int next_cell_id );*/
	//float move_vehicle();
	inline float move_vehicle();
	
	inline void add_at_phase( const int& cell_id,const int& phase_id );
	float	in_flow,
			out_flow;
private:

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

	int on_intersection;

	ivector<int> at_phase[MAX_ADJ_CELL];

};

extern std::vector<cell> cells;

inline void cell::add_at_phase( const int& cell_id, const int& phase_id ){
	int i = 0;
	while( next_cell[i] != cell_id ) ++i;
	at_phase[i].push_back(phase_id);
}

// S(t)
inline float cell::send_flow(int cn){ 
	if(type != diverge)
		return Min( max_flow, exist_vehicle[present_clock - 1][id] );

	int i = index_next_cell[id][cn];
	//for( i = 0; i < next_cell.size(); i++ )
	//	if( next_cell[i] == cn )	break;

	if ( on_intersection < 0 ) 
		return Min( max_flow*diverge_coeff[i], diverge_flow[index_diverge_cell[id]][i] );

	else {
		if ( at_phase[i].empty() )
			return Min( max_flow*diverge_coeff[i], diverge_flow[index_diverge_cell[id]][i] );
		else {
			for( int j = 0; j < at_phase[i].top; ++j )
				if( omega[present_clock][on_intersection][at_phase[i][j]] )
					return Min( max_flow*diverge_coeff[i], diverge_flow[index_diverge_cell[id]][i] );
		}
	}
	return 0.0;
}

// R(t)
inline float cell::receive_flow() {
	return Min( max_flow, delta * (max_vehicle - exist_vehicle[present_clock - 1][id]) );
}


inline void cell::set_out_flow( const float& out, int next_cell_id){ 
	if ( type == diverge ) {
		int i = index_next_cell[id][next_cell_id];
		/*for( i = 0; i < next_cell.size(); i++ )
			if(next_cell_id == next_cell[i])	break;*/
		diverge_flow[index_diverge_cell[id]][i] -= out;
		diverge_flow[index_diverge_cell[id]][i] += in_flow * diverge_coeff[i];
		out_flow += out;
	}
	else {
		out_flow = out;
	}
}

// y_ij(t)
inline void cell::diverge_update_flow() {
	//In flow.
	in_flow = Min( cells[previous_cell[0]].send_flow(id), receive_flow() );
	cells[previous_cell[0]].set_out_flow(in_flow, id);
	out_flow = 0.0;
}

inline void cell::merge_update_flow() {

	float sum_send_flow = 0.0, pre_send_flow, this_receive_flow, part_in_flow;

	for ( int i = 0; i < (int)previous_cell.size(); ++i ) {
		//Assert( previous_cell[i]>=0 );
		sum_send_flow += cells[previous_cell[i]].send_flow(id);
	}
	in_flow = 0.0;
	for( int i = 0; i < (int)previous_cell.size(); ++i ){
		pre_send_flow = cells[previous_cell[i]].send_flow(id);
		this_receive_flow = pre_send_flow / sum_send_flow * receive_flow();
		part_in_flow = Min( pre_send_flow, this_receive_flow );
		in_flow += part_in_flow;
		//Set flow for previous cell.
		cells[previous_cell[i]].set_out_flow( part_in_flow, id );
	}
}

inline void cell::destination_update_flow(){
	in_flow = cells[previous_cell[0]].send_flow();
	out_flow = send_flow();
	cells[previous_cell[0]].set_out_flow( in_flow, id );
}

inline void cell::normal_update_flow(){
	//Assert( previous_cell[0]>=0 );
	in_flow = Min( cells[previous_cell[0]].send_flow(id), receive_flow() );
	cells[previous_cell[0]].set_out_flow(in_flow,id);
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

inline float cell::move_vehicle(){

	float delay = exist_vehicle[present_clock - 1][id] - out_flow;
	exist_vehicle[present_clock][id] = delay + in_flow;
	//Log flows and states.
	/*sprintf(Log->get_str(),"Cell#%04d: In Flow(%7.2lf ), Out Flow(%7.2lf ), Exist vehicles (%7.2lf ), Delay (%7.2lf )",
		id,get_in_flow(),get_out_flow(), exist_vehicle[present_clock][id], delay );
	Log->process(Log->get_str());
	if(get_in_flow() - max_flow > settings.epsilon )
		Log->warning("Exceed maximum flow! ^^^^^^");
	if(get_out_flow() - max_flow > settings.epsilon )
		Log->warning("Exceed maximum flow!\t\t   ^^^^^^");
	if(exist_vehicle[present_clock][id] - max_vehicle > settings.epsilon)
		Log->warning("Exceed maximum vehicle!\t\t\t\t\t    ^^^^^^");*/
	
	return delay;
}

#endif


