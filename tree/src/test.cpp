// test.cpp
#include "tree/tree.h"

#include <cstdlib>
#include <iostream>

#include "logging/logging.h"

using namespace std;
using namespace logging;

// binary search tree node type
template <typename T>
struct BSNode {
	typedef T value_type;
	value_type value;
	BSNode* left;
	BSNode* right;
	BSNode* parent;
	BSNode(const value_type& val=value_type(),
			BSNode* l=NULL, BSNode* r=NULL, BSNode* p=NULL)
		: value(val), left(l), right(r), parent(p) {}
};

// unary function to print node value
template <typename T>
void print(const T& value) { cout << value << " "; }

void test_print_tree();
void test_rotation();
void test_traversal();

int main(int argc, char* argv[]) {
	test_print_tree();
	test_rotation();
	test_traversal();
	system("pause");
    return 0;
}

void test_print_tree() {
	Trace trace(INFO_, "test_print_tree()");
	vector<BSNode<int> > nodes;
	nodes.resize(6);
	for (size_t i=0; i<nodes.size(); ++i)
		nodes[i] = BSNode<int>(i);
	using tree::internal::set_left;
	using tree::internal::set_right;
	set_left(&nodes[0], &nodes[1]);
	set_right(&nodes[0], &nodes[2]);
	set_left(&nodes[1], &nodes[3]);
	set_right(&nodes[3], &nodes[4]);
	set_right(&nodes[2], &nodes[5]);
	tree::print_tree(&nodes[0]);
}

void test_rotation() {
	Trace trace(INFO_, "test_rotation()");
	const int N = 6;
	BSNode<int> nodes[N];
	for (int i=0; i<N; ++i)
		nodes[i] = BSNode<int>(i);
}

void test_traversal() {
	Trace trace(INFO_, "test_traversal()");
}
