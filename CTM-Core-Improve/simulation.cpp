#include "simulation.h"



simulation::simulation(char* inputname) {
	std::cout << "simulation object" << inputname << std::endl;
	print_start();
	scanfile(inputname);
	strcpy(inputname, simuname);
}

void simulation::initialize() {
	start = clock();
	initial_control();
	initial_occupation(exist_vehicle[0]);
	initial_diverge_flow();
	delay0 = simulate(1, settings.get_max_ticks());
}

float simulation::excecute() {
	delay0 = simulate(1, settings.get_max_ticks());
	return delay0;
}

// print part
void simulation::print_start() {
	printf("\n");
	printf(" CTM Core Improve is a signalized intersection simulator based on Green Signal project \n");
	printf(" implemented by cell transmission model \n");
	printf(" CTM Core Improve is free software. You can redistribute it or modify it\n");
	printf(" under the terms of the GNU General Public License 3.0.\n\n");
	printf(" Green Signal Developer: Zhang Kai, Wang Yi, Xiong Yiliang.\n");
	printf(" CTM Core Improve Developer: Orion Que @ Tongji University.\n");
	printf(" Green Signal Last Update: April 2008.\n\n\n");
	printf(" CTM Core Improve Last Update: March 2023.\n\n\n");
	printf(" ******* CTM Core Improve is running now. *******\n\n");
}

void simulation::output_result() {
	printf("Total delay (before optimization): %7.2lf veh*sec.\n", delay0);
	printf("Running time: %.3f seconds.\n\n", double(clock() - start) / CLOCKS_PER_SEC);
	printoccup(simuname, delay0);
	printplan(simuname);
	printdelay(simuname);
	printf("Timing plan output files\n >>t_%s.txt\n", &simuname[2]);
	printf("Cell occupation output files\n >>o_%s.txt\n", &simuname[2]);
	printf("Detail delay output files\n >>d_%s.txt\n", &simuname[2]);
	printf("\nSimulation %s Done.\n\n", &simuname);
	system("pause");
}

// shell part
void simulation::shell_arc_emplace(int i, int un, int dn, float ms,
		float mf, float jd, float del) {
	if (del < 0.0) {
		del = mf * settings.clock_tick /
			(jd * ms - settings.clock_tick * mf);
	}
	nodes[un].set_arc(i);
	nodes[dn].set_arc(i);
	float l = nodes[un].get_pos().dist(nodes[dn].get_pos());
	float cl = settings.cell_length_factor * ms * settings.clock_tick;

	arcs.emplace_back(i, un, dn, ms, mf, jd, del, l, cl);
	if (arcs.back().get_num_cell() < 2) {
		char str[256];
		sprintf(str, "In Arc#%03d  num_cell < 2 ", i);
		Log->throws(str);
	}
}

void simulation::shell_cell_emplace(int i, int arc, cell_type t, float len) {
	Assert(cells.size() < MAX_CELL);
	float max_speed = arcs[arc].get_max_speed();
	float max_flow = arcs[arc].get_max_flow() * (settings.clock_tick);
	float jam_density = arcs[arc].get_jam_density();
	float delta = arcs[arc].get_delta();

	cells.emplace_back(i, arc, t, len, max_speed, max_flow, jam_density, delta);


};

void simulation::shell_create_cell(arc& cell_owner) {
	if (cells.empty()) cells.push_back(cell());
	Assert(cells.size() <= MAX_CELL + 1);
	float LL = cell_owner.get_length();
	int tmp_id = cell_owner.get_id();
	int tmp_first_cell = cells.size();
	int tmp_last_cell = cell_owner.get_first_cell() + cell_owner.get_num_cell() - 1;
	cell_owner.set_first_cell(tmp_first_cell);
	cell_owner.set_last_cell(tmp_last_cell);
	sprintf(Log->get_str(), "Arc#%03d: First Cell #%03d, Last Cell #%03d",
				tmp_id,
				tmp_first_cell,
				tmp_last_cell
			);
	Log->process(Log->get_str());
	int tmp_cell_size;
	int tmp_cell_length = cell_owner.get_cell_length();
	while (LL >= 2 * tmp_cell_length) {
		LL -= tmp_cell_length;
		tmp_cell_size = cells.size();
		//owner->cells[tmp_cell_size] = cell( tmp_cell_size,id,normal,cell_length );
		//
		//cells.emplace_back(tmp_cell_size, id, normal, cell_length);
		shell_cell_emplace(tmp_cell_size, tmp_id, normal, tmp_cell_length);
		char str[256];
		sprintf(str, "Create Cell#%03d successfully", tmp_cell_size);
		Log->process(str);
	}

	int tmp_type = nodes[cell_owner.get_down_node()].get_type();
	if (tmp_type == 2) {
		tmp_cell_size = cells.size();
		//cells.emplace_back(tmp_cell_size, tmp_id, destination, LL);
		shell_cell_emplace(tmp_cell_size, tmp_id, destination, LL);
		char str[256];
		sprintf(str, "Create Cell#%03d successfully", tmp_cell_size);
		Log->process(str);
	}
	else {
		tmp_cell_size = cells.size();
		//cells.emplace_back(tmp_cell_size, tmp_id, normal, LL);
		shell_cell_emplace(tmp_cell_size, tmp_id, normal, LL);
		char str[256];
		sprintf(str, "Create Cell#%03d successfully", tmp_cell_size);
		Log->process(str);
	}

	if (tmp_type == 1) {
		cells[tmp_first_cell].set_type(origin);
	}

	for (int i = tmp_first_cell + 1; i <= tmp_last_cell; ++i) {
		cells[i].add_previous_cell(i - 1);
	}
	for (int i = tmp_first_cell; i < tmp_last_cell; ++i) {
		cells[i].add_next_cell(i + 1);
	}
}

inline void simulation::shell_diverge_update_flow(cell& cur_cell) {
	int tmp_id = cur_cell.get_id();
	int tmp_pv_cell0 = cur_cell.previous_cell[0];
	//float tmp_in_flow = Min(cells[tmp_pv_cell0].send_flow(tmp_id), receive_flow());
	float tmp_in_flow = Min(shell_send_flow(cells[tmp_pv_cell0], tmp_id), shell_receive_flow(cur_cell));
	cur_cell.set_in_flow(tmp_in_flow);
	shell_set_out_flow(cells[tmp_pv_cell0], tmp_in_flow, tmp_id);
	cur_cell.out_flow = 0.0;
}

inline void simulation::shell_origin_update_flow(cell& cur_cell) {
	cur_cell.in_flow = origin_demand[present_clock][cur_cell.get_temp_origin_demand_id()];
}

inline void simulation::shell_merge_update_flow(cell& cur_cell) {

	float sum_send_flow = 0.0, pre_send_flow, this_receive_flow, part_in_flow;
	int tmp_id = cur_cell.get_id();
	ivector<int>& temp_pv_cell = cur_cell.previous_cell;
	int tmp_previous_cell_size = temp_pv_cell.size();

	for (int i = 0; i < tmp_previous_cell_size; ++i) {
		//sum_send_flow += cells[temp_pv_cell[i]].send_flow(tmp_id);
		sum_send_flow += shell_send_flow(cells[temp_pv_cell[i]], tmp_id);
	}
	cur_cell.set_in_flow(0.0);
	for (int i = 0; i < tmp_previous_cell_size; ++i) {
		pre_send_flow = shell_send_flow(cells[temp_pv_cell[i]], tmp_id);
		this_receive_flow = pre_send_flow / sum_send_flow * shell_receive_flow(cur_cell);
		part_in_flow = Min(pre_send_flow, this_receive_flow);
		cur_cell.in_flow += part_in_flow;
		//Set flow for previous cell.
		shell_set_out_flow(cells[temp_pv_cell[i]], part_in_flow, tmp_id);
	}
}

inline void simulation::shell_destination_update_flow(cell& cur_cell) {
	int tmp_id = cur_cell.get_id();
	int tmp_pv_cell0 = cur_cell.previous_cell[0];

	cur_cell.in_flow = shell_send_flow(cells[tmp_pv_cell0]);
	cur_cell.out_flow = shell_send_flow(cur_cell);
	shell_set_out_flow(cells[tmp_pv_cell0], cur_cell.in_flow, tmp_id);
}

inline void simulation::shell_normal_update_flow(cell& cur_cell) {
	int tmp_id = cur_cell.get_id();
	int tmp_pv_cell0 = cur_cell.previous_cell[0];

	//cur_cell.in_flow = Min(cells[tmp_pv_cell0].send_flow(tmp_id), receive_flow());
	cur_cell.in_flow = Min(shell_send_flow(cells[tmp_pv_cell0], tmp_id), shell_receive_flow(cur_cell));
	//cells[tmp_pv_cell0].set_out_flow(cur_cell.in_flow, tmp_id);
	shell_set_out_flow(cells[tmp_pv_cell0], cur_cell.in_flow, tmp_id);
}

inline float simulation::shell_move_vehicle(cell& cur_cell) {
	int tmp_id = cur_cell.get_id();

	float delay = exist_vehicle[present_clock - 1][tmp_id] - cur_cell.out_flow;
	exist_vehicle[present_clock][tmp_id] = delay + cur_cell.in_flow;
	return delay;
}

inline float simulation::shell_send_flow(cell& cur_cell, int cn) {
	int tmp_id = cur_cell.get_id();
	float tmp_max_flow = cur_cell.get_max_flow();

	if (cur_cell.get_type() != diverge)
		return Min(tmp_max_flow, exist_vehicle[present_clock - 1][tmp_id]);

	int i = index_next_cell[tmp_id][cn];

	if (cur_cell.get_on_intersection() < 0)
		return Min(tmp_max_flow * cur_cell.diverge_coeff[i], diverge_flow[index_diverge_cell[tmp_id]][i]);

	else {
		if (cur_cell.at_phase[i].empty())
			return Min(tmp_max_flow * cur_cell.diverge_coeff[i], diverge_flow[index_diverge_cell[tmp_id]][i]);
		else {
			for (int j = 0; j < cur_cell.at_phase[i].top; ++j)
				if (omega[present_clock][cur_cell.get_on_intersection()][cur_cell.at_phase[i][j]])
					return Min(tmp_max_flow * cur_cell.diverge_coeff[i], diverge_flow[index_diverge_cell[tmp_id]][i]);
		}
	}
	return 0.0;
}

inline float simulation::shell_receive_flow(cell& cur_cell) {
	return Min(cur_cell.get_max_flow(),
		cur_cell.get_delta() * (cur_cell.get_max_vehicle() - exist_vehicle[present_clock - 1][cur_cell.get_id()]));
}

inline void simulation::shell_set_out_flow(cell& cur_cell, const float& out, int next_cell_id) {
	int temp_id = cur_cell.get_id();

	if (cur_cell.get_type() == diverge) {
		int i = index_next_cell[temp_id][next_cell_id];
		diverge_flow[index_diverge_cell[temp_id]][i] -= out;
		diverge_flow[index_diverge_cell[temp_id]][i] += cur_cell.in_flow * cur_cell.diverge_coeff[i];
		cur_cell.out_flow += out;
	}
	else {
		cur_cell.out_flow = out;
	}
}

inline void simulation::shell_set_max_flow(cell& cur_cell, const float& mf) {
	cur_cell.max_flow = mf * ((settings).clock_tick);
}

// input part
void simulation::skip(FILE* in) {
	//Log->process("Going to skipping line...\n");
	int key = fgetc(in);
	while (true) {
		if (key == '%') {
			while (fgetc(in) != '\n');
		}
		else if (key != ' ' && key != '\n') {
			//fputc(key,in); 
			ungetc(key, in);
			break;
		}
		key = fgetc(in);
	}
	return;
}

void simulation::input_setting(FILE* in) {
	skip(in);
	char keyword[1024];
	int hh, mm, ss, sec = 0;
	fscanf(in, "%s", keyword);
	while (strcmpi("end", keyword)) {
		if (strcmpi("time", keyword) == 0) {
			Log->process("Going to input time...");
			fscanf(in, "%d:%d:%d", &hh, &mm, &ss);
			sec = hh * 3600 + mm * 60 + ss;
			settings.start_time = sec;
			fscanf(in, "%d:%d:%d", &hh, &mm, &ss);
			sec = hh * 3600 + mm * 60 + ss;
			settings.end_time = sec;
			Log->process("Input times successfully...");
		}
		else if (strcmpi("clock", keyword) == 0) {
			Log->process("Going to input clock...");
			fscanf(in, "%d", &sec);
			settings.clock_tick = sec;
			Log->process("Input clock successfully...");
		}
		else if (strcmpi("epsilon", keyword) == 0) {
			Log->process("Going to input epsilon...");
			fscanf(in, "%f", &settings.epsilon);
			Log->process("Input epsilon successfully...");
		}
		else if (strcmpi("initoccp", keyword) == 0) {
			Log->process("Going to input initoccp...");
			fscanf(in, "%d", &settings.initial_occupation);
			Log->process("Input initoccp successfully...");
		}
		else if (strcmpi("initctrl", keyword) == 0) {
			Log->process("Going to input initctrl...");
			fscanf(in, "%d", &settings.initial_control);
			Log->process("Input initctrl successfully...");
		}
		else if (strcmpi("cellscale", keyword) == 0) {
			Log->process("Going to input cellscal...");
			fscanf(in, "%f", &settings.cell_length_factor);
			Log->process("Input cellscale successfully...");
		}
		else if (strcmpi("losttime", keyword) == 0) {
			Log->process("Going to input losttime...");
			fscanf(in, "%d", &settings.yellow_time);
			settings.yellow_ticks = ((settings.yellow_time / settings.clock_tick == 0) ?
				1 : (settings.yellow_time / settings.clock_tick));
			Log->process("Input losttime successfully...");
		}
		skip(in);
		fscanf(in, "%s", keyword);
	}
	settings.max_ticks = (int)ceil((settings.end_time - settings.start_time) * 1.0 / settings.clock_tick);
	Log->process("Input setting successfully...");
}

void simulation::input_geometry(FILE* in) {
	skip(in);
	char str[1024];
	int id, type, x, y;
	Log->process("Going to input node...");
	//memset(str,0,1024);
	if (nodes.empty()) nodes.push_back(node(this));
	while (fscanf(in, "%s", str), strcmpi(str, "arc")) {
		fscanf(in, "%d%d%d%d", &id, &type, &x, &y);
		//int tmp_node_size = node::size + 1;
		//nodes[tmp_node_size] = node( id,type,x,y );
		nodes.emplace_back(this, id, type, x, y);
		skip(in);
	}

	char sstr[256];
	Log->process("Input node successfully...");
	//memset(sstr,0,256);
	sprintf(sstr, "The size of node is: %d...", nodes.size());
	Log->process(sstr);

	int ai, anf, anl;
	float as, af, ad, adel;
	Log->process("Going to input arc...");
	if (arcs.empty()) arcs.push_back(arc());
	while (strcmpi(str, "end")) {
		fscanf(in, "%d%d%d%f%f%f%f", &ai, &anf, &anl, &as, &af, &ad, &adel);
		//int tmp_arc_size = arc::size + 1;
		//int tmp_arc_size = arcs.size();
		//arcs[tmp_arc_size] = arc( ai,anf,anl,as,af,ad,adel );
		//arcs[tmp_arc_size].create_cell();
		//arcs.emplace_back(ai, anf, anl, as, af, ad, adel);
		shell_arc_emplace(ai, anf, anl, as, af, ad, adel);
		//arcs.back().create_cell();
		shell_create_cell(arcs.back());
		skip(in);
		//memset(str,0,1024);
		fscanf(in, "%s", str);
	}
	Log->process("Input arc successfully...");
	sprintf(str, "The size of arc is: %d...", arcs.size());
	Log->process(str);
	Log->process("Input geometry successfully...");
}

void simulation::input_traffic(FILE* in) {
	char str[1024];
	skip(in);
	//demand
	int hh, mm, ss, origin_node, sec;
	float traffic_demand;

	int __count = 0;
	Log->process("Going to input demand ...");
	while (fscanf(in, "%s", str), strcmpi(str, "demand") == 0) {
		fscanf(in, "%d:%d:%d %d %f", &hh, &mm, &ss, &origin_node, &traffic_demand);
		//demands
		//cl
		sec = hh * 3600 + mm * 60 + ss;
		sprintf(Log->get_str(), "Origin Node#%03d at Arc#%03d",
			origin_node, nodes[origin_node].get_arc(), arcs[nodes[origin_node].get_arc()].get_first_cell());
		Log->process(Log->get_str());
		int id = arcs[nodes[origin_node].get_arc()].get_first_cell();

		cells[id].add_demand(sec, traffic_demand);

		skip(in);
		__count++;
	}
	char sstr[256];
	//memset(sstr,0,256);
	Log->process("Input arc successfully...");
	sprintf(sstr, "The size of demand is: %d ...", __count);
	Log->process(sstr);

	//We need some dummy cells here.

	__count = 0;
	Log->process("Going to input diverge ...");
	while (strcmpi("diverge", str) == 0) {
		int from, to, type;
		float ceoff;
		fscanf(in, "%d %d %f %d", &from, &to, &ceoff, &type);
		from = arcs[from].get_last_cell();
		to = arcs[to].get_first_cell();
		//		cells[from].set_type(diverge);
		cells[from].add_next_cell(to, ceoff, type);
		cells[from].set_type(diverge);
		cells[to].add_previous_cell(from);
		skip(in);
		//memset(str,0,1024);
		fscanf(in, "%s", str);
		__count++;
	}
	//memset(str,0,1024);
	Log->process("Input diverge successfully...");
	sprintf(sstr, "The size of diverge is: %d ...", __count);
	Log->process(sstr);

	__count = 0;
	Log->process("Going to input merge...");
	while (strcmpi("end", str)) {
		int from, to;
		fscanf(in, "%d %d", &from, &to);
		from = arcs[from].get_last_cell();
		to = arcs[to].get_first_cell();
		//		cells[to].set_type(Cell::merge);
		cells[to].add_previous_cell(from);
		cells[from].add_next_cell(to);
		skip(in);
		//memset(str,0,1024);
		fscanf(in, "%s", str);
		__count++;
	}
	//memset(str,0,1024);
	Log->process("Input merge successfully...");
	sprintf(sstr, "The size of merge is: %d ...", __count);
	Log->process(sstr);
	Log->process("Input traffic successfully...");
}

void simulation::classify_cell() {
	origin_size = normal_size = diverge_size = merge_size = destination_size = 0;
	//for( int i = 1; i <= cell::size; ++i ){
	for (int i = 1; i < cells.size(); ++i) {

		switch (cells[i].get_type()) {
		case normal: normal_set[normal_size++] = (i); break;
		case origin: origin_set[origin_size++] = (i); break;
		case destination: destination_set[destination_size++] = (i); break;
		case diverge: diverge_set[diverge_size++] = (i); break;
		case Cell::merge: merge_set[merge_size++] = (i); break;
		}
	}
}

void simulation::initial_diverge_cell_index() {
	number_diverge_cell = diverge_size;
	for (int i = 0; i < number_diverge_cell; ++i) {
		index_diverge_cell[diverge_set[i]] = i;
	}
}

void simulation::initial_origin_demand() {
	for (int i = 0; i < origin_size; ++i) {
		int n = cells[origin_set[i]].get_num_demand();
		int cell_id = origin_set[i], pre_clock = 0, end_clock = 0;
		int idemand = cells[cell_id].get_temp_origin_demand_id();
		sort(temp_origin_demand[idemand], temp_origin_demand[idemand] + n);
		float pre_flow = 0.0;
		demand* iter;
		for (int jd = 0; jd < n; jd++) {
			iter = &temp_origin_demand[idemand][jd];
			end_clock = (iter->clock - settings.start_time) / settings.clock_tick
				+ ((((iter->clock - settings.start_time) % settings.clock_tick) == 0) ? 0 : 1);

			while (pre_clock <= end_clock)
				origin_demand[pre_clock++][idemand] = pre_flow;

			pre_flow = iter->traffic * settings.clock_tick;
		}
		while (pre_clock <= settings.get_max_ticks())
			origin_demand[pre_clock++][idemand] = pre_flow;
	}
}

void simulation::input_intersection(FILE* in) {
	int id, type, x, y, min_g = 0, max_g = 0, right_t = 0, num_phase = 0;

	Log->process("Going to input intersection.....*");
	fscanf(in, "%d%d%d%d", &id, &type, &x, &y);
	char str[256];
	if (type) {
		fscanf(in, "%d%d%d%d", &min_g, &max_g, &right_t, &num_phase);
		//int tmp_intersection_size = intersections.size() + 1;
		//intersections[tmp_intersection_size] = intersection( id,type,x,y,min_g,max_g,right_t,num_phase );
		intersections.emplace_back(this, id, type, x, y, min_g, max_g, right_t, num_phase);

		while (true) {
			memset(str, 0, 256);
			while ((str[0] = fgetc(in)) == ' ');
			if (str[0] == '\n') break;

			id = 0;
			while (str[id] != ' ' && str[id] != '\n') str[++id] = fgetc(in);
			int i;
			sscanf(str, "%d", &i);
			Assert(i >= 0);
			//intersections[tmp_intersection_size].add_node(i);
			intersections.back().add_node(i);
			if (str[id] == '\n') break;
		}
	}
	else {
		//int tmp_intersection_size = intersection::size + 1;
		//intersections[tmp_intersection_size] = intersection( id,type,x,y );
		intersections.emplace_back(this, id, type, x, y);
	}
	Log->process("Input intersection successfully...");
}

void simulation::input_phase(FILE* in) {
	Log->process("Going to input phase...");
	int id_inter, id_phase, from_arc, to_arc;
	fscanf(in, "%d%d%d%d", &id_inter, &id_phase, &from_arc, &to_arc);
	id_inter--;
	intersections[id_inter].add_phase(id_phase, from_arc, to_arc);

	int from_cell_id = arcs[from_arc].get_last_cell();
	int to_cell_id = arcs[to_arc].get_first_cell();
	cells[from_cell_id].add_at_phase(to_cell_id, id_phase - 1);
	cells[from_cell_id].set_on_intersection(id_inter);
}

void simulation::input_control(FILE* in) {
	skip(in);
	char str[1024];
	Log->process("Going to input control...");
	while (true) {
		fscanf(in, "%s", str);
		if (strcmpi("intersection", str) == 0) {
			input_intersection(in);
		}
		else if (strcmpi("phase", str) == 0) {
			input_phase(in);
		}
		else break;
		skip(in);
	}
	//Log->process("Input control successfully...");
	sprintf(str, "The size of intersection is: %d ...", intersections.size());
	Log->process(str);
	Log->process("Input control successfully...");
}

void simulation::input_event(FILE* in) {
	skip(in);
	char str[1024];
	Log->process("Going to input incident...");
	while (true) {
		int at_arc;
		float dist;
		int sy, sm, ss, ey, em, es;
		float mf;
		fscanf(in, "%s", str);
		if (strcmpi("event", str)) break;
		fscanf(in, "%d %f %d:%d:%d %d:%d:%d %f", &at_arc, &dist, &sy, &sm, &ss, &ey, &em, &es, &mf);
		int st = sy * 3600 + sm * 60 + ss;
		int et = ey * 3600 + em * 60 + es;
		int tmp_incident_size = incident::size + 1;
		//incidents[tmp_incident_size] = incident(at_arc, dist, st, et, mf);
		skip(in);
	}
	Log->process("Input incident successfully...");
	sprintf(str, "The size of incident is: %d", incident::size);
	Log->process(str);
	Log->process("Input event successfully...");
}

void simulation::scanfile(char namestr[]) {
	char full_name[256];
	strcpy(full_name, namestr);
	strcat(full_name, ".txt");
	FILE* in = fopen(full_name, "r");
	if (in == NULL) {
		Log = new debug(namestr);
		char str[256];
		sprintf(str, "Cannot open input file: \t%s\n", namestr);
		Log->throws(str);
		exit(0);
	}
	else {
		Log = new debug(namestr);
	}
	temp_origin_demand_size = 0;
	char line[1024], * pstr;
	memset(index_diverge_cell, 0, sizeof(index_diverge_cell));
	while (!feof(in)) {
		fgets(line, 1023, in);
		pstr = line;
		while (*pstr == ' ') pstr++;
		if (*pstr == '\0') continue;
		if (*pstr == '%') continue;
		if (strnicmp("setting", pstr, 7) == 0) {
			Log->process("input setting...");
			input_setting(in);
		}
		else if (strnicmp("geometry", pstr, 8) == 0) {
			Log->process("input geometry...");
			input_geometry(in);
		}
		else if (strnicmp("traffic", pstr, 7) == 0) {
			Log->process("input traffic...");
			input_traffic(in);
			classify_cell();
			initial_origin_demand();
			initial_diverge_cell_index();
		}
		else if (strnicmp("control", pstr, 7) == 0) {
			Log->process("input control...");
			input_control(in);
		}
		else if (strnicmp("event", pstr, 5) == 0) {
			Log->process("input event...");
			input_event(in);
		}
	}

	fclose(in);


}

// initialize part
void simulation::initial_diverge_flow() {
	for (int i = 0; i < number_diverge_cell; ++i) {
		for (int j = 0; j < MAX_ADJ_CELL; ++j) diverge_flow[i][j] = 0.0;
	}
}

void simulation::initial_occupation(float* vehicle) {
	//for( int i = 1; i <= cell::size; ++i ){
	for (int i = 1; i < cells.size(); ++i) {
		vehicle[i] = settings.initial_occupation / 100.0 * cells[i].get_max_vehicle();
	}
}

void simulation::initial_control() {
	memset(omega, false, sizeof(omega));
	char str[256], _str[256];
	int istr = 0;

	for (int i = 0; i < intersections.size(); ++i) {
		int min_green = (int)ceil(intersections[i].get_min_green() * 1.0 / settings.clock_tick);
		int g = 0, p = 0;
		for (int j = 1; j <= settings.get_max_ticks(); j++) {
			omega[j][i][p] = true;
			if (++g == min_green) {
				j += settings.yellow_ticks;
				g = 0;
				p = ++p % intersections[i].get_num_phases();
			}
		}
	}

	//Output initial timing plan.
	Log->process("Initial Timing Plan...");
	for (int i = 0; i < intersections.size(); ++i) {
		sprintf(_str, "Intersection %03d", i);
		Log->process(_str);
		for (int j = 1; j <= settings.get_max_ticks(); ++j) {
			memset(str, 0, sizeof(str));
			istr = 0;
			sprintf(str, "#%05d ", j);
			istr = strlen(str);
			for (int k = 0; k < intersections[i].get_num_phases(); ++k) {
				sprintf(str + istr, "%d ", omega[j][i][k] ? 1 : 0);
				istr += 2;
			}
			Log->process(str);
		}
	}

}

// output part
void simulation::printoccup(char namestr[], float delay) {
	FILE* out;
	char outputname[256];
	//strcat(outputname,&namestr[2]);
	//strcat(outputname,".txt");
	sprintf(outputname, "o_%s.txt", &namestr[2]);
	out = fopen(outputname, "w");

	fputs("Green Signal Cell Occupation Output File.\n", out);
	fputs("----------------------------------------\n", out);
	fprintf(out, "Output File: \t%s\n", outputname);
	time_t run;
	time(&run);
	localtime(&run);
	fprintf(out, "The Date: \t%s", ctime(&run));
	fprintf(out, "Total delay: \t %8.4lf vehi*sec.\n", delay);
	fprintf(out, "~\n\n");
	int i, j, k;

	fprintf(out, "%d nodes, %d arcs, %d intersections, %d ticks\n\n",
		nodes.size(), arcs.size(), intersections.size(), settings.get_max_ticks());

	//Output arcs and cells.
	coordinate first, last, interval;
	int first_id, last_id;
	//for( i = 1; i <= arc::size; ++i ){
	for (i = 1; i < arcs.size(); ++i) {
		first_id = arcs[i].get_up_node();
		last_id = arcs[i].get_down_node();
		first = nodes[first_id].get_pos();
		last = nodes[last_id].get_pos();
		/*interval.x = (last.x - first.x)/(arcs[i].get_num_cell()+1);
		interval.y = (last.y - first.y)/(arcs[i].get_num_cell()+1);*/

		fprintf(out, "arc %d\n", i);
		fprintf(out, "upnode %d (%d, %d)\ndownnode %d (%d, %d)\n", first_id, first.x, first.y, last_id, last.x, last.y);
		fprintf(out, "%d cells  %f cell length\n", arcs[i].get_num_cell(), arcs[i].get_cell_length());
		//Output cell coordinates.		
		/*for( j=1, k = arcs[i].get_first_cell(); k <=arcs[i].get_last_cell(); k++, j++){
			fprintf( out,"cell %d, coordinate (%d, %d), length %.0lf\n",k, first.x + j*interval.x, first.y + j*interval.y, cells[k].get_length() );
		}*/
		//Output cell states.
		for (j = 0; j <= settings.get_max_ticks(); ++j) {
			fprintf(out, "#%04d", j);
			for (k = arcs[i].get_first_cell(); k <= arcs[i].get_last_cell(); k++) {
				fprintf(out, "\t%9.4lf", exist_vehicle[j][k]);
			}
			fprintf(out, "\n");
		}
		fprintf(out, "\n");
	}

	//Output intersections.
	coordinate pos;
	for (i = 0; i < intersections.size(); ++i) {
		fprintf(out, "intersection %d\n", i + 1);
		pos = intersections[i].get_pos();
		fprintf(out, "coordinate (%d, %d)\n", pos.x, pos.y);
		//Output phases.
		//for( j = 0, j < intersections[i].get_num_phases(), j++){
		//	intersections[i].phases
		//}
	}


	fclose(out);
}

void simulation::printdelay(char namestr[]) {
	FILE* out;
	char outputname[256];
	sprintf(outputname, "d_%s.txt", &namestr[2]);
	out = fopen(outputname, "w");

	fputs("Green Signal Cell Occupation Output File.\n", out);
	fputs("----------------------------------------\n", out);
	fprintf(out, "Output File: \t%s\n", outputname);
	time_t run;
	time(&run);
	localtime(&run);
	fprintf(out, "The Date: \t%s", ctime(&run));
	//fprintf( out,"Total delay: \t %7.2lf vehi*sec.\n", delay );
	fprintf(out, "~\n\n");
	double temp = 0.0;
	fprintf(out, "%d ticks\n\n", settings.max_ticks);
	fprintf(out, "#%04d\t%f\n", 0, 0.0);
	//initial_diverge_flow();
	for (int it = 1; it <= settings.max_ticks; ++it) {
		//fprintf( out,"#%04d\t%f\n",it,simulate(it-1,it) );
		fprintf(out, "#%04d\t%f\n", it, delay_record[it - 1]);
	}
	fclose(out);
}

void simulation::printplan(char namestr[]) {
	FILE* out;
	char outputname[256];
	sprintf(outputname, "t_%s.txt", &namestr[2]);
	out = fopen(outputname, "w");

	fputs("Green Signal Time Planning File.\n", out);
	fputs("----------------------------------------\n", out);
	fprintf(out, "Output File: \t%s\n", outputname);

	time_t run;
	time(&run);
	localtime(&run);
	fprintf(out, "The Date: \t%s", ctime(&run));
	fprintf(out, "~\n\n");
	fprintf(out, "%d intersections, %d ticks\n\n", intersections.size(), settings.get_max_ticks());
	for (int i = 0; i < intersections.size(); ++i) {
		fprintf(out, "intersection %d\n", i + 1);
		intersections[i].print_phases(out);
		/*fprintf( out,"%d phases\n",intersections[i].get_num_phases() );
		int count_id = 1;
		for( phase* iter = intersections[i].get_phases_begin();
			iter != intersections[i].get_phases_end(); ++iter ){
				fprintf( out,"Phase %d (%d)",count_id++,(iter->from_arc.size()) );
			for( int ii = 0; ii < (iter->from_arc.size()); ++ii ){
				fprintf( out,"\tfrom %d to %d",arcs[iter->from_arc[ii]].get_down_node(),
					arcs[iter->to_arc[ii]].get_up_node() );
			}
			fprintf( out,"\n" );
		}*/
		fprintf(out, "\n");
		fprintf(out, "%%No.  ");
		for (int ip = 1; ip <= intersections[i].get_num_phases(); ++ip) {
			fprintf(out, " (%d)", ip);
		}
		fprintf(out, "\n");
		for (int it = 1; it <= settings.get_max_ticks(); ++it) {
			fprintf(out, "@%05d", it);
			for (int ip = 0; ip < intersections[i].get_num_phases(); ++ip) {
				fprintf(out, "%4d", omega[it][i][ip] ? 1 : 0);
			}
			fprintf(out, "\n");
		}
		fputs("", out);
	}
	fclose(out);
}

// update part
void simulation::modify_control(const int& I, const int& t0, const int& t1, const int& P) {
	for (int i = t0; i <= t1; i++) {
		omega[i][I][P] = true;
		for (int j = 0; j < intersections[I].get_num_phases(); ++j) {
			if (j == P) continue;
			omega[i][I][j] = false;
		}
	}
}

void simulation::set_control_off(const int& I, const int& st, const int& et)
{
	for (int it = st; it <= et; ++it)
	{
		for (int ip = 0; ip < intersections[I].get_num_phases(); ++ip)
			omega[it][I][ip] = false;
	}
}

void simulation::update_flow() {
	//cell *cl = cells;
	std::vector<cell>& cl = cells;

	for (int i = 0; i < origin_size; ++i)
		shell_origin_update_flow(cl[origin_set[i]]);
		//cl[origin_set[i]].origin_update_flow();
	
	for (int i = 0; i < diverge_size; ++i)
		shell_diverge_update_flow(cl[diverge_set[i]]);
		//cl[diverge_set[i]].diverge_update_flow();
	
	for (int i = 0; i < merge_size; ++i)
		shell_merge_update_flow(cl[merge_set[i]]);
		//cl[merge_set[i]].merge_update_flow();

	for (int i = 0; i < normal_size; ++i)
		shell_normal_update_flow(cl[normal_set[i]]);
		//cl[normal_set[i]].normal_update_flow();

	for (int i = 0; i < destination_size; ++i)
		shell_destination_update_flow(cl[destination_set[i]]);
		//cl[destination_set[i]].destination_update_flow();

}

void simulation::update_event() {
	for (int i = 1; i <= incident::size; ++i) {
		incidents[i].occur();
	}
}

float simulation::update_occupation() {
	float delay = 0.0;
	//for (int i = 1; i <= cell::size; ++i) {
	for (int i = 1; i < cells.size(); ++i)
		if (cells[i].get_type() != destination && cells[i].get_type() != origin)
			//delay += cells[i].move_vehicle();
			delay += shell_move_vehicle(cells[i]);
	return delay;
}

float simulation::simulate(int st, int et) {
	float delay = 0.0f;
	float temp = 0.0f;
	float pre_delay = 0.0f;

	//Start simulation.
	for (int it = st; it <= et; ++it) {

		present_clock = it;
		for (int i = 1; i <= incident::size; ++i) {
			incidents[i].occur();
		}
		update_flow();
		//for( int i = 1; i <= cell::size; ++i ){
		for (int i = 1; i < cells.size(); ++i) {
			temp = exist_vehicle[it - 1][i] - cells[i].out_flow;
			exist_vehicle[it][i] = temp + cells[i].in_flow;
			if (cells[i].get_type() != destination && cells[i].get_type() != origin)
				delay += temp;
		}
		delay_record[it] = delay - pre_delay;
		pre_delay = delay;
	}

	return delay * settings.clock_tick;
}
