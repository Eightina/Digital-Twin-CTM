#ifndef _demand_H
#define _demand_H

class demand {
friend bool operator < (const demand& first, const demand& second){
	return first.clock < second.clock;
}
public:
	int clock;
	float traffic;
	demand();
	demand( int cl, float tra );
	
};


#endif

