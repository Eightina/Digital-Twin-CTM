#ifndef _localopt_H

#define _localopt_H

#include "update.h"
#include "base.h"
#include "cell.h"
#include "arc.h"
#include "demand.h"
#include "node.h"
#include "setting.h"
#include "intersection.h"
#include "debug.h"
#include "nodeopt.h"
#include "iqueue.h"
#include "initialize.h"
#include <queue>
using namespace std;

extern int			present_clock;
extern setting		settings;
extern node			nodes[MAX_NODE];
extern arc			arcs[MAX_ARC];
extern cell			cells[MAX_CELL];
extern int origin_set[MAX_ORIGIN_CELL],normal_set[MAX_NORMAL_CELL],
	diverge_set[MAX_DIVERGE_CELL],merge_set[MAX_MERGE_CELL],destination_set[MAX_DESTINATION_CELL];

extern int origin_size,normal_size,diverge_size,merge_size,destination_size;
extern float		exist_vehicle[MAX_CLOCK][MAX_CELL];
extern float		diverge_flow[MAX_DIVERGE_CELL][MAX_ADJ_CELL];
extern int			index_diverge_cell[MAX_CELL];
extern intersection intersections[MAX_INTERSECTION];
extern incident		incidents[MAX_INCIDENT];
extern bool			omega[MAX_CLOCK][MAX_INTERSECTION][MAX_PHASE];
extern debug		*Log;
extern int			number_diverge_cell;

nodeopt inode[MAX_CLOCK][MAX_PHASE];
float h_delay[MAX_CLOCK], b_delay[MAX_CLOCK];

float dp_no_penalty( int id ){
	inode[0][0].pre_nodeopt = NULL;
	inode[0][0].clock = 0;
	inode[0][0].delay = 0.0;
	//Initialize.
	float vehicle[MAX_CELL];
	for( int i = 0; i <= settings.get_max_ticks(); ++i ){
		for( int  ip = 0; ip < intersections[id].get_num_phases(); ++ip ){
			inode[i][ip].delay = MAX_DELAY;
			inode[i][ip].clock = i;
			inode[i][ip].phase = ip;
			inode[i][ip].pre_nodeopt = NULL;
		}
	}

	//First phase.
	for( int op = 0; op < intersections[id].get_num_phases(); ++op ){

		modify_control( id,0,intersections[id].get_max_green_ticks(),op );
		float tmpdelay = 0.0;
		initial_diverge_flow();

		for( int t = 1; t <= intersections[id].get_max_green_ticks(); ++t ){
			
			tmpdelay += simulate( vehicle,vehicle,t-1,t );
			inode[t][op].delay = tmpdelay;
			memcpy( inode[t][op].vehicle,exist_vehicle[t],sizeof(float)*(1+cell::size));
			inode[t][op].pre_nodeopt = &inode[0][op];
			memcpy(inode[t][op].diverge_flow,diverge_flow,sizeof(float)*MAX_ADJ_CELL*number_diverge_cell);
		}
	}
	
	long start = clock();
	for( int  i = 1; i <= settings.get_max_ticks(); ++i ){
		int et = Min( i+intersections[id].get_max_green_ticks(),settings.get_max_ticks() );
		int st = i+intersections[id].get_min_green_ticks();
		if( st > settings.get_max_ticks() )
		{ 
			for( int ip = 0; ip < intersections[id].get_num_phases(); ++ip )
			{
				for( int jp = 0; jp < intersections[id].get_num_phases(); ++jp )
				{
					if( ip == jp ) continue;
					memcpy(diverge_flow,inode[i][ip].diverge_flow,sizeof(float)*MAX_ADJ_CELL*number_diverge_cell);
					memcpy(exist_vehicle[i],inode[i][ip].vehicle,sizeof(float)*(1+cell::size));
					modify_control(id,i,settings.max_ticks,jp);
					float tmpdelay2 = simulate(NULL,NULL,i,settings.max_ticks) + inode[i][ip].delay;
					int tmp_et = settings.max_ticks;
					if( tmpdelay2 < inode[tmp_et][jp].delay )
					{
						inode[tmp_et][jp].delay = tmpdelay2;
						memcpy(inode[tmp_et][jp].vehicle,exist_vehicle[tmp_et],sizeof(float)*(1+cell::size));
						inode[tmp_et][jp].pre_nodeopt = &inode[i][ip];
						memcpy(inode[tmp_et][jp].diverge_flow,diverge_flow,sizeof(float)*MAX_ADJ_CELL*(number_diverge_cell));
					}
				}
			}
			continue;
		}
		
		for( int  ip = 0; ip < intersections[id].get_num_phases(); ++ip ){
			for( int jp = 0; jp < intersections[id].get_num_phases(); ++jp ){
				if( ip == jp ) continue;
				modify_control( id, i, et, jp );
				//Load diverge flow.
				memcpy(diverge_flow,inode[i][ip].diverge_flow,sizeof(float)*MAX_ADJ_CELL*number_diverge_cell);
				memcpy(exist_vehicle[i],inode[i][ip].vehicle,sizeof(float)*(1+cell::size));
				float tmpdelay = simulate( inode[i][ip].vehicle,vehicle,i,st-1 )+inode[i][ip].delay;
				
				for( int t = st; t <= et; ++t ){
					tmpdelay += simulate( vehicle,vehicle,t-1,t );
					if( tmpdelay < inode[t][jp].delay ){
						inode[t][jp].delay = tmpdelay;
						memcpy(inode[t][jp].vehicle,exist_vehicle[t],sizeof(float)*(1+cell::size));
						inode[t][jp].pre_nodeopt = &inode[i][ip];
						memcpy(inode[t][jp].diverge_flow,diverge_flow,sizeof(float)*MAX_ADJ_CELL*(number_diverge_cell));
					}
				}
			}
		}
	}

	int best_phase = 0;
	for( int i = settings.get_max_ticks(),ip = 1; ip < intersections[id].get_num_phases(); ++ip ){
		if( inode[i][ip].delay < inode[i][best_phase].delay ){
			best_phase = ip;
		}
	}
	
	for( nodeopt *p = &inode[settings.get_max_ticks()][best_phase]; p->pre_nodeopt; p = p->pre_nodeopt ){
		modify_control( id, p->pre_nodeopt->clock+1, p->clock, p->phase );
	}

	//get heuristic values
	float temp = 0.0f;
	h_delay[0] = 0.0f;
	initial_diverge_flow();
	for( int it = 1; it <= settings.max_ticks; ++it )
	{
		temp += simulate( it,it );
		h_delay[it] = temp;
		//printf( "clock ticks#%03d ----> delay : %lf\n",it,temp );
	}
	for( int i = 0; i <= settings.max_ticks; ++i )
		h_delay[i] = (temp - h_delay[i]);
	return inode[settings.get_max_ticks()][best_phase].delay;
}


float dp_with_penalty(int id) // The intersection id 
{
	float temp_diverge_flow[MAX_DIVERGE_CELL][MAX_ADJ_CELL];
	inode[0][0].pre_nodeopt = NULL;
	inode[0][0].clock = 0;
	inode[0][0].delay = 0.0;
	//Initialize.
	float vehicle[MAX_CELL];
	for( int i = 0; i <= settings.get_max_ticks(); ++i )
	{
		for( int  ip = 0; ip < intersections[id].get_num_phases(); ++ip )
		{
			inode[i][ip].delay = MAX_DELAY;
			inode[i][ip].clock = i;
			inode[i][ip].phase = ip;
			inode[i][ip].pre_nodeopt = NULL;
		}
	}

	//First phase.
	for( int op = 0; op < intersections[id].get_num_phases(); ++op )
	{
		//modify_control( id,0,intersections[id].get_max_green_ticks(),op );
		float tmpdelay = 0.0;
		initial_diverge_flow();

		for( int t = 1; t <= intersections[id].get_max_green_ticks(); ++t )
		{
			modify_control(id,t-1,t,op);
			tmpdelay += simulate( NULL,NULL,t-1,t );
			float tmpdelay2 = tmpdelay;
			//save the previous diverge flow
			memcpy(temp_diverge_flow,diverge_flow,sizeof(float)*MAX_ADJ_CELL*number_diverge_cell);
			int tmp_et = t+settings.yellow_ticks;
			// calculate the yellow cost
			set_control_off(id,t,tmp_et);
			tmpdelay2 += simulate(NULL,NULL,t,tmp_et);
			// save state
			inode[tmp_et][op].delay = tmpdelay2;
			memcpy( inode[tmp_et][op].vehicle,exist_vehicle[tmp_et],sizeof(float)*(1+cell::size));
			inode[tmp_et][op].pre_nodeopt = &inode[0][op];
			memcpy(inode[tmp_et][op].diverge_flow,diverge_flow,sizeof(float)*MAX_ADJ_CELL*number_diverge_cell);
			//reLoad the previous state
			memcpy(diverge_flow,temp_diverge_flow,sizeof(float)*MAX_ADJ_CELL*number_diverge_cell);
			inode[tmp_et][op].last_phase_gap = t;
		}
	}
	
	long start = clock();
	for( int  i = 1+settings.yellow_ticks; i <= settings.get_max_ticks(); ++i )
	{
		int et = Min( i+intersections[id].get_max_green_ticks(),settings.get_max_ticks() );
		int st = i+intersections[id].get_min_green_ticks();
		if( st > settings.get_max_ticks() )
		{
			for( int ip = 0; ip < intersections[id].get_num_phases(); ++ip )
			{
				for( int jp = 0; jp < intersections[id].get_num_phases(); ++jp )
				{
					if( ip == jp ) continue;
					memcpy(diverge_flow,inode[i][ip].diverge_flow,sizeof(float)*MAX_ADJ_CELL*number_diverge_cell);
					memcpy(exist_vehicle[i],inode[i][ip].vehicle,sizeof(float)*(1+cell::size));
					modify_control(id,i,settings.max_ticks,jp);
					float tmpdelay2 = simulate(NULL,NULL,i,settings.max_ticks) + inode[i][ip].delay;
					int tmp_et = settings.max_ticks;
					if( tmpdelay2 < inode[tmp_et][jp].delay )
					{
						inode[tmp_et][jp].delay = tmpdelay2;
						memcpy(inode[tmp_et][jp].vehicle,exist_vehicle[tmp_et],sizeof(float)*(1+cell::size));
						inode[tmp_et][jp].pre_nodeopt = &inode[i][ip];
						memcpy(inode[tmp_et][jp].diverge_flow,diverge_flow,sizeof(float)*MAX_ADJ_CELL*(number_diverge_cell));
						inode[tmp_et][jp].last_phase_gap = settings.max_ticks - i;
					}
				}
			}
			continue;
		}
		
		for( int  ip = 0; ip < intersections[id].get_num_phases(); ++ip )
		{
			for( int jp = 0; jp < intersections[id].get_num_phases(); ++jp )
			{
				if( ip == jp ) continue;
				modify_control( id, i, st-1, jp );
				//Load diverge flow.
				memcpy(diverge_flow,inode[i][ip].diverge_flow,sizeof(float)*MAX_ADJ_CELL*number_diverge_cell);
				memcpy(exist_vehicle[i],inode[i][ip].vehicle,sizeof(float)*(1+cell::size));
				float tmpdelay = simulate( inode[i][ip].vehicle,vehicle,i,st-1 )+inode[i][ip].delay;
				
				for( int t = st; t <= et; ++t )
				{
					modify_control( id,t-1,t,jp );
					tmpdelay += simulate( vehicle,vehicle,t-1,t );
					float tmpdelay2 = tmpdelay;
					int tmp_et = t+settings.yellow_ticks;
					set_control_off(id,t,tmp_et);
					//save the previous diverge flow
					memcpy(temp_diverge_flow,diverge_flow,sizeof(float)*MAX_ADJ_CELL*number_diverge_cell);

					tmpdelay2 += simulate(NULL,NULL,t,tmp_et);

					if( tmpdelay2 < inode[tmp_et][jp].delay )
					{
						inode[tmp_et][jp].delay = tmpdelay2;
						memcpy(inode[tmp_et][jp].vehicle,exist_vehicle[tmp_et],sizeof(float)*(1+cell::size));
						inode[tmp_et][jp].pre_nodeopt = &inode[i][ip];
						memcpy(inode[tmp_et][jp].diverge_flow,diverge_flow,sizeof(float)*MAX_ADJ_CELL*(number_diverge_cell));
						inode[tmp_et][jp].last_phase_gap = t - i;
					}
					//reLoad previous state
					memcpy(diverge_flow,temp_diverge_flow,sizeof(float)*MAX_ADJ_CELL*number_diverge_cell);
				}
			}
		}
	}

	int best_phase = 0;
	for( int i = settings.get_max_ticks(),ip = 1; ip < intersections[id].get_num_phases(); ++ip ){
		if( inode[i][ip].delay < inode[i][best_phase].delay ){
			best_phase = ip;
		}
	}
	
	for( nodeopt *p = &inode[settings.get_max_ticks()][best_phase]; p->pre_nodeopt; p = p->pre_nodeopt ){
		modify_control( id, p->pre_nodeopt->clock+1, p->pre_nodeopt->clock+p->last_phase_gap, p->phase );
		set_control_off( id, p->pre_nodeopt->clock+p->last_phase_gap+1, p->clock );
	}
	
	//get heuristic values
	float temp = 0.0f;
	b_delay[0] = 0.0f;
	initial_diverge_flow();
	for( int it = 1; it <= settings.max_ticks; ++it )
	{
		temp += simulate( it,it );
		b_delay[it] = temp;
		//printf( "clock ticks#%03d ----> delay : %lf\n",it,temp );
	}
	for( int i = 0; i <= settings.max_ticks; ++i )
		b_delay[i] = (temp - b_delay[i]);

	return inode[settings.get_max_ticks()][best_phase].delay;
}

//struct nodeAstar{
//	float delay;
//	int clock;
//	float vehicle[MAX_CELL];
//	float diverge_flow[MAX_DIVERGE_CELL][MAX_ADJ_CELL];
//	int phase_size;
//	int phases[MAX_PATH_SIZE];
//	int phase_start_clock[MAX_PATH_SIZE];
//	int phase_end_clock[MAX_PATH_SIZE];
//	int last_phase_gap;
//};

float localAstar(int id)
{
	int tmp_et, st, et;
	int ip, it, i, iterative, cut, max_queue;
	float tmp_diverge_flow[MAX_DIVERGE_CELL][MAX_ADJ_CELL];
	float tmp_delay, present_delay, eval_delay;
	static float boundary_delay;
	nodeAstar new_node;
	iqueue<nodeAstar> Queue;

	//heuristic value
	float h_factor = 1.0;
	initial_diverge_flow();
	dp_no_penalty(id);
	for( it = 0; it <= settings.max_ticks; ++it )
		h_delay[it] = h_delay[it] * h_factor;
	
	//boundary delay
	initial_diverge_flow();
	initial_control();
	boundary_delay = dp_with_penalty(id);
	printf( "  Initial boundary delay: %f\n",boundary_delay );

	//Search algorithms
	nodeAstar* Best = NULL;
	nodeAstar pop_node;

	//push all the first phases
	for( ip = 0; ip < intersections[id].get_num_phases(); ++ip )
	{
		initial_diverge_flow();
		tmp_delay = 0.0f;
		for( it = 1; it <= intersections[id].get_max_green_ticks(); ++it )
		{
			//simulation sequence
			modify_control(id,it,it,ip);
			tmp_delay += simulate(it,it);
			memcpy(tmp_diverge_flow,diverge_flow,sizeof(float)*MAX_ADJ_CELL*number_diverge_cell);
			//calculate lost time
			tmp_et = it + settings.yellow_ticks;
			set_control_off(id,it+1,tmp_et);
			present_delay = tmp_delay + simulate(it+1,tmp_et);

			//get fisrt phase
			new_node.clock = tmp_et;
			new_node.delay = present_delay;
			new_node.eval_delay = present_delay + h_delay[tmp_et];
			memcpy(new_node.diverge_flow, diverge_flow, sizeof(float)*MAX_ADJ_CELL*number_diverge_cell);
			memcpy(new_node.vehicle, exist_vehicle[new_node.clock], sizeof(float)*(1+cell::size));
			new_node.phase_size = 1;
			new_node.phases[0] = ip;
			new_node.phase_start_clock[0] = 1;
			new_node.phase_end_clock[0] = tmp_et;
			new_node.last_phase_gap = it - 1;
			Queue.push(new_node);
			//next simulation
			memcpy(diverge_flow,tmp_diverge_flow,sizeof(float)*MAX_ADJ_CELL*number_diverge_cell);
		}
	}

	//A* algorithm goes here.
	cut = 0;
	max_queue = 0;
	while( !Queue.empty() )
	{	
		if(max_queue < Queue.size())
			max_queue = Queue.size();
		//get the top node
		pop_node = Queue.top();
		Queue.pop();

		//find the best solution
		if( pop_node.clock == settings.max_ticks )
		{
			Best = &pop_node;
			break;
		}

		//expand nodes
		memcpy(exist_vehicle[pop_node.clock],pop_node.vehicle,sizeof(float)*(1+cell::size));
		st = pop_node.clock + intersections[id].get_min_green_ticks();
		et = pop_node.clock + intersections[id].get_max_green_ticks();
		et = Min( et,settings.max_ticks );

		if( st > et )
		{
			for( ip = 0; ip < intersections[id].get_num_phases(); ++ip )
			{	
				//no adjacent phase are the same
				if( ip == pop_node.phases[pop_node.phase_size-1] )
					continue;
				
				//the last phases
				memcpy(diverge_flow,pop_node.diverge_flow,sizeof(float)*MAX_ADJ_CELL*number_diverge_cell);
				modify_control(id,pop_node.clock+1,settings.max_ticks,ip);
				tmp_delay = pop_node.delay + simulate(pop_node.clock+1,et);

				new_node = pop_node;
				new_node.clock = et;
				new_node.delay = tmp_delay;
				new_node.eval_delay = tmp_delay + h_delay[tmp_et];
				memcpy(new_node.diverge_flow, diverge_flow, sizeof(float)*MAX_ADJ_CELL*number_diverge_cell);
				memcpy(new_node.vehicle, exist_vehicle[new_node.clock], sizeof(float)*(1+cell::size));
				new_node.phases[new_node.phase_size] = ip;
				new_node.phase_start_clock[new_node.phase_size] = pop_node.clock + 1;
				new_node.phase_end_clock[new_node.phase_size] = settings.max_ticks;
				new_node.last_phase_gap = settings.max_ticks - pop_node.clock;
				new_node.phase_size++;
				Queue.push(new_node);
			}
		}
		else{
			for( ip = 0; ip < intersections[id].get_num_phases(); ++ip )
			{	
				//no adjacent phase are the same
				if( ip == pop_node.phases[pop_node.phase_size-1] )
					continue;

				//simualtion of min green time
				memcpy(diverge_flow,pop_node.diverge_flow,sizeof(float)*MAX_ADJ_CELL*number_diverge_cell);
				modify_control(id,pop_node.clock+1,st-1,ip);
				tmp_delay = pop_node.delay + simulate(pop_node.clock+1,st-1);

				for( it = st; it <= et; ++it )
				{	
					//simulation sequence
					modify_control(id,it,it,ip);
					tmp_delay += simulate(it,it);
					memcpy(tmp_diverge_flow,diverge_flow,sizeof(float)*MAX_ADJ_CELL*number_diverge_cell);

					//calaulate lost time
					tmp_et = it + settings.yellow_ticks;
					tmp_et = Min( tmp_et,settings.max_ticks );
					set_control_off(id,it+1,tmp_et);
					present_delay = tmp_delay + simulate(it+1,tmp_et);
					
					//tighten up boundary
					//if(present_delay + b_delay[tmp_et] < boundary_delay )
					//	boundary_delay = present_delay + b_delay[tmp_et];

					//cut branches
					eval_delay = present_delay + h_delay[tmp_et];
					if( eval_delay > boundary_delay )
					{
						cut++;
						memcpy(diverge_flow,tmp_diverge_flow,sizeof(float)*MAX_ADJ_CELL*number_diverge_cell);
						continue;
					}
					//expand new nodes
					new_node = pop_node;
					new_node.clock = tmp_et;
					new_node.delay = present_delay;
					new_node.eval_delay = eval_delay;
					memcpy(new_node.diverge_flow, diverge_flow, sizeof(float)*MAX_ADJ_CELL*number_diverge_cell);
					memcpy(new_node.vehicle, exist_vehicle[new_node.clock], sizeof(float)*(1+cell::size));
					new_node.phases[new_node.phase_size] = ip;
					new_node.phase_start_clock[new_node.phase_size] = pop_node.clock + 1;
					new_node.phase_end_clock[new_node.phase_size] = tmp_et;
					new_node.last_phase_gap = it - pop_node.clock - 1;
					new_node.phase_size++;
					Queue.push(new_node);

					//next simulation
					memcpy(diverge_flow,tmp_diverge_flow,sizeof(float)*MAX_ADJ_CELL*number_diverge_cell);
				}
			}
		}
	}

	printf( "  Cut branches : %d\n",cut );
	if( !Best )
	{
		printf("  Iterative %d: Cannot find solution!\n", iterative);
		exit(1);
	}
	
	while(!Queue.empty()){
		if((Queue.top()).eval_delay<=boundary_delay)
			cout<<"  clock: "<<(Queue.top()).clock<<", delay: "<<(Queue.top()).delay<<", eval_delay: "<<(Queue.top()).eval_delay<<endl;
		Queue.pop();
	}

	printf("  Maximum queue size: %d\n\n", max_queue);

	//modify omega
	for( i = 0; i < Best->phase_size - 1; ++i )
	{
		modify_control(id,Best->phase_start_clock[i],Best->phase_end_clock[i]-settings.yellow_ticks,Best->phases[i]);
		set_control_off(id,Best->phase_end_clock[i]-settings.yellow_ticks+1,Best->phase_end_clock[i]);
	}
	modify_control(id,Best->phase_start_clock[i],Best->phase_start_clock[i]+Best->last_phase_gap,Best->phases[i]);
	set_control_off(id,Best->phase_start_clock[i]+Best->last_phase_gap+1,Best->phase_end_clock[i]);

	//	printf("Astar optimised delay: %f\n",Best->delay);
	return Best->delay;
}

float localopt(int id){
	return dp_with_penalty(id);
}

#endif

