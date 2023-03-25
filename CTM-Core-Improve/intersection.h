#ifndef _intersection_H

#define _intersection_H

#include "coordinate.h"
//#include <vector>
#include "cell.h"
#include "arc.h"
#include "base.h"
#include "debug.h"
#include "setting.h"
#include "ivector.h"
//#include <assert.h>

extern cell cells[];
extern arc arcs[];
extern setting settings;

extern bool omega[MAX_CLOCK][MAX_INTERSECTION][MAX_PHASE];
extern int present_clock;
extern debug *Log;

//using std::vector;

class phase{
public:
	int id;
	ivector<int>  from_arc,to_arc;
	//phase();
	explicit phase( int i = 0 );
	void add_flow( int _from,int _to );
	/*inline void set_on();
	inline void set_off();*/
};


//inline void phase::set_on(){
//	for( int i = 0; i < from_arc.size(); ++i ){
//		int from_cell,to_cell;
//		from_cell = arcs[from_arc[i]].get_last_cell();
//		to_cell =	arcs[to_arc[i]].get_first_cell();
//		cells[from_cell].set_signals_on( to_cell );
//	}
//}

//inline void phase::set_off(){
//	for( int i = 0; i < from_arc.size(); ++i ){
//		int from_cell,to_cell;
//		from_cell = arcs[from_arc[i]].get_last_cell();
//		to_cell =	arcs[to_arc[i]].get_first_cell();
//		cells[from_cell].set_signals_off( to_cell );
//	}
//}

class intersection{
public:

	int size;
	intersection();
	intersection( int i,int t,int px,int py );
	intersection( int i,int t,int px,int py,int min_g,int max_g,int rturn,int num_p );

	int get_id()const{			return id; }
	int get_type()const{		return type; }
	coordinate get_pos()const{	return pos; }
	int get_min_green()const{	return min_green; }
	int get_max_green()const{	return max_green; }
	int get_min_green_ticks()const{ return min_green_ticks; }
	int get_max_green_ticks()const{ return max_green_ticks; }
	bool get_right_turning()const{	return right_turning; }
	int get_num_phases()const{		return num_phases; }
	//phase* get_phases_begin(){ return phases.begin(); }
	//phase* get_phases_end(){ return phases.end(); }

	void add_node( const int& i );
	void add_phase( int i,const int& from_arc,const int& to_arc );
	/*inline void update_signal();*/

	void print_phases(FILE *out);

private:
	int id;
	int type;
	coordinate pos;
	int min_green,max_green;
	bool right_turning;
	int num_phases;
	ivector<int> connected_nodes;
	ivector<phase> phases;
	int min_green_ticks,max_green_ticks;
};

//inline void intersection::update_signal(){
//	for( int i = 0; i < phases.size(); ++i ){
//		if( omega[present_clock][id][i] ){
//			phases[i].set_on();
//		}
//		else phases[i].set_off();
//	}
//}

#endif

