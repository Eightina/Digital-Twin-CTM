#ifndef _iqueue_H

#define _iqueue_H

#include <vector>
#include <algorithm>
using namespace std;

extern const int MAX_QUEUE_SIZE;

template <typename T, typename Container = vector<T> >
class iqueue {
	public:
		typedef typename Container::value_type value_type;
		typedef typename Container::size_type  size_type;
		typedef          Container             container_type;

	protected:
		Container c;

	public:

	explicit iqueue( const Container& cont = Container()): c(cont) {
		make_heap(c.begin(),c.end());
	}

	void push(const value_type& x) {
		if(c.size() > MAX_QUEUE_SIZE)
			c.pop_back();
		c.push_back(x);
		push_heap(c.begin(),c.end());
	}

	void pop() {
		pop_heap(c.begin(),c.end());
		c.pop_back();
	}

	bool              empty() const { return c.empty(); }
	size_type         size() const  { return c.size(); }
	const value_type& top() const   { return c.front(); }
};

#endif
