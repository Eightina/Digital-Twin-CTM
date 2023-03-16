#ifndef _base_H

#define _base_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <cctype>
#include <climits>
//#include <set>
//#include <map>
//#include <list>
//#include <stack>
//#include <deque>
//#include <queue>
#include <string>
#include <complex>
#include <bitset>
#include <iomanip>
#include <numeric>
#include <utility>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cassert>

#define Min(x,y) (x)>(y)?(y):(x)
#define Max(x,y) (x)>(y)?(x):(y)
#define Assert(expr) assert(expr);

const int MAX_NODE			=	128;
const int MAX_ARC			=	512;
const int MAX_INTERSECTION	=	64;
const int MAX_CELL			=	2048;
const int MAX_ADJ_CELL		=	8;
const int MAX_CLOCK			=	7200;	// 2 hours
const int MAX_PHASE			=	10;
const int MAX_INCIDENT		=	128;
const int MAX_ORIGIN_CELL	=	MAX_NODE;
const int MAX_DIVERGE_CELL	=	MAX_INTERSECTION*MAX_ADJ_CELL;
const int MAX_MERGE_CELL	=	MAX_DIVERGE_CELL;
const int MAX_DESTINATION_CELL = MAX_ORIGIN_CELL;
const int MAX_NORMAL_CELL	=	MAX_CELL;

const float MAX_DELAY		=	1e16;
const int MAX_PATH_SIZE		=	32;
const int MAX_QUEUE_SIZE	=	10000;

#endif

