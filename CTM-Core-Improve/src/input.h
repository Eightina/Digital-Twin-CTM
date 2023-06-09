//#ifndef _input_H
//
//#define _input_H
//
//#include "base.h"
////#include <assert.h>
//#include "setting.h"
//#include "arc.h"
//#include "cell.h"
//#include "demand.h"
//#include "intersection.h"
//#include "node.h"
//#include "coordinate.h"
//#include "incident.h"
//#include "debug.h"
//#include "ivector.h"
//#include "simulation.h"
//
////#include <vector>
//
////using std::vector;
//
//
//
////extern int present_clock;
////extern setting settings;
////extern std::vector<node> nodes;
////extern std::vector<arc> arcs;
////extern std::vector<cell> cells;
////extern int origin_set[MAX_ORIGIN_CELL],normal_set[MAX_NORMAL_CELL],
////	diverge_set[MAX_DIVERGE_CELL],merge_set[MAX_MERGE_CELL],destination_set[MAX_DESTINATION_CELL];
////
////extern int origin_size,normal_size,diverge_size,merge_size,destination_size;
////extern float exist_vehicle[MAX_CLOCK][MAX_CELL];
////extern float diverge_flow[MAX_DIVERGE_CELL][MAX_ADJ_CELL];
////extern int index_diverge_cell[MAX_CELL];
////extern int number_diverge_cell;
////
//////extern std::vector<intersection> intersections;
////extern vector<intersection> intersections;
////extern incident incidents[MAX_INCIDENT];
////extern debug *Log;
////extern float origin_demand[MAX_CLOCK][MAX_ORIGIN_CELL];
////extern demand temp_origin_demand[MAX_ORIGIN_CELL][MAX_CLOCK];
////extern int temp_origin_demand_size;
//
//
//
//void skip(FILE* in) {
//	//Log->process("Going to skipping line...\n");
//	int key = fgetc(in);
//	while (true) {
//		if (key == '%') {
//			while (fgetc(in) != '\n');
//		}
//		else if (key != ' ' && key != '\n') {
//			//fputc(key,in); 
//			ungetc(key, in);
//			break;
//		}
//		key = fgetc(in);
//	}
//	return;
//}
//
//void input_setting(FILE* in) {
//	skip(in);
//	char keyword[1024];
//	int hh, mm, ss, sec = 0;
//	fscanf(in, "%s", keyword);
//	while (strcmpi("end", keyword)) {
//		if (strcmpi("time", keyword) == 0) {
//			Log->process("Going to input time...");
//			fscanf(in, "%d:%d:%d", &hh, &mm, &ss);
//			sec = hh * 3600 + mm * 60 + ss;
//			settings.start_time = sec;
//			fscanf(in, "%d:%d:%d", &hh, &mm, &ss);
//			sec = hh * 3600 + mm * 60 + ss;
//			settings.end_time = sec;
//			Log->process("Input times successfully...");
//		}
//		else if (strcmpi("clock", keyword) == 0) {
//			Log->process("Going to input clock...");
//			fscanf(in, "%d", &sec);
//			settings.clock_tick = sec;
//			Log->process("Input clock successfully...");
//		}
//		else if (strcmpi("epsilon", keyword) == 0) {
//			Log->process("Going to input epsilon...");
//			fscanf(in, "%f", &settings.epsilon);
//			Log->process("Input epsilon successfully...");
//		}
//		else if (strcmpi("initoccp", keyword) == 0) {
//			Log->process("Going to input initoccp...");
//			fscanf(in, "%d", &settings.initial_occupation);
//			Log->process("Input initoccp successfully...");
//		}
//		else if (strcmpi("initctrl", keyword) == 0) {
//			Log->process("Going to input initctrl...");
//			fscanf(in, "%d", &settings.initial_control);
//			Log->process("Input initctrl successfully...");
//		}
//		else if (strcmpi("cellscale", keyword) == 0) {
//			Log->process("Going to input cellscal...");
//			fscanf(in, "%f", &settings.cell_length_factor);
//			Log->process("Input cellscale successfully...");
//		}
//		else if (strcmpi("losttime", keyword) == 0) {
//			Log->process("Going to input losttime...");
//			fscanf(in, "%d", &settings.yellow_time);
//			settings.yellow_ticks = ((settings.yellow_time / settings.clock_tick == 0) ?
//				1 : (settings.yellow_time / settings.clock_tick));
//			Log->process("Input losttime successfully...");
//		}
//		skip(in);
//		fscanf(in, "%s", keyword);
//	}
//	settings.max_ticks = (int)ceil((settings.end_time - settings.start_time) * 1.0 / settings.clock_tick);
//	Log->process("Input setting successfully...");
//}
//
//void input_geometry(FILE* in) {
//	skip(in);
//	char str[1024];
//	int id, type, x, y;
//	Log->process("Going to input node...");
//	//memset(str,0,1024);
//	if (nodes.empty()) nodes.push_back(node());
//	while (fscanf(in, "%s", str), strcmpi(str, "arc")) {
//		fscanf(in, "%d%d%d%d", &id, &type, &x, &y);
//		//int tmp_node_size = node::size + 1;
//		//nodes[tmp_node_size] = node( id,type,x,y );
//		nodes.emplace_back(id, type, x, y);
//		skip(in);
//	}
//
//	char sstr[256];
//	Log->process("Input node successfully...");
//	//memset(sstr,0,256);
//	sprintf(sstr, "The size of node is: %d...", nodes.size());
//	Log->process(sstr);
//
//	int ai, anf, anl;
//	float as, af, ad, adel;
//	Log->process("Going to input arc...");
//	if (arcs.empty()) arcs.push_back(arc());
//	while (strcmpi(str, "end")) {
//		fscanf(in, "%d%d%d%f%f%f%f", &ai, &anf, &anl, &as, &af, &ad, &adel);
//		//int tmp_arc_size = arc::size + 1;
//		//int tmp_arc_size = arcs.size();
//		//arcs[tmp_arc_size] = arc( ai,anf,anl,as,af,ad,adel );
//		//arcs[tmp_arc_size].create_cell();
//		arcs.emplace_back(ai, anf, anl, as, af, ad, adel);
//		arcs.back().create_cell();
//		skip(in);
//		//memset(str,0,1024);
//		fscanf(in, "%s", str);
//	}
//	Log->process("Input arc successfully...");
//	sprintf(str, "The size of arc is: %d...", arcs.size());
//	Log->process(str);
//	Log->process("Input geometry successfully...");
//}
//
//void input_traffic(FILE* in) {
//	char str[1024];
//	skip(in);
//	//demand
//	int hh, mm, ss, origin_node, sec;
//	float traffic_demand;
//
//	int __count = 0;
//	Log->process("Going to input demand ...");
//	while (fscanf(in, "%s", str), strcmpi(str, "demand") == 0) {
//		fscanf(in, "%d:%d:%d %d %f", &hh, &mm, &ss, &origin_node, &traffic_demand);
//		//demands
//		//cl
//		sec = hh * 3600 + mm * 60 + ss;
//		sprintf(Log->get_str(), "Origin Node#%03d at Arc#%03d",
//			origin_node, nodes[origin_node].get_arc(), arcs[nodes[origin_node].get_arc()].get_first_cell());
//		Log->process(Log->get_str());
//		int id = arcs[nodes[origin_node].get_arc()].get_first_cell();
//		cells[id].add_demand(sec, traffic_demand);
//		skip(in);
//		__count++;
//	}
//	char sstr[256];
//	//memset(sstr,0,256);
//	Log->process("Input arc successfully...");
//	sprintf(sstr, "The size of demand is: %d ...", __count);
//	Log->process(sstr);
//
//	//We need some dummy cells here.
//
//	__count = 0;
//	Log->process("Going to input diverge ...");
//	while (strcmpi("diverge", str) == 0) {
//		int from, to, type;
//		float ceoff;
//		fscanf(in, "%d %d %f %d", &from, &to, &ceoff, &type);
//		from = arcs[from].get_last_cell();
//		to = arcs[to].get_first_cell();
//		//		cells[from].set_type(diverge);
//		cells[from].add_next_cell(to, ceoff, type);
//		cells[from].set_type(diverge);
//		cells[to].add_previous_cell(from);
//		skip(in);
//		//memset(str,0,1024);
//		fscanf(in, "%s", str);
//		__count++;
//	}
//	//memset(str,0,1024);
//	Log->process("Input diverge successfully...");
//	sprintf(sstr, "The size of diverge is: %d ...", __count);
//	Log->process(sstr);
//
//	__count = 0;
//	Log->process("Going to input merge...");
//	while (strcmpi("end", str)) {
//		int from, to;
//		fscanf(in, "%d %d", &from, &to);
//		from = arcs[from].get_last_cell();
//		to = arcs[to].get_first_cell();
//		//		cells[to].set_type(Cell::merge);
//		cells[to].add_previous_cell(from);
//		cells[from].add_next_cell(to);
//		skip(in);
//		//memset(str,0,1024);
//		fscanf(in, "%s", str);
//		__count++;
//	}
//	//memset(str,0,1024);
//	Log->process("Input merge successfully...");
//	sprintf(sstr, "The size of merge is: %d ...", __count);
//	Log->process(sstr);
//	Log->process("Input traffic successfully...");
//}
//
//void classify_cell() {
//	origin_size = normal_size = diverge_size = merge_size = destination_size = 0;
//	//for( int i = 1; i <= cell::size; ++i ){
//	for (int i = 1; i < cells.size(); ++i) {
//
//		switch (cells[i].get_type()) {
//		case normal: normal_set[normal_size++] = (i); break;
//		case origin: origin_set[origin_size++] = (i); break;
//		case destination: destination_set[destination_size++] = (i); break;
//		case diverge: diverge_set[diverge_size++] = (i); break;
//		case Cell::merge: merge_set[merge_size++] = (i); break;
//		}
//	}
//}
//
//void initial_diverge_cell_index() {
//	number_diverge_cell = diverge_size;
//	for (int i = 0; i < number_diverge_cell; ++i) {
//		index_diverge_cell[diverge_set[i]] = i;
//	}
//}
//
//void initial_origin_demand() {
//	for (int i = 0; i < origin_size; ++i) {
//		int n = cells[origin_set[i]].get_num_demand();
//		int cell_id = origin_set[i], pre_clock = 0, end_clock = 0;
//		int idemand = cells[cell_id].get_temp_origin_demand_id();
//		sort(temp_origin_demand[idemand], temp_origin_demand[idemand] + n);
//		float pre_flow = 0.0;
//		demand* iter;
//		for (int jd = 0; jd < n; jd++) {
//			iter = &temp_origin_demand[idemand][jd];
//			end_clock = (iter->clock - settings.start_time) / settings.clock_tick
//				+ ((((iter->clock - settings.start_time) % settings.clock_tick) == 0) ? 0 : 1);
//
//			while (pre_clock <= end_clock)
//				origin_demand[pre_clock++][idemand] = pre_flow;
//
//			pre_flow = iter->traffic * settings.clock_tick;
//		}
//		while (pre_clock <= settings.get_max_ticks())
//			origin_demand[pre_clock++][idemand] = pre_flow;
//	}
//}
//
///**
//%                 (1)      (2)       (3)        (4)       (5)       (6)       (7)      (8)            (9)
//%                                                      min green max green   right    number       connected
//%                 No.      type       x          y        time      time    turning  of phase        nodes
//intersection       1        1        456        550        20        60        1        4       1 2 3 4 5 6 7 8
//*/
//
//void input_intersection(FILE* in) {
//	int id, type, x, y, min_g = 0, max_g = 0, right_t = 0, num_phase = 0;
//
//	Log->process("Going to input intersection.....*");
//	fscanf(in, "%d%d%d%d", &id, &type, &x, &y);
//	char str[256];
//	if (type) {
//		fscanf(in, "%d%d%d%d", &min_g, &max_g, &right_t, &num_phase);
//		//int tmp_intersection_size = intersections.size() + 1;
//		//intersections[tmp_intersection_size] = intersection( id,type,x,y,min_g,max_g,right_t,num_phase );
//		intersections.emplace_back(id, type, x, y, min_g, max_g, right_t, num_phase);
//
//		while (true) {
//			memset(str, 0, 256);
//			while ((str[0] = fgetc(in)) == ' ');
//			if (str[0] == '\n') break;
//
//			id = 0;
//			while (str[id] != ' ' && str[id] != '\n') str[++id] = fgetc(in);
//			int i;
//			sscanf(str, "%d", &i);
//			Assert(i >= 0);
//			//intersections[tmp_intersection_size].add_node(i);
//			intersections.back().add_node(i);
//			if (str[id] == '\n') break;
//		}
//	}
//	else {
//		//int tmp_intersection_size = intersection::size + 1;
//		//intersections[tmp_intersection_size] = intersection( id,type,x,y );
//		intersections.emplace_back(id, type, x, y);
//	}
//	Log->process("Input intersection successfully...");
//}
//
///*
//% The followings are the phases of the intersection:
//%           (1)      (2)      (3)      (4)
//%      intersection phase     from     to
//%           No.      No.      arc      arc
//phase        1        1        1        6
//*/
//
//void input_phase(FILE* in) {
//	Log->process("Going to input phase...");
//	int id_inter, id_phase, from_arc, to_arc;
//	fscanf(in, "%d%d%d%d", &id_inter, &id_phase, &from_arc, &to_arc);
//	id_inter--;
//	intersections[id_inter].add_phase(id_phase, from_arc, to_arc);
//
//	int from_cell_id = arcs[from_arc].get_last_cell();
//	int to_cell_id = arcs[to_arc].get_first_cell();
//	cells[from_cell_id].add_at_phase(to_cell_id, id_phase - 1);
//	cells[from_cell_id].set_on_intersection(id_inter);
//}
//
//void input_control(FILE* in) {
//	skip(in);
//	char str[1024];
//	Log->process("Going to input control...");
//	while (true) {
//		fscanf(in, "%s", str);
//		if (strcmpi("intersection", str) == 0) {
//			input_intersection(in);
//		}
//		else if (strcmpi("phase", str) == 0) {
//			input_phase(in);
//		}
//		else break;
//		skip(in);
//	}
//	//Log->process("Input control successfully...");
//	sprintf(str, "The size of intersection is: %d ...", intersections.size());
//	Log->process(str);
//	Log->process("Input control successfully...");
//}
//
//void input_event(FILE* in) {
//	skip(in);
//	char str[1024];
//	Log->process("Going to input incident...");
//	while (true) {
//		int at_arc;
//		float dist;
//		int sy, sm, ss, ey, em, es;
//		float mf;
//		fscanf(in, "%s", str);
//		if (strcmpi("event", str)) break;
//		fscanf(in, "%d %f %d:%d:%d %d:%d:%d %f", &at_arc, &dist, &sy, &sm, &ss, &ey, &em, &es, &mf);
//		int st = sy * 3600 + sm * 60 + ss;
//		int et = ey * 3600 + em * 60 + es;
//		int tmp_incident_size = incident::size + 1;
//		incidents[tmp_incident_size] = incident(at_arc, dist, st, et, mf);
//		skip(in);
//	}
//	Log->process("Input incident successfully...");
//	sprintf(str, "The size of incident is: %d", incident::size);
//	Log->process(str);
//	Log->process("Input event successfully...");
//}
//
//
//
//void scanfile(char namestr[]) {
//	char full_name[256];
//	strcpy(full_name, namestr);
//	strcat(full_name, ".txt");
//	FILE* in = fopen(full_name, "r");
//	if (in == NULL) {
//		Log = new debug(namestr);
//		char str[256];
//		sprintf(str, "Cannot open input file: \t%s\n", namestr);
//		Log->throws(str);
//		exit(0);
//	}
//	else {
//		Log = new debug(namestr);
//	}
//	temp_origin_demand_size = 0;
//	char line[1024], * pstr;
//	memset(index_diverge_cell, 0, sizeof(index_diverge_cell));
//	while (!feof(in)) {
//		fgets(line, 1023, in);
//		pstr = line;
//		while (*pstr == ' ') pstr++;
//		if (*pstr == '\0') continue;
//		if (*pstr == '%') continue;
//		if (strnicmp("setting", pstr, 7) == 0) {
//			Log->process("input setting...");
//			input_setting(in);
//		}
//		else if (strnicmp("geometry", pstr, 8) == 0) {
//			Log->process("input geometry...");
//			input_geometry(in);
//		}
//		else if (strnicmp("traffic", pstr, 7) == 0) {
//			Log->process("input traffic...");
//			input_traffic(in);
//			classify_cell();
//			initial_origin_demand();
//			initial_diverge_cell_index();
//		}
//		else if (strnicmp("control", pstr, 7) == 0) {
//			Log->process("input control...");
//			input_control(in);
//		}
//		else if (strnicmp("event", pstr, 5) == 0) {
//			Log->process("input event...");
//			input_event(in);
//		}
//	}
//
//	fclose(in);
//
//
//}
//#endif
//
