#pragma once
#include <iostream>
#include "ivector.h"
#include "node.h"
#include "setting.h"
#include "arc.h"
#include "cell.h"
#include "base.h"
#include "update.h"
#include "coordinate.h"
#include "intersection.h"
#include "input.h"
#include "incident.h"
#include "debug.h"
#include "output.h"
#include "initialize.h"

class simulation {
public:
	simulation(char* inputname);


private:
	void initial_control() {};


	int present_clock;
	setting settings;
	std::vector<node> nodes[MAX_NODE];
	std::vector<arc> arcs[MAX_ARC];
	std::vector<cell> cells[MAX_CELL];
	std::vector<intersection> intersections[MAX_INTERSECTION];
	std::vector<incident> incidents[MAX_INCIDENT];

	std::vector<vector<short>> index_next_cell[MAX_CELL][MAX_CELL];

	std::vector<int> origin_set[MAX_ORIGIN_CELL], normal_set[MAX_NORMAL_CELL],
		diverge_set[MAX_DIVERGE_CELL], merge_set[MAX_MERGE_CELL], destination_set[MAX_DESTINATION_CELL];

	int origin_size, normal_size, diverge_size, merge_size, destination_size;

	float exist_vehicle[MAX_CLOCK][MAX_CELL];
	float diverge_flow[MAX_DIVERGE_CELL][MAX_ADJ_CELL];
	bool omega[MAX_CLOCK][MAX_INTERSECTION][MAX_PHASE];		//w -- timing plan

	int index_diverge_cell[MAX_CELL];
	int number_diverge_cell;


	float origin_demand[MAX_CLOCK][MAX_ORIGIN_CELL];
	demand temp_origin_demand[MAX_ORIGIN_CELL][MAX_CLOCK]; // if demand not changed then remain the same
	int temp_origin_demand_size;


	debug* Log;

	float vehicle[MAX_CELL];

};



