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
#include "demand.h"
//#include "input.h"
#include "incident.h"
#include "debug.h"
//#include "output.h"
//#include "initialize.h"
#include <iostream>
#include <vector>


class simulation {
public:
	simulation(char* inputname);

	void initialize();

	float excecute();

	void print_start();

	void output_result();

	float delay0;

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

private:
	//shell part
	void shell_arc_emplace(int i, int un, int dn, float ms, float mf, float jd, float del);

	void shell_cell_emplace(int i, int arc, cell_type t, float len);

	void shell_create_cell(arc& cell_owner);

	void shell_diverge_update_flow(cell& cur_cell);

	void shell_origin_update_flow(cell& cur_cell);

	void shell_merge_update_flow(cell& cur_cell);

	void shell_destination_update_flow(cell& cur_cell);

	void shell_normal_update_flow(cell& cur_cell);

	float shell_move_vehicle(cell& cur_cell);

	float shell_send_flow(cell& cur_cell, int cn = 0);

	float shell_receive_flow(cell& cur_cell);

	void shell_set_out_flow(cell& cur_cell, const float& out, int next_cell_id);

	void shell_set_max_flow(cell& cur_cell, const float& mf);

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



