#ifndef _initialize_H

#define _initialize_H

void initial_diverge_flow(){
	for( int i = 0; i < number_diverge_cell; ++i ){
		for( int  j = 0; j < MAX_ADJ_CELL; ++j ) diverge_flow[i][j] = 0.0;
	}
}

void initial_occupation( float *vehicle ){
	//for( int i = 1; i <= cell::size; ++i ){
	for( int i = 1; i < cells.size(); ++i) {
		vehicle[i] = settings.initial_occupation/100.0*cells[i].get_max_vehicle();
	}
}

//without yellow light
////void initial_control(){
////
////	memset(omega,false,sizeof(omega) );
////	char str[256],_str[256];
////	int istr = 0;
////	for( int i = 1; i <= intersection::size; ++i ){
////		int min_green = (int)ceil(intersections[i].get_min_green()*1.0/settings.clock_tick);
////		int g=0,p=0;
////		for( int j = 1; j <= settings.get_max_ticks(); j++ ){
////			omega[j][i][p] = true;
////			if(++g == min_green){
////				g = 0;
////				p = ++p % intersections[i].get_num_phases();
////			}
////		}
////	}
////
////	//Output initial timing plan.
////	Log->process("Initial Timing Plan...");
////	for( int i = 1; i <= intersection::size; ++i ){
////		sprintf(_str,"Intersection %03d",i);
////		Log->process(_str);
////		for( int j = 1; j <= settings.get_max_ticks(); ++j ){
////			memset( str,0,sizeof(str) );
////			istr = 0;
////			sprintf( str,"#%05d ",j );
////			istr = strlen(str);
////			for( int k = 0; k < intersections[i].get_num_phases(); ++k ){
////				sprintf( str+istr,"%d ",omega[j][i][k]?1:0 );
////				istr+=2;
////			}
////			Log->process(str);
////		}
////	}
////}

/*
	initial the omega
*/

void initial_control(){

	memset(omega,false,sizeof(omega) );
	char str[256],_str[256];
	int istr = 0;
	
	for( int i = 0; i < intersections.size(); ++i) {
		int min_green = (int)ceil(intersections[i].get_min_green()*1.0/settings.clock_tick);
		int g=0,p=0;
		for( int j = 1; j <= settings.get_max_ticks(); j++ ){
			omega[j][i][p] = true;
			if(++g == min_green){
				j += settings.yellow_ticks;
				g = 0;
				p = ++p % intersections[i].get_num_phases();
			}
		}
	}

	//Output initial timing plan.
	Log->process("Initial Timing Plan...");
	for( int i = 0; i < intersections.size(); ++i ){
		sprintf(_str,"Intersection %03d",i);
		Log->process(_str);
		for( int j = 1; j <= settings.get_max_ticks(); ++j ){
			memset( str,0,sizeof(str) );
			istr = 0;
			sprintf( str,"#%05d ",j );
			istr = strlen(str);
			for( int k = 0; k < intersections[i].get_num_phases(); ++k ){
				sprintf( str+istr,"%d ",omega[j][i][k]?1:0 );
				istr+=2;
			}
			Log->process(str);
		}
	}
}

#endif
