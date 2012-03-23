// test.cpp
#include "tree/tree.h"

#include <algorithm>
#include <cstdlib>
#include <functional>
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

struct Collector {
	vector<int> values;
	void collect(int value) { values.push_back(value); }
};

// unary function to print node value
template <typename T>
void print(const T& value) { cout << value << " "; }

void test_utility();
void test_rotation();
void test_traversal();
void test_observer();

int main(int argc, char* argv[]) {
	test_utility();
	test_rotation();
	test_traversal();
	test_observer();
	system("pause");
    return 0;
}

void test_utility() {
	Trace trace(INFO_, "test_utility()");

	// make tree
	vector<BSNode<int> > nodes;
	nodes.resize(6);
	for (size_t i=0; i<nodes.size(); ++i)
		nodes[i] = BSNode<int>(i);
	tree::set_left(&nodes[0], &nodes[1]);
	tree::set_right(&nodes[0], &nodes[2]);
	tree::set_left(&nodes[1], &nodes[3]);
	tree::set_right(&nodes[3], &nodes[4]);
	tree::set_right(&nodes[2], &nodes[5]);
	tree::set_left(&nodes[5], (BSNode<int>*)NULL);
	tree::set_right(&nodes[5], (BSNode<int>*)NULL);

	bool pass = true;
	pass = (nodes[0].left==&nodes[1]) && (nodes[1].parent==&nodes[0])
		&& (nodes[5].left==NULL);
	log(INFO_) << "test set_left: " << (pass ? "pass" : "FAILED") << endl;

	pass = (nodes[0].right==&nodes[2]) && (nodes[2].parent==&nodes[0])
		&& (nodes[5].right==NULL);
	log(INFO_) << "test set_right: " << (pass ? "pass" : "FAILED") << endl;

	log(INFO_) << "print: " << endl;
	tree::print(&nodes[0]);

	log(INFO_) << "print_preorder: " << endl;
	tree::print_preorder(&nodes[0]);

	log(INFO_) << "print_inorder: " << endl;
	tree::print_inorder(&nodes[0]);

	log(INFO_) << "print_postorder: " << endl;
	tree::print_postorder(&nodes[0]);
}

void test_rotation() {
	Trace trace(INFO_, "test_rotation()");
	bool pass = true;
	vector<BSNode<int> > nodes;
	BSNode<int>* p = NULL;

	nodes.resize(4);
	for (size_t i=0; i<nodes.size(); ++i)
		nodes[i] = BSNode<int>(i);
	p = &nodes[3];
	tree::set_left(&nodes[3], &nodes[1]);
	tree::set_left(&nodes[1], &nodes[0]);
	tree::set_right(&nodes[1], &nodes[2]);
	log(INFO_) << "before rotation: " << endl;
	tree::print(p);
	p = tree::rotate_left_left(p);
	log(INFO_) << "after rotation: " << endl;
	tree::print(p);
	pass = (p==&nodes[1]) && (p->right==&nodes[3]) && (nodes[3].left==&nodes[2]);
	log(INFO_) << "test rotate_left_left: " << (pass ? "pass" : "FAILED") << endl;

	nodes.resize(4);
	for (size_t i=0; i<nodes.size(); ++i)
		nodes[i] = BSNode<int>(i);
	p = &nodes[0];
	tree::set_right(&nodes[0], &nodes[2]);
	tree::set_right(&nodes[2], &nodes[3]);
	tree::set_left(&nodes[2], &nodes[1]);
	log(INFO_) << "before rotation: " << endl;
	tree::print(p);
	p = tree::rotate_right_right(p);
	log(INFO_) << "after rotation: " << endl;
	tree::print(p);
	pass = (p==&nodes[2]) && (p->left==&nodes[0]) && (nodes[0].right==&nodes[1]);
	log(INFO_) << "test rotate_right_right: " << (pass ? "pass" : "FAILED") << endl;

	nodes.resize(6);
	for (size_t i=0; i<nodes.size(); ++i)
		nodes[i] = BSNode<int>(i);
	p = &nodes[4];
	tree::set_left(&nodes[4], &nodes[1]);
	tree::set_right(&nodes[4], &nodes[5]);
	tree::set_left(&nodes[1], &nodes[0]);
	tree::set_right(&nodes[1], &nodes[3]);
	tree::set_left(&nodes[3], &nodes[2]);
	log(INFO_) << "before rotation: " << endl;
	tree::print(p);
	p = tree::rotate_left_right(p);
	log(INFO_) << "after rotation: " << endl;
	tree::print(p);
	pass = (p==&nodes[3]) && (nodes[1].right==&nodes[2]) && (nodes[4].left==NULL)
		&& (nodes[3].left==&nodes[1]) && (nodes[3].right==&nodes[4]);
	log(INFO_) << "test rotate_left_right: " << (pass ? "pass" : "FAILED") << endl;

	nodes.resize(6);
	for (size_t i=0; i<nodes.size(); ++i)
		nodes[i] = BSNode<int>(i);
	p = &nodes[1];
	tree::set_left(&nodes[1], &nodes[0]);
	tree::set_right(&nodes[1], &nodes[4]);
	tree::set_left(&nodes[4], &nodes[3]);
	tree::set_right(&nodes[4], &nodes[5]);
	tree::set_left(&nodes[3], &nodes[2]);
	log(INFO_) << "before rotation: " << endl;
	tree::print(p);
	p = tree::rotate_right_left(p);
	log(INFO_) << "after rotation: " << endl;
	tree::print(p);
	pass = (p==&nodes[3]) && (nodes[1].right==&nodes[2]) && (nodes[4].left==NULL)
		&& (nodes[3].left==&nodes[1]) && (nodes[3].right==&nodes[4]);
	log(INFO_) << "test rotate_right_left: " << (pass ? "pass" : "FAILED") << endl;
}

void test_traversal() {
	Trace trace(INFO_, "test_traversal()");
	bool pass = true;
	vector<BSNode<int> > nodes;
	BSNode<int>* p = NULL;

	nodes.resize(7);
	for (size_t i=0; i<nodes.size(); ++i)
		nodes[i] = BSNode<int>(i);
	p = &nodes[0];
	tree::set_left(&nodes[0], &nodes[1]);
	tree::set_right(&nodes[0], &nodes[2]);
	tree::set_left(&nodes[1], &nodes[3]);
	tree::set_right(&nodes[1], &nodes[4]);
	tree::set_left(&nodes[2], &nodes[5]);
	tree::set_left(&nodes[4], &nodes[6]);
	log(INFO_) << "tree: " << endl;
	tree::print(p);

	Collector co;
	co.values.clear();
	tree::traverse_inorder(p, bind1st(mem_fun(&Collector::collect), &co));
	log(INFO_) << "inorder traversal: ";
	for (size_t i=0; i<co.values.size(); ++i)
		standard_logger() << co.values[i] << " ";
	standard_logger() << endl;
	int inorder[] = {3,1,6,4,0,5,2};
	pass = equal(co.values.begin(), co.values.end(), inorder);
	log(INFO_) << "test traverse_inorder: " << (pass ? "pass" : "FAILED") << endl;

	co.values.clear();
	tree::traverse_postorder(p, bind1st(mem_fun(&Collector::collect), &co));
	log(INFO_) << "postorder traversal: ";
	for (size_t i=0; i<co.values.size(); ++i)
		standard_logger() << co.values[i] << " ";
	standard_logger() << endl;
	int postorder[] = {3,6,4,1,5,2,0};
	pass = equal(co.values.begin(), co.values.end(), postorder);
	log(INFO_) << "test traverse_postorder: " << (pass ? "pass" : "FAILED") << endl;

	co.values.clear();
	tree::traverse_preorder(p, bind1st(mem_fun(&Collector::collect), &co));
	log(INFO_) << "preorder traversal: ";
	for (size_t i=0; i<co.values.size(); ++i)
		standard_logger() << co.values[i] << " ";
	standard_logger() << endl;
	int preorder[] = {0,1,3,4,6,2,5};
	pass = equal(co.values.begin(), co.values.end(), preorder);
	log(INFO_) << "test traverse_preorder: " << (pass ? "pass" : "FAILED") << endl;

	co.values.clear();
	tree::traverse_level_order(p, bind1st(mem_fun(&Collector::collect), &co));
	log(INFO_) << "level_order traversal: ";
	for (size_t i=0; i<co.values.size(); ++i)
		standard_logger() << co.values[i] << " ";
	standard_logger() << endl;
	int level_order[] = {0,1,2,3,4,5,6};
	pass = equal(co.values.begin(), co.values.end(), level_order);
	log(INFO_) << "test traverse_level_order: " << (pass ? "pass" : "FAILED") << endl;
}

void test_observer() {
	Trace trace(INFO_, "test_observer()");
	bool pass = true;
	vector<BSNode<int> > nodes;
	BSNode<int>* p = NULL;

	nodes.resize(7);
	for (size_t i=0; i<nodes.size(); ++i)
		nodes[i] = BSNode<int>(i);
	tree::set_left(&nodes[0], &nodes[1]);
	tree::set_right(&nodes[0], &nodes[2]);
	tree::set_left(&nodes[1], &nodes[3]);
	tree::set_right(&nodes[1], &nodes[4]);
	tree::set_left(&nodes[2], &nodes[5]);
	tree::set_left(&nodes[4], &nodes[6]);
	pass = (tree::height((BSNode<int>*)NULL)==-1) && (tree::height(&nodes[0])==3) 
		&& (tree::height(&nodes[1])==2) && (tree::height(&nodes[2])==1) 
		&& (tree::height(&nodes[6])==0);
	log(INFO_) << "test height: " << (pass ? "pass" : "FAILED") << endl;
}
