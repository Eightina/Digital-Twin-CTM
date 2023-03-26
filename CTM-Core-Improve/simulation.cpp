#include "simulation.h"



simulation::simulation(char* inputname) {
	std::cout << "simulation object" << inputname << std::endl;
	print_start();
	scanfile(inputname);
	start = clock();
	initial_control();


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


}

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


