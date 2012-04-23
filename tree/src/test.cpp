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
void test_splay_tree();
void test_traversal_orders();
void test_heap();

int main(int argc, char* argv[]) {
    test_utility();
    test_rotation();
    test_traversal();
    test_observer();
    test_avl_tree();
    test_splay_tree();
    test_traversal_orders();
    test_heap();
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
    tree::rotate_left_left(p);
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
    tree::rotate_right_right(p);
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
    tree::rotate_left_right(p);
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
    tree::rotate_right_left(p);
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
    copy(co.values.begin(), co.values.end(), log(" "));
    log() << endl;
    int inorder[] = {3,1,6,4,0,5,2};
    pass = equal(co.values.begin(), co.values.end(), inorder);
    log(INFO_) << "test traverse_inorder: " << (pass ? "pass" : "FAILED") << endl;

    co.values.clear();
    tree::traverse_postorder(p, bind1st(mem_fun(&Collector::collect), &co));
    log(INFO_) << "postorder traversal: ";
    copy(co.values.begin(), co.values.end(), log(" "));
    log() << endl;
    int postorder[] = {3,6,4,1,5,2,0};
    pass = equal(co.values.begin(), co.values.end(), postorder);
    log(INFO_) << "test traverse_postorder: " << (pass ? "pass" : "FAILED") << endl;

    co.values.clear();
    tree::traverse_preorder(p, bind1st(mem_fun(&Collector::collect), &co));
    log(INFO_) << "preorder traversal: ";
    copy(co.values.begin(), co.values.end(), log(" "));
    log() << endl;
    int preorder[] = {0,1,3,4,6,2,5};
    pass = equal(co.values.begin(), co.values.end(), preorder);
    log(INFO_) << "test traverse_preorder: " << (pass ? "pass" : "FAILED") << endl;

    co.values.clear();
    tree::traverse_level_order(p, bind1st(mem_fun(&Collector::collect), &co));
    log(INFO_) << "level_order traversal: ";
    copy(co.values.begin(), co.values.end(), log(" "));
    log() << endl;
    int level_order[] = {0,1,2,3,4,5,6};
    pass = equal(co.values.begin(), co.values.end(), level_order);
    log(INFO_) << "test traverse_level_order: " << (pass ? "pass" : "FAILED") << endl;

    co.values.clear();
    tree::traverse_inorder_recursive(p, bind1st(mem_fun(&Collector::collect), &co));
    log(INFO_) << "inorder traversal: ";
    copy(co.values.begin(), co.values.end(), log(" "));
    log() << endl;
    pass = equal(co.values.begin(), co.values.end(), inorder);
    log(INFO_) << "test traverse_inorder_recursive: " << (pass ? "pass" : "FAILED") << endl;

    co.values.clear();
    tree::traverse_postorder_recursive(p, bind1st(mem_fun(&Collector::collect), &co));
    log(INFO_) << "postorder traversal: ";
    copy(co.values.begin(), co.values.end(), log(" "));
    log() << endl;
    pass = equal(co.values.begin(), co.values.end(), postorder);
    log(INFO_) << "test traverse_postorder_recursive: " << (pass ? "pass" : "FAILED") << endl;

    co.values.clear();
    tree::traverse_preorder_recursive(p, bind1st(mem_fun(&Collector::collect), &co));
    log(INFO_) << "preorder traversal: ";
    copy(co.values.begin(), co.values.end(), log(" "));
    log() << endl;
    pass = equal(co.values.begin(), co.values.end(), preorder);
    log(INFO_) << "test traverse_preorder_recursive: " << (pass ? "pass" : "FAILED") << endl;

    co.values.clear();
    tree::traverse_inorder_stack(p, bind1st(mem_fun(&Collector::collect), &co));
    log(INFO_) << "inorder traversal: ";
    copy(co.values.begin(), co.values.end(), log(" "));
    log() << endl;
    pass = equal(co.values.begin(), co.values.end(), inorder);
    log(INFO_) << "test traverse_inorder_stack: " << (pass ? "pass" : "FAILED") << endl;

    co.values.clear();
    tree::traverse_postorder_stack(p, bind1st(mem_fun(&Collector::collect), &co));
    log(INFO_) << "postorder traversal: ";
    copy(co.values.begin(), co.values.end(), log(" "));
    log() << endl;
    pass = equal(co.values.begin(), co.values.end(), postorder);
    log(INFO_) << "test traverse_postorder_stack: " << (pass ? "pass" : "FAILED") << endl;

    co.values.clear();
    tree::traverse_preorder_stack(p, bind1st(mem_fun(&Collector::collect), &co));
    log(INFO_) << "preorder traversal: ";
    copy(co.values.begin(), co.values.end(), log(" "));
    log() << endl;
    pass = equal(co.values.begin(), co.values.end(), preorder);
    log(INFO_) << "test traverse_preorder_stack: " << (pass ? "pass" : "FAILED") << endl;

    co.values.clear();
    tree::traverse_inorder_tricky(p, bind1st(mem_fun(&Collector::collect), &co));
    log(INFO_) << "inorder traversal: ";
    copy(co.values.begin(), co.values.end(), log(" "));
    log() << endl;
    pass = equal(co.values.begin(), co.values.end(), inorder);
    log(INFO_) << "test traverse_inorder_tricky: " << (pass ? "pass" : "FAILED") << endl;

    co.values.clear();
    tree::traverse_postorder_tricky(p, bind1st(mem_fun(&Collector::collect), &co));
    log(INFO_) << "postorder traversal: ";
    copy(co.values.begin(), co.values.end(), log(" "));
    log() << endl;
    pass = equal(co.values.begin(), co.values.end(), postorder);
    log(INFO_) << "test traverse_postorder_tricky: " << (pass ? "pass" : "FAILED") << endl;

    co.values.clear();
    tree::traverse_preorder_tricky(p, bind1st(mem_fun(&Collector::collect), &co));
    log(INFO_) << "preorder traversal: ";
    copy(co.values.begin(), co.values.end(), log(" "));
    log() << endl;
    pass = equal(co.values.begin(), co.values.end(), preorder);
    log(INFO_) << "test traverse_preorder_tricky: " << (pass ? "pass" : "FAILED") << endl;
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

    pass = (tree::count((BSNode<int>*)NULL)==0) && (tree::count(&nodes[3])==7)
        && (tree::count(&nodes[4])==3) && (tree::count(&nodes[1])==3)
        && (tree::count(&nodes[5])==1);
    log(INFO_) << "test count: " << (pass ? "pass" : "FAILED") << endl;

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

void test_splay_tree() {
    Trace trace(INFO_, "test_splay_tree()");
    bool pass = true;
    vector<BSNode<int> > nodes;
    BSNode<int>* root = NULL;
    BSNode<int>* curr = NULL;

    nodes.resize(100);
    for (size_t i=0; i<nodes.size(); ++i)
        nodes[i] = BSNode<int>(i);
    for (int i=7; i>=1; --i)
        tree::insert(root, &nodes[i]);
    log(INFO_) << "original tree: " << endl;
    tree::print(root);

    for (int i=1; i<=7; ++i) {
        curr = &nodes[i];
        root = tree::splay::adjust(curr);
        log(INFO_) << "after adjust " << curr->value << endl;
        tree::print(root);
        pass = (root==curr) && (NULL==root->parent);
        log(INFO_) << "test adjust: " << (pass ? "pass" : "FAILED") << endl;
    }
}

void test_traversal_orders() {
    Trace trace(INFO_, "test_traversal_orders()");
    bool pass = true;
    vector<BSNode<int> > nodes;
    BSNode<int>* root = NULL;

    nodes.resize(100);
    for (size_t i=0; i<nodes.size(); ++i)
        nodes[i] = BSNode<int>(i);
    tree::insert(root, &nodes[50]);
    tree::insert(root, &nodes[30]);
    tree::insert(root, &nodes[20]);
    tree::insert(root, &nodes[15]);
    tree::insert(root, &nodes[10]);
    tree::insert(root, &nodes[25]);
    tree::insert(root, &nodes[40]);
    tree::insert(root, &nodes[35]);
    tree::insert(root, &nodes[45]);
    tree::insert(root, &nodes[47]);
    tree::insert(root, &nodes[70]);
    tree::insert(root, &nodes[55]);
    tree::insert(root, &nodes[60]);
    tree::insert(root, &nodes[65]);
    tree::insert(root, &nodes[85]);
    tree::insert(root, &nodes[80]);
    tree::insert(root, &nodes[75]);
    log(INFO_) << "tree is " << endl;
    tree::print(root);

    Collector co;
    co.values.clear();
    tree::traverse_preorder(root, bind1st(mem_fun(&Collector::collect), &co));
    vector<int> preorders = co.values;
    log(INFO_) << "preorder traversal: ";
    for (size_t i=0; i<preorders.size(); ++i)
        standard_logger() << preorders[i] << " ";
    standard_logger() << endl;

    co.values.clear();
    tree::traverse_inorder(root, bind1st(mem_fun(&Collector::collect), &co));
    vector<int> inorders = co.values;
    log(INFO_) << "inorder traversal: ";
    for (size_t i=0; i<inorders.size(); ++i)
        standard_logger() << inorders[i] << " ";
    standard_logger() << endl;

    co.values.clear();
    tree::traverse_postorder(root, bind1st(mem_fun(&Collector::collect), &co));
    vector<int> postorders = co.values;
    log(INFO_) << "postorder traversal: ";
    for (size_t i=0; i<postorders.size(); ++i)
        standard_logger() << postorders[i] << " ";
    standard_logger() << endl;

    vector<int> result(inorders.size());
    vector<int>::iterator iter;
    iter = tree::preorder_inorder_to_postorder(
            preorders.begin(), preorders.end(), inorders.begin(), result.begin());
    log(INFO_) << "calculated postorder traversal: ";
    for (size_t i=0; i<result.size(); ++i)
        standard_logger() << result[i] << " ";
    standard_logger() << endl;
    pass == (iter==result.end()) && (result==postorders);
    log(INFO_) << "test preorder_inorder_to_postorder: " << (pass ? "pass" : "FAILED") << endl;

    iter = tree::inorder_postorder_to_preorder(
            inorders.begin(), inorders.end(), postorders.begin(), result.begin());
    log(INFO_) << "calculated postorder traversal: ";
    for (size_t i=0; i<result.size(); ++i)
        standard_logger() << result[i] << " ";
    standard_logger() << endl;
    pass == (iter==result.end()) && (result==preorders);
    log(INFO_) << "test inorder_postorder_to_preorder: " << (pass ? "pass" : "FAILED") << endl;
}

void test_heap() {
    Trace trace(INFO_, "test_heap()");
    vector<int> values;
    vector<BSNode<int> > nodes;
    bool pass = true;

    pass = (tree::heap::left(0)==1) && (tree::heap::left(1)==3)
        && (tree::heap::left(2)==5) && (tree::heap::left(5)==11);
    log(INFO_) << "test left: " << (pass ? "pass" : "FAILED") << endl;

    pass = (tree::heap::right(0)==2) && (tree::heap::right(1)==4)
        && (tree::heap::right(2)==6) && (tree::heap::right(5)==12);
    log(INFO_) << "test right: " << (pass ? "pass" : "FAILED") << endl;

    pass = (tree::heap::parent(0)==0) && (tree::heap::parent(1)==0)
        && (tree::heap::parent(2)==0) && (tree::heap::parent(5)==2);
    log(INFO_) << "test parent: " << (pass ? "pass" : "FAILED") << endl;

    // test percolate_up()
    int data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, -9, 10};
    values.assign(data, data+sizeof(data)/sizeof(int));
    nodes.resize(values.size());
    for (size_t i=0; i<values.size(); ++i) {
        nodes[i] = BSNode<int>(values[i]);
        if (tree::heap::left(i)<values.size())
            tree::set_left(&nodes[i], &nodes[tree::heap::left(i)]);
        if (tree::heap::right(i)<values.size())
            tree::set_right(&nodes[i], &nodes[tree::heap::right(i)]);
    }
    log(INFO_) << "original tree" << endl;
    tree::print(&nodes[0]);

    int i = 9;
    tree::heap::percolate_up(
            values.begin(), values.end(), i, greater<int>());
    for (size_t i=0; i<values.size(); ++i)
        nodes[i].value = values[i];
    log(INFO_) << "after percolate_up " << i << ": " << data[i] << endl;
    tree::print(&nodes[0]);
    int data_check[] = {-9, 0, 2, 3, 1, 5, 6, 7, 8, 4, 10};
    pass = equal(values.begin(), values.end(), data_check);
    log(INFO_) << "test percolate_up: " << (pass ? "pass" : "FAILED") << endl;

    // test percolate_down()
    int data0[] = {0, 19, 2, 4, 3, 5, 6, 7, 8, 9, 10, 11, 12};
    values.assign(data0, data0+sizeof(data0)/sizeof(int));
    nodes.resize(values.size());
    for (size_t i=0; i<values.size(); ++i) {
        nodes[i] = BSNode<int>(values[i]);
        if (tree::heap::left(i)<values.size())
            tree::set_left(&nodes[i], &nodes[tree::heap::left(i)]);
        if (tree::heap::right(i)<values.size())
            tree::set_right(&nodes[i], &nodes[tree::heap::right(i)]);
    }
    log(INFO_) << "original tree" << endl;
    tree::print(&nodes[0]);

    i = 1;
    tree::heap::percolate_down(
            values.begin(), values.end(), i, greater<int>());
    for (size_t i=0; i<values.size(); ++i)
        nodes[i].value = values[i];
    log(INFO_) << "after percolate_down " << i << ": " << data0[i] << endl;
    tree::print(&nodes[0]);
    int data0_check[] = {0, 3, 2, 4, 9, 5, 6, 7, 8, 19, 10, 11, 12};
    pass = equal(values.begin(), values.end(), data0_check);
    log(INFO_) << "test percolate_down: " << (pass ? "pass" : "FAILED") << endl;

    // test make()
    int data1[] = {150, 80, 40, 30, 10, 70, 110, 100, 20, 90, 60, 50, 120, 140, 130};
    values.assign(data1, data1+sizeof(data1)/sizeof(int));
    nodes.resize(values.size());
    for (size_t i=0; i<values.size(); ++i) {
        nodes[i] = BSNode<int>(values[i]);
        if (tree::heap::left(i)<values.size())
            tree::set_left(&nodes[i], &nodes[tree::heap::left(i)]);
        if (tree::heap::right(i)<values.size())
            tree::set_right(&nodes[i], &nodes[tree::heap::right(i)]);
    }
    log(INFO_) << "original array" << endl;
    tree::print(&nodes[0]);

    tree::heap::make(values.begin(), values.end(), greater<int>());

    for (size_t i=0; i<values.size(); ++i)
        nodes[i].value = values[i];
    log(INFO_) << "after heap::make" << endl;
    tree::print(&nodes[0]);

    int data1_check[] = {10, 20, 40, 30, 60, 50, 110, 100, 150, 90, 80, 70, 120, 140, 130};
    pass = equal(values.begin(), values.end(), data1_check);
    log(INFO_) << "test make: " << (pass ? "pass" : "FAILED") << endl;

    // test push()
    int data2[] = {13, 21, 16, 24, 31, 19, 68, 65, 26, 32, 14};
    values.assign(data2, data2+sizeof(data2)/sizeof(int));
    nodes.resize(values.size());
    for (size_t i=0; i<values.size(); ++i) {
        nodes[i] = BSNode<int>(values[i]);
        if (tree::heap::left(i)<values.size())
            tree::set_left(&nodes[i], &nodes[tree::heap::left(i)]);
        if (tree::heap::right(i)<values.size())
            tree::set_right(&nodes[i], &nodes[tree::heap::right(i)]);
    }
    log(INFO_) << "original heap" << endl;
    tree::print(&nodes[0]);

    tree::heap::push(values.begin(), values.end(), greater<int>());

    for (size_t i=0; i<values.size(); ++i)
        nodes[i].value = values[i];
    log(INFO_) << "after heap::push" << endl;
    tree::print(&nodes[0]);

    int data2_check[] = {13, 14, 16, 24, 21, 19, 68, 65, 26, 32, 31};
    pass = equal(values.begin(), values.end(), data2_check);
    log(INFO_) << "test push: " << (pass ? "pass" : "FAILED") << endl;

    // test pop()
    int data3[] = {13, 14, 16, 19, 21, 19, 68, 65, 26, 32, 31};
    values.assign(data3, data3+sizeof(data3)/sizeof(int));
    nodes.resize(values.size());
    for (size_t i=0; i<values.size(); ++i) {
        nodes[i] = BSNode<int>(values[i]);
        if (tree::heap::left(i)<values.size())
            tree::set_left(&nodes[i], &nodes[tree::heap::left(i)]);
        if (tree::heap::right(i)<values.size())
            tree::set_right(&nodes[i], &nodes[tree::heap::right(i)]);
    }
    log(INFO_) << "original heap" << endl;
    tree::print(&nodes[0]);

    tree::heap::pop(values.begin(), values.end(), greater<int>());

    for (size_t i=0; i<values.size(); ++i)
        nodes[i].value = values[i];
    log(INFO_) << "after heap::pop" << endl;
    tree::print(&nodes[0]);

    int data3_check[] = {14, 19, 16, 26, 21, 19, 68, 65, 31, 32, 13};
    pass = equal(values.begin(), values.end(), data3_check);
    log(INFO_) << "test pop: " << (pass ? "pass" : "FAILED") << endl;
}
