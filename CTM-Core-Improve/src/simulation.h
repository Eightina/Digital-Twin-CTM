#pragma once
#include <iostream>
#include <vector>
#include "ivector.h"
#include "node.h"
#include "setting.h"
#include "arc.h"
#include "cell.h"
#include "base.h"
//#include "update.h"
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

	void initialize(char* inputname);

	float excecute();

	void stepend();

	void print_start();

	void output_result();

	float delay0;

	int present_clock = 0;
	setting settings;

	// structure part
	std::vector<node> nodes = {}; // len is number of nodes + 1
	std::vector<arc> arcs = {}; // len is number of arcs + 1
	std::vector<cell> cells = {}; // len is number of cells + 1
	std::vector<intersection> intersections = {}; // len is number of intersections
	incident incidents[MAX_INCIDENT] = {}; // not implemented yet

	short index_next_cell[MAX_CELL][MAX_CELL] = {};
 
	int origin_set[MAX_ORIGIN_CELL] = {}, normal_set[MAX_NORMAL_CELL] = {},
		diverge_set[MAX_DIVERGE_CELL] = {}, merge_set[MAX_MERGE_CELL] = {}, destination_set[MAX_DESTINATION_CELL] = {};
	int origin_size = 0, normal_size = 0, diverge_size = 0, merge_size = 0, destination_size = 0;

	// update part
	float eventual_state[MAX_CELL] = {};
	float exist_vehicle[MAX_CLOCK][MAX_CELL] = {};
	float diverge_flow[MAX_DIVERGE_CELL][MAX_ADJ_CELL] = {};
	bool omega[MAX_CLOCK][MAX_INTERSECTION][MAX_PHASE] = {};		
	/*w -- timing plan; this defines the control signals in each tick, in each intersection,
	* whether the phases are on or off. working with cell.at_phase array
	* omega[i][j][m]
	* i takes current clock
	* j takes intersection id
	* m takes phase id
	*/
	

	int index_diverge_cell[MAX_CELL] = {};
	int number_diverge_cell = {};

	// demand part
	float origin_demand[MAX_CLOCK][MAX_ORIGIN_CELL] = {};
	demand temp_origin_demand[MAX_ORIGIN_CELL][MAX_CLOCK] = {};
	// if demand not changed then remain the same
	// 2d array temp_origin_demand[i][j] contains every source cell in order [i] and its demands [j]
	// i is just a numeric index
	// temp_origin_demand will be transferred to origin_demand in function simulation::initial_origin_demand() eventually 
	int temp_origin_demand_size = 0;
	float delay_record[MAX_CLOCK] = {};

	debug* Log;

	//float vehicle[MAX_CELL] = {};

	char simuname[128];

	int start;

private:
	

	// input part
	void skip(FILE* in);

	void input_setting(FILE* in);

	void input_geometry(FILE* in);

	void input_traffic(FILE* in);

	void classify_cell();

	void initial_diverge_cell_index();

	void initial_origin_demand(); // customized demands can be set here

	void input_intersection(FILE* in);

	void input_phase(FILE* in);

	void input_control(FILE* in);

	void input_event(FILE* in);

	void scanfile_construct(char namestr[]);

	void scanfile_initialize(char namestr[]);

	// initialize part
	void initial_diverge_flow();

	void initial_occupation(float* vehicle);

	void initial_control();

	void printoccup(char namestr[], float delay);

	void printdelay(char namestr[]);

	void printplan(char namestr[]);

	// update part
	void modify_control(const int& I, const int& t0, const int& t1, const int& P);

	void set_control_off(const int& I, const int& st, const int& et);

	void update_flow();

	void update_event();

	float update_occupation();

	float simulate(int st, int et);

	void set_on_all() {
		memset(omega, true, sizeof(omega));
	}



};



