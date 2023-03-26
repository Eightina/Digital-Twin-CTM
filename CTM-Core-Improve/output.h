#ifndef _output_H

#define _output_H

#include "arc.h"
#include "cell.h"
#include "intersection.h"
#include "coordinate.h"
#include "base.h"
#include "ivector.h"
#include "update.h"
#include "initialize.h"
// #include "localopt.h"

extern float exist_vehicle[][MAX_CELL];
extern std::vector<intersection> intersections;
extern std::vector<cell> cells;
extern setting settings;
extern std::vector<arc> arcs;
extern float delay_record[MAX_CLOCK];

void printoccup(char namestr[], float delay){
	FILE *out;
	char outputname[256];
	//strcat(outputname,&namestr[2]);
	//strcat(outputname,".txt");
	sprintf(outputname, "o_%s.txt", &namestr[2]);
	out = fopen( outputname, "w" );

	fputs("Green Signal Cell Occupation Output File.\n",out);
	fputs("----------------------------------------\n",out);
	fprintf( out,"Output File: \t%s\n",outputname );
	time_t run;
	time(&run);
	localtime(&run);
	fprintf( out,"The Date: \t%s",ctime(&run) );
	fprintf( out,"Total delay: \t %8.4lf vehi*sec.\n", delay );
	fprintf( out,"~\n\n" );
	int i,j,k;

	fprintf( out,"%d nodes, %d arcs, %d intersections, %d ticks\n\n",
		node::size, arcs.size(), intersections.size(), settings.get_max_ticks());
	
	//Output arcs and cells.
	coordinate first,last,interval;
	int first_id,last_id;
	//for( i = 1; i <= arc::size; ++i ){
	for (i = 1; i < arcs.size(); ++i) {
		first_id = arcs[i].get_up_node();
		last_id = arcs[i].get_down_node();
		first = nodes[first_id].get_pos();
		last = nodes[last_id].get_pos();
		/*interval.x = (last.x - first.x)/(arcs[i].get_num_cell()+1);
		interval.y = (last.y - first.y)/(arcs[i].get_num_cell()+1);*/
		
		fprintf( out,"arc %d\n",i );
		fprintf( out,"upnode %d (%d, %d)\ndownnode %d (%d, %d)\n",first_id,first.x,first.y,last_id,last.x,last.y);
		fprintf( out,"%d cells  %f cell length\n",arcs[i].get_num_cell(),arcs[i].get_cell_length() );
		//Output cell coordinates.		
		/*for( j=1, k = arcs[i].get_first_cell(); k <=arcs[i].get_last_cell(); k++, j++){
			fprintf( out,"cell %d, coordinate (%d, %d), length %.0lf\n",k, first.x + j*interval.x, first.y + j*interval.y, cells[k].get_length() );
		}*/
		//Output cell states.
		for( j = 0; j <= settings.get_max_ticks(); ++j ){
			fprintf( out,"#%04d",j );
			for( k = arcs[i].get_first_cell(); k <=arcs[i].get_last_cell(); k++){
				fprintf( out,"\t%9.4lf",exist_vehicle[j][k] );
			}
			fprintf( out,"\n" );
		}
		fprintf( out,"\n" );
	}

	//Output intersections.
	coordinate pos;
	for( i = 0; i < intersections.size(); ++i ){
		fprintf( out,"intersection %d\n",i + 1 );
		pos = intersections[i].get_pos();
		fprintf( out,"coordinate (%d, %d)\n",pos.x,pos.y );
		//Output phases.
		//for( j = 0, j < intersections[i].get_num_phases(), j++){
		//	intersections[i].phases
		//}
	}


	fclose(out);
}


void printdelay( char namestr[] ){
	FILE *out;
	char outputname[256];
	sprintf( outputname,"d_%s.txt",&namestr[2]);
	out = fopen( outputname,"w" );

	fputs("Green Signal Cell Occupation Output File.\n",out);
	fputs("----------------------------------------\n",out);
	fprintf( out,"Output File: \t%s\n",outputname );
	time_t run;
	time(&run);
	localtime(&run);
	fprintf( out,"The Date: \t%s",ctime(&run) );
	//fprintf( out,"Total delay: \t %7.2lf vehi*sec.\n", delay );
	fprintf( out,"~\n\n" );
	double temp = 0.0;
	fprintf( out,"%d ticks\n\n",settings.max_ticks );
	fprintf( out,"#%04d\t%f\n",0,0.0 );
	//initial_diverge_flow();
	for( int it = 1; it <= settings.max_ticks; ++it ){
		//fprintf( out,"#%04d\t%f\n",it,simulate(it-1,it) );
		fprintf( out, "#%04d\t%f\n", it, delay_record[it - 1]);
	}
	fclose(out);
}

void printplan( char namestr[] ){
	FILE *out;
	char outputname[256];
	sprintf( outputname,"t_%s.txt",&namestr[2] );
	out = fopen( outputname,"w" );

	fputs("Green Signal Time Planning File.\n",out);
	fputs("----------------------------------------\n",out);
	fprintf( out,"Output File: \t%s\n",outputname );

	time_t run;
	time(&run);
	localtime(&run);
	fprintf( out,"The Date: \t%s",ctime(&run) );
	fprintf( out,"~\n\n" );
	fprintf( out,"%d intersections, %d ticks\n\n", intersections.size(),settings.get_max_ticks() );
	for( int i = 0; i < intersections.size(); ++i ){
		fprintf( out,"intersection %d\n",i + 1 );
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
		fprintf( out,"\n" );
		fprintf( out,"%%No.  " );
		for( int ip = 1; ip <= intersections[i].get_num_phases(); ++ip ){
			fprintf( out," (%d)",ip );
		}
		fprintf(out,"\n");
		for( int it = 1; it <= settings.get_max_ticks(); ++it ){
			fprintf(out,"@%05d",it );
			for( int ip = 0; ip < intersections[i].get_num_phases(); ++ip ){
				fprintf( out,"%4d",omega[it][i][ip]?1:0 );
			}
			fprintf( out,"\n" );
		}
		fputs( "",out );
	}
	fclose(out);
}


#endif

