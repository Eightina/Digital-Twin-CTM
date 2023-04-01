#include "node.h"
#include "debug.h"
#include "simulation.h"

//extern debug *Log;


node::node(simulation* nodeowner) :
	owner(nodeowner), id(0), type(0), x(0), y(0), pos(x, y) {}
node::node(simulation* nodeowner, int i, int t, int _x, int _y, int ia) :
	owner(nodeowner), id(i), type(t), x(_x), y(_y), pos(x, y), adj_arc(ia) {
	//size++;
}


//int node::size = 0;
