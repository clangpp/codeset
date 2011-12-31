// test.cpp
#include "serialization/serialization.h"

#include <algorithm>
#include <cassert>
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

int main(int argc, char* argv[]) {
	test_pair_serialization();
	test_triad_serialization();
	test_cell_serialization();
	test_dimension_serialization();
	test_pair_cell_serialization();
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
	namespace sm = serialization::sparse_matrix;
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
	namespace sm = serialization::sparse_matrix;
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
	namespace sm = serialization::sparse_matrix;
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