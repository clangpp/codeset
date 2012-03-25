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

template <typename T>
struct AVLNode {
	typedef T value_type;
	value_type value;
	AVLNode* left;
	AVLNode* right;
	AVLNode* parent;
	tree::ssize_type height;
	AVLNode(const value_type& val=value_type(), AVLNode* l=NULL,
			AVLNode* r=NULL, AVLNode* p=NULL, tree::ssize_type h=0)
		: value(val), left(l), right(r), parent(p), height(h) {}
};

void test_utility();
void test_rotation();
void test_traversal();
void test_observer();
void test_avl_tree();

int main(int argc, char* argv[]) {
	test_utility();
	test_rotation();
	test_traversal();
	test_observer();
	test_avl_tree();
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
	pass = (NULL==p->parent) && (p==&nodes[1])
        && (p->right==&nodes[3]) && (nodes[3].left==&nodes[2]);
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
	pass = (NULL==p->parent) && (p==&nodes[2])
        && (p->left==&nodes[0]) && (nodes[0].right==&nodes[1]);
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
	pass = (NULL==p->parent) && (p==&nodes[3])
        && (nodes[1].right==&nodes[2]) && (NULL==nodes[4].left)
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
	pass = (NULL==p->parent) && (p==&nodes[3])
        && (nodes[1].right==&nodes[2]) && (NULL==nodes[4].left)
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
    BSNode<int>* root = NULL;
    BSNode<int>* curr = NULL;

	nodes.resize(7);
	for (size_t i=0; i<nodes.size(); ++i)
		nodes[i] = BSNode<int>(i);

    log(INFO_) << "original tree: " << endl;
    tree::print(root);

    curr = &nodes[3];
    p = tree::insert(root, curr);
    log(INFO_) << "after insert " << curr->value << endl;
    tree::print(root);
    pass = (root==&nodes[3]) && (p==curr);
	log(INFO_) << "test insert: " << (pass ? "pass" : "FAILED") << endl;

    curr = &nodes[1];
    p = tree::insert(root, &nodes[1]);
    log(INFO_) << "after insert " << curr->value << endl;
    tree::print(root);
    pass = (root==&nodes[3]) && (p==curr)
        && (nodes[3].left==curr) && (curr->parent==&nodes[3]);
	log(INFO_) << "test insert: " << (pass ? "pass" : "FAILED") << endl;

    curr = &nodes[4];
    p = tree::insert(root, &nodes[4]);
    log(INFO_) << "after insert " << curr->value << endl;
    tree::print(root);
    pass = (root==&nodes[3]) && (p==curr)
        && (nodes[3].right==curr) && (curr->parent==&nodes[3]);
	log(INFO_) << "test insert: " << (pass ? "pass" : "FAILED") << endl;

    curr = &nodes[0];
    p = tree::insert(root, curr);
    log(INFO_) << "after insert " << curr->value << endl;
    tree::print(root);
    pass = (root==&nodes[3]) && (p==curr)
        && (nodes[1].left==curr) && (curr->parent==&nodes[1]);
	log(INFO_) << "test insert: " << (pass ? "pass" : "FAILED") << endl;

    curr = &nodes[2];
    p = tree::insert(root, curr);
    log(INFO_) << "after insert " << curr->value << endl;
    tree::print(root);
    pass = (root==&nodes[3]) && (p==curr)
        && (nodes[1].right==curr) && (curr->parent==&nodes[1]);
	log(INFO_) << "test insert: " << (pass ? "pass" : "FAILED") << endl;

    curr = &nodes[6];
    p = tree::insert(root, curr);
    log(INFO_) << "after insert " << curr->value << endl;
    tree::print(root);
    pass = (root==&nodes[3]) && (p==curr)
        && (nodes[4].right==curr) && (curr->parent==&nodes[4]);
	log(INFO_) << "test insert: " << (pass ? "pass" : "FAILED") << endl;

    curr = &nodes[5];
    p = tree::insert(root, curr);
    log(INFO_) << "after insert " << curr->value << endl;
    tree::print(root);
    pass = (root==&nodes[3]) && (p==curr)
        && (nodes[6].left==curr) && (curr->parent==&nodes[6]);
	log(INFO_) << "test insert: " << (pass ? "pass" : "FAILED") << endl;

	pass = (tree::height((BSNode<int>*)NULL)==-1) && (tree::height(&nodes[3])==3) 
		&& (tree::height(&nodes[4])==2) && (tree::height(&nodes[1])==1) 
		&& (tree::height(&nodes[5])==0);
	log(INFO_) << "test height: " << (pass ? "pass" : "FAILED") << endl;

	pass = (tree::find(root, 6)==&nodes[6])
		&& (tree::find(root, 7)==NULL) && (tree::find(root, -1)==NULL);
	log(INFO_) << "test find: " << (pass ? "pass" : "FAILED") << endl;
}

void test_avl_tree() {
	Trace trace(INFO_, "test_avl_tree()");
	bool pass = true;
	vector<AVLNode<int> > nodes, neg_nodes;
	AVLNode<int>* root = NULL;
	AVLNode<int>* p = NULL;
    AVLNode<int>* curr = NULL;

	nodes.resize(100);
	for (size_t i=0; i<nodes.size(); ++i)
		nodes[i] = AVLNode<int>(i);

    log(INFO_) << "original tree: " << endl;
    tree::print(root);

    curr = &nodes[50];
    p = tree::avl::insert(root, curr);
    log(INFO_) << "after insert " << curr->value << endl;
    tree::print(root);
    pass = (p==curr) && (root==&nodes[50]);
	log(INFO_) << "test insert: " << (pass ? "pass" : "FAILED") << endl;

    curr = &nodes[45];
    p = tree::avl::insert(root, curr);
    log(INFO_) << "after insert " << curr->value << endl;
    tree::print(root);
    pass = (p==curr) && (root==&nodes[50]) && (root->left==&nodes[45]);
	log(INFO_) << "test insert: " << (pass ? "pass" : "FAILED") << endl;

    curr = &nodes[40];
    p = tree::avl::insert(root, curr);
    log(INFO_) << "after insert " << curr->value << endl;
    tree::print(root);
    pass = (p==curr) && (root==&nodes[45]);
	log(INFO_) << "test insert: " << (pass ? "pass" : "FAILED") << endl;
    pass = (root==&nodes[45]) &&
        (root->right==&nodes[50]) && (root->left==&nodes[40]);
	log(INFO_) << "test rotate_left_left: " << (pass ? "pass" : "FAILED") << endl;

    curr = &nodes[55];
    p = tree::avl::insert(root, curr);
    log(INFO_) << "after insert " << curr->value << endl;
    tree::print(root);
    pass = (p==curr) && (root==&nodes[45]);
	log(INFO_) << "test insert: " << (pass ? "pass" : "FAILED") << endl;

    curr = &nodes[60];
    p = tree::avl::insert(root, curr);
    log(INFO_) << "after insert " << curr->value << endl;
    tree::print(root);
    pass = (p==curr) && (root->right==&nodes[55]);
	log(INFO_) << "test insert: " << (pass ? "pass" : "FAILED") << endl;
    pass =  (nodes[55].left==&nodes[50]) && (nodes[55].right==&nodes[60]);
	log(INFO_) << "test rotate_right_right: " << (pass ? "pass" : "FAILED") << endl;

    curr = &nodes[48];
    p = tree::avl::insert(root, curr);
    log(INFO_) << "after insert " << curr->value << endl;
    tree::print(root);
    pass = (p==curr) && (root==&nodes[50]);
	log(INFO_) << "test insert: " << (pass ? "pass" : "FAILED") << endl;
    pass =  (root->left==&nodes[45]) && (root->right==&nodes[55])
        && (root->left->right==&nodes[48]) && (root->right->left==NULL);
	log(INFO_) << "test rotate_right_left: " << (pass ? "pass" : "FAILED") << endl;

    curr = &nodes[30];
    p = tree::avl::insert(root, curr);
    log(INFO_) << "after insert " << curr->value << endl;
    tree::print(root);
    pass = (p==curr) && (root==&nodes[50]);
	log(INFO_) << "test insert: " << (pass ? "pass" : "FAILED") << endl;

    curr = &nodes[35];
    p = tree::avl::insert(root, curr);
    log(INFO_) << "after insert " << curr->value << endl;
    tree::print(root);
    pass = (p==curr) && (nodes[45].left==&nodes[35]);
	log(INFO_) << "test insert: " << (pass ? "pass" : "FAILED") << endl;
    pass =  (nodes[35].left==&nodes[30]) && (nodes[35].right==&nodes[40])
        && (nodes[30].right==NULL) && (nodes[40].left==NULL);
	log(INFO_) << "test rotate_left_right: " << (pass ? "pass" : "FAILED") << endl;

    AVLNode<int> node60(60);
    curr = &node60;
    p = tree::avl::insert(root, curr);
    log(INFO_) << "after insert " << curr->value << endl;
    tree::print(root);
    pass = (p==&nodes[60]) && (p!=&node60);
	log(INFO_) << "test insert: " << (pass ? "pass" : "FAILED") << endl;
}
