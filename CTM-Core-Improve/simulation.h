#pragma once
#include <iostream>
#include <vector>
#include "ivector.h"
#include "node.h"
#include "setting.h"
#include "arc.h"
#include "cell.h"
#include "base.h"
#include "update.h"
#include "coordinate.h"
#include "intersection.h"
//#include "input.h"
#include "incident.h"
#include "debug.h"
//#include "output.h"
//#include "initialize.h"

class simulation {
public:
	simulation(char* inputname);

	void initialize();

	float excecute();

	void print_start();

	void output_result();

	float delay0;



private:
	

	// input part
	void skip(FILE* in);

	void input_setting(FILE* in);

	void input_geometry(FILE* in);

	void input_traffic(FILE* in);

	void classify_cell();

	void initial_diverge_cell_index();

	void initial_origin_demand();

	void input_intersection(FILE* in);

	void input_phase(FILE* in);

	void input_control(FILE* in);

	void input_event(FILE* in);

	void scanfile(char namestr[]);

	// initialize part
	void initial_diverge_flow();

	void initial_occupation(float* vehicle);

	void initial_control();

	void printoccup(char namestr[], float delay);

	void printdelay(char namestr[]);

	void printplan(char namestr[]);


	int present_clock;
	setting settings;
	//node nodes[MAX_NODE];
	std::vector<node> nodes;
	//arc arcs[MAX_ARC];
	std::vector<arc> arcs;
	//cell cells[MAX_CELL]; 
	std::vector<cell> cells; // len is number of cells + 1
	//intersection intersections[MAX_INTERSECTION];
	std::vector<intersection> intersections; // len is number of intersections
	incident incidents[MAX_INCIDENT];

	short index_next_cell[MAX_CELL][MAX_CELL];

	int origin_set[MAX_ORIGIN_CELL], normal_set[MAX_NORMAL_CELL],
		diverge_set[MAX_DIVERGE_CELL], merge_set[MAX_MERGE_CELL], destination_set[MAX_DESTINATION_CELL];

	int origin_size = 0, normal_size = 0, diverge_size = 0, merge_size = 0, destination_size = 0;

	float exist_vehicle[MAX_CLOCK][MAX_CELL];
	float diverge_flow[MAX_DIVERGE_CELL][MAX_ADJ_CELL];
	bool omega[MAX_CLOCK][MAX_INTERSECTION][MAX_PHASE];		//w -- timing plan

	int index_diverge_cell[MAX_CELL];
	int number_diverge_cell;


	float origin_demand[MAX_CLOCK][MAX_ORIGIN_CELL];
	demand temp_origin_demand[MAX_ORIGIN_CELL][MAX_CLOCK]; // if demand not changed then remain the same
	int temp_origin_demand_size = 0;
	float delay_record[MAX_CLOCK];

	debug* Log;

	float vehicle[MAX_CELL];

	char simuname[256];

	int start;



	//int present_clock;
	//setting settings;
	//std::vector<node> nodes;
	//std::vector<arc> arc;
	//std::vector<cell> cells;
	//std::vector<intersection> intersections;
	//std::vector<incident> incidents;

	//std::vector<std::vector<short>> index_next_cell;

	//std::vector<int> origin_set, normal_set,
	//	diverge_set, merge_set, destination_set;
	//int origin_size, normal_size, diverge_size, merge_size, destination_size;

	//std::vector<std::vector<float>> exist_vehicle;
	//std::vector<std::vector<float>> diverge_flow;
	//std::vector<std::vector<std::vector<float>>> omega;		//w -- timing plan

	//std::vector<int> index_diverge_cell;
	//int number_diverge_cell;


	//std::vector<std::vector<float>> origin_demand;
	//std::vector<std::vector<demand>> temp_origin_demand; // if demand not changed then remain the same
	//int temp_origin_demand_size;


	//debug* Log;

	//std::vector<float> vehicle;

};



