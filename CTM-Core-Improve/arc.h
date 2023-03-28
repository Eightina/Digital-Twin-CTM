#ifndef _arc_H

#define _arc_H 1
#include "simulation.h"
#include "node.h"
#include "setting.h"
#include <vector>


class arc {
public:
	//static int size;

	arc();

	arc(int i, int un, int dn, float ms, float mf, float jd, float del, float l, float cl); arc( int i, int un, int dn, float ms, float mf, float jd, float del );

	int get_id()const{ return id; }
	int get_up_node()const{ return up_node; }
	int get_down_node()const{ return down_node; }
	float get_max_speed()const{ return max_speed; }
	float get_max_flow()const{ return max_flow; }
	float get_jam_density()const{ return jam_density; }
	float get_delta()const{ return delta; }
	float get_length()const{ return length; }
	int get_num_cell()const{ return num_cell; }
	int get_first_cell()const { return first_cell; }
	int get_last_cell()const { return last_cell; }
	float get_cell_length()const { return cell_length; }

	void set_max_speed( float ms ){ max_speed = ms; }
	void set_max_flow( int mf ){ max_flow = mf; }
	void set_jam_density( float jd ){ jam_density = jd; }
	void set_delta( float d ){ delta = d; }
	void set_length(float l) { length = l; }
	void set_first_cell(int fc) { first_cell = fc; }
	void set_last_cell(int lc) { last_cell = lc; }
	


	//void create_cell();

private:
	//simulation* owner;
	int		id;
	int		up_node,down_node;
	float	max_speed,max_flow,jam_density,delta;
	float	length;
	float	cell_length;
	int		num_cell;
	int		first_cell,last_cell;	

};


#endif


