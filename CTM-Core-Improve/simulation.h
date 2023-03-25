//#pragma once
//#include <iostream>
//#include "ivector.h"
//#include "node.h"
//#include "setting.h"
//#include "arc.h"
//#include "cell.h"
//#include "base.h"
//#include "update.h"
//#include "coordinate.h"
//#include "intersection.h"
//#include "input.h"
//#include "incident.h"
//#include "debug.h"
//#include "output.h"
//#include "initialize.h"
//
//class simulation {
//public:
//	simulation(char* inputname);
//
//
//private:
//	void initial_control() {};
//
//	int present_clock;
//	setting settings;
//	node nodes[MAX_NODE];
//	arc arcs[MAX_ARC];
//	cell cells[MAX_CELL];
//	std::vector<intersection> intersections;
//	incident incidents[MAX_INCIDENT];
//	
//	short index_next_cell[MAX_CELL][MAX_CELL];
//	
//	int origin_set[MAX_ORIGIN_CELL], normal_set[MAX_NORMAL_CELL],
//		diverge_set[MAX_DIVERGE_CELL], merge_set[MAX_MERGE_CELL], destination_set[MAX_DESTINATION_CELL];
//	
//	int origin_size = 0, normal_size = 0, diverge_size = 0, merge_size = 0, destination_size = 0;
//	
//	float exist_vehicle[MAX_CLOCK][MAX_CELL];
//	float diverge_flow[MAX_DIVERGE_CELL][MAX_ADJ_CELL];
//	bool omega[MAX_CLOCK][MAX_INTERSECTION][MAX_PHASE];		//w -- timing plan
//	
//	int index_diverge_cell[MAX_CELL];
//	int number_diverge_cell;
//	
//	
//	float origin_demand[MAX_CLOCK][MAX_ORIGIN_CELL];
//	demand temp_origin_demand[MAX_ORIGIN_CELL][MAX_CLOCK]; // if demand not changed then remain the same
//	int temp_origin_demand_size = 0;
//	
//	
//	debug *Log;
//	
//	float vehicle[MAX_CELL];
//
//
//
//
//	//int present_clock;
//	//setting settings;
//	//std::vector<node> nodes;
//	//std::vector<arc> arc;
//	//std::vector<cell> cells;
//	//std::vector<intersection> intersections;
//	//std::vector<incident> incidents;
//
//	//std::vector<std::vector<short>> index_next_cell;
//
//	//std::vector<int> origin_set, normal_set,
//	//	diverge_set, merge_set, destination_set;
//	//int origin_size, normal_size, diverge_size, merge_size, destination_size;
//
//	//std::vector<std::vector<float>> exist_vehicle;
//	//std::vector<std::vector<float>> diverge_flow;
//	//std::vector<std::vector<std::vector<float>>> omega;		//w -- timing plan
//
//	//std::vector<int> index_diverge_cell;
//	//int number_diverge_cell;
//
//
//	//std::vector<std::vector<float>> origin_demand;
//	//std::vector<std::vector<demand>> temp_origin_demand; // if demand not changed then remain the same
//	//int temp_origin_demand_size;
//
//
//	//debug* Log;
//
//	//std::vector<float> vehicle;
//
//};
//
//
//
