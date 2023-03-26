
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
#include "input.h"
#include "incident.h"
#include "debug.h"
#include "output.h"
// #include "nodeopt.h"
// #include "localopt.h"
// #include "netopt.h"
#include "initialize.h"

//using namespace std;


//int present_clock;
//setting settings;
////node nodes[MAX_NODE];
//std::vector<node> nodes;
////arc arcs[MAX_ARC];
//std::vector<arc> arcs;
////cell cells[MAX_CELL]; 
//std::vector<cell> cells; // len is number of cells + 1
////intersection intersections[MAX_INTERSECTION];
//std::vector<intersection> intersections; // len is number of intersections
//incident incidents[MAX_INCIDENT];
//
//short index_next_cell[MAX_CELL][MAX_CELL];
//
//int origin_set[MAX_ORIGIN_CELL], normal_set[MAX_NORMAL_CELL],
//	diverge_set[MAX_DIVERGE_CELL], merge_set[MAX_MERGE_CELL], destination_set[MAX_DESTINATION_CELL];
//
//int origin_size = 0, normal_size = 0, diverge_size = 0, merge_size = 0, destination_size = 0;
//
//float exist_vehicle[MAX_CLOCK][MAX_CELL];
//float diverge_flow[MAX_DIVERGE_CELL][MAX_ADJ_CELL];
//bool omega[MAX_CLOCK][MAX_INTERSECTION][MAX_PHASE];		//w -- timing plan
//
//int index_diverge_cell[MAX_CELL];
//int number_diverge_cell;
//
//
//float origin_demand[MAX_CLOCK][MAX_ORIGIN_CELL];
//demand temp_origin_demand[MAX_ORIGIN_CELL][MAX_CLOCK]; // if demand not changed then remain the same
//int temp_origin_demand_size = 0;
//float delay_record[MAX_CLOCK];
//
//debug *Log;
//
//float vehicle[MAX_CELL];

void print_start(){

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

int main( int argc,char *argv[] ){

	char inputname[256] = "i_Test";
	float delay1, delay2, delay0;
	int start;

	print_start();

	if( argc != 2 ){
		puts( "Please input the name of the file:" );
		gets_s( inputname );
	}
	else
		strcpy( inputname, argv[1] );

	//input file
	scanfile( inputname );
	
	printf("\n\nRESULTS\n\n");

	//simulation
	start = clock();
	initial_control();
	initial_occupation( exist_vehicle[0] );
	initial_diverge_flow();
	delay0 = simulate( 1, settings.get_max_ticks() );
	
	//print results
	printf( "Total delay (before optimization): %7.2lf veh*sec.\n", delay0 );
	printf( "Running time: %.3f seconds.\n\n", double(clock() - start) / CLOCKS_PER_SEC);
	
	//output files
	//printoccup(inputname, delay2);
	printoccup(inputname, delay0);
	printplan(inputname);
	printdelay(inputname);
	printf( "Timing plan output files\n >>t_%s.txt\n", &inputname[2] );
	printf( "Cell occupation output files\n >>o_%s.txt\n", &inputname[2] );
	printf( "Detail delay output files\n >>d_%s.txt\n", &inputname[2] );
	printf( "\nProgram Terminated.\n\n");

	delete Log;
	system("pause");
	return 0;
}
