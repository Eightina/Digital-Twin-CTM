#ifndef _netopt_H

#include "localopt.h"
#include "base.h"
#include "setting.h"
#include "intersection.h"
#include <cstdlib>
using namespace std;

extern intersection intersections[MAX_INTERSECTION];

float netopt(){
	float pre_delay = 0.0,total_delay = 0.0;
	int iterative = 0, i;

	do{
		printf("  Iterative #%d\n",++iterative);
		pre_delay = total_delay;
		for( i = 1; i <= intersection::size; ++i )
		{	
			total_delay = localopt(i);
			printf( "  Intersection #%d, total delay = %.2f\n", i, total_delay );
		}
		printf("\n");
	}while( fabs(pre_delay - total_delay) > settings.epsilon );

	return total_delay;
}


#endif

