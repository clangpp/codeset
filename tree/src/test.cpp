// test.cpp
#include "tree/tree.h"

#include <cstdlib>
#include <iostream>

#include "logging/logging.h"

using namespace std;
using namespace logging;

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

template <typename T>
void print(const T& value) { cout << value << " "; }

void test_rotation();
void test_traversal();

int main(int argc, char* argv[]) {
	test_rotation();
	test_traversal();
	system("pause");
    return 0;
}

void test_rotation() {
	Trace trace(INFO_, "test_rotation()");
}

void test_traversal() {
	Trace trace(INFO_, "test_traversal()");
}
