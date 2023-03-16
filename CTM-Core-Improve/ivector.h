#ifndef _ivector_H
#define _ivector_H

const int MAX_SIZE=32;
template <typename T>
struct ivector{
	ivector():top(0){}
	void push_back( T key ){ a[top] = key; ++top; }
	void pop_back(){ top--; }
	bool empty(){ return top <= 0; }
	int size(){ return top; }
	T& operator[]( int i ){ return a[i]; }
	void clear(){ top = 0; }
	int top;
	T a[MAX_SIZE];
};

#endif
