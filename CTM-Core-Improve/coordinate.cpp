#include "coordinate.h"
#include <math.h>

coordinate::coordinate():x(0),y(0){}
coordinate::coordinate( int _x,int _y):x(_x),y(_y){}
float coordinate::dist( coordinate a ){
		return sqrt( (float)(x-a.x)*(x-a.x) + (y-a.y)*(y-a.y) );
	}

