// test.cpp
#include "serialization/serialization.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

using namespace std;

void test_pair_serialization();
void test_triad_serialization();
void test_cell_serialization();
void test_dimension_serialization();
void test_pair_cell_serialization();
void test_crosslist_serialization();

int main(int argc, char* argv[]) {
	test_pair_serialization();
	test_triad_serialization();
	test_cell_serialization();
	test_dimension_serialization();
	test_pair_cell_serialization();
    test_crosslist_serialization();
    system("pause");
	return 0;
}

void test_pair_serialization() {
	using namespace serialization;
	pair<int, int> val;
	stringstream ss, so;
	ss << " ( " << 2 << " " << 5 << " ) ";
	ss >> val;
	assert(2 == val.first);
	assert(5 == val.second);
	
	ss << val;
	istream_iterator<pair<int, int> > is_iter(ss), is_end;  // compile error: rror C2678: binary '>>' : no operator found which takes a left-hand operand of type 'std::basic_istream<_Elem,_Traits>' (or there is no acceptable conversion)
	ostream_iterator<pair<int, int> > os_iter(so);
	copy(is_iter, is_end, os_iter);
	so >> val;
	assert(2 == val.first);
	assert(5 == val.second);

	cout << "pass std::pair<int, int>: " << val << endl;
}

void test_triad_serialization() {
	serialization::Triad<int,int,int> val;
	stringstream ss, so;
	ss << " ( " << 2 << " " << 5 << " " << 8 << " ) ";
	ss >> val;
	assert(2 == val.first);
	assert(5 == val.second);
	assert(8 == val.third);

	ss << val;
	istream_iterator<serialization::Triad<int,int,int> > is_iter(ss), is_end;
	ostream_iterator<serialization::Triad<int,int,int> > os_iter(so, "\n");
	copy(is_iter, is_end, os_iter);
	so >> val;
	assert(2 == val.first);
	assert(5 == val.second);
	assert(8 == val.third);

	cout << "pass Triad<int,int,int>: " << val << endl;
}

void test_cell_serialization() {
	namespace sm = serialization::sparsematrix;
	sm::Cell<int> val;
	stringstream ss, so;
	ss << " ( " << 2 << " " << 5 << " " << 8 << " ) ";
	ss >> val;
	assert(2 == val.row);
	assert(5 == val.column);
	assert(8 == val.value);

	ss << val;
	istream_iterator<sm::Cell<int> > is_iter(ss), is_end;
	ostream_iterator<sm::Cell<int> > os_iter(so, "\n");
	copy(is_iter, is_end, os_iter);
	so >> val;
	assert(2 == val.row);
	assert(5 == val.column);
	assert(8 == val.value);

	cout << "pass Cell<int>: " << val << endl;
}

void test_dimension_serialization() {
	namespace sm = serialization::sparsematrix;
	sm::Dimension val;
	stringstream ss, so;
	ss << " [ " << 3 << " " << 6 << " ] ";
	ss >> val;
	assert(3 == val.row);
	assert(6 == val.column);

	ss << val;
	istream_iterator<sm::Dimension> is_iter(ss), is_end;
	ostream_iterator<sm::Dimension> os_iter(so, "\n");
	copy(is_iter, is_end, os_iter);
	so >> val;
	assert(3 == val.row);
	assert(6 == val.column);

	cout << "pass Dimension: " << val << endl;
}

void test_pair_cell_serialization() {
	namespace sm = serialization::sparsematrix;
	sm::Cell<pair<int, int> > val;
	stringstream ss, so;
	ss << " ( 2 5 ( 7 8) ) ";
	ss >> val;
	assert(2 == val.row);
	assert(5 == val.column);
	assert(make_pair(7,8) == val.value);

	ss << val;
	istream_iterator<sm::Cell<pair<int, int> > > is_iter(ss), is_end;
	ostream_iterator<sm::Cell<pair<int, int> > > os_iter(so, "\n");
	copy(is_iter, is_end, os_iter);
	so >> val;
	assert(2 == val.row);
	assert(5 == val.column);
	assert(make_pair(7,8) == val.value);

	cout << "pass Cell<int>: " << val << endl;
}

void test_crosslist_serialization() {
    using namespace serialization;
    CrossList<int> c;
	stringstream ss, so;
	ss << " ( 0 0 1 )  ( 0 2 2 ) \n";
	ss << " ( 1 1 3 )  ( 1 3 4 ) \n";
	ss << " ( 2 1 5 )  ( 2 2 6 ) \n";
    ss << " [ 3 4 ] ";
	ss >> c;
	assert(3 == c.row_count());
	assert(4 == c.column_count());
    assert(6 == c.size());
	assert(1==c(0,0) && 2==c(0,2) && 3==c(1,1) && 4==c(1,3) && 5==c(2,1) && 6==c(2,2));

	so << c;
	so >> c;

	assert(3 == c.row_count());
	assert(4 == c.column_count());
    assert(6 == c.size());
	assert(1==c(0,0) && 2==c(0,2) && 3==c(1,1) && 4==c(1,3) && 5==c(2,1) && 6==c(2,2));

	cout << "pass CrossList<int>: \n" << c << endl;
}
