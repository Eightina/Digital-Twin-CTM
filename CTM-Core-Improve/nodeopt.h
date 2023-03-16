#ifndef _nodeopt_H

#define _nodeopt_H
#include "base.h"

class nodeopt{
public:
	float delay;
	int clock;
	nodeopt* pre_nodeopt;
	int phase;
	float vehicle[MAX_CELL];
	float diverge_flow[MAX_DIVERGE_CELL][MAX_ADJ_CELL];
	int last_phase_gap;
};

class nodeAstar{
friend bool operator < (const nodeAstar& first, const nodeAstar second){
	return first.eval_delay > second.eval_delay;
	//return first.delay > second.delay;
}
public:
	float delay, eval_delay;
	int clock;
	float vehicle[MAX_CELL];
	float diverge_flow[MAX_DIVERGE_CELL][MAX_ADJ_CELL];
	int phase_size;
	int phases[MAX_PATH_SIZE];
	int phase_start_clock[MAX_PATH_SIZE];
	int phase_end_clock[MAX_PATH_SIZE];
	int last_phase_gap;
};


#endif
