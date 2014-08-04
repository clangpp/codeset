// tree.h
#ifndef TREE_H_
#define TREE_H_

#include <cstddef>
#include <iostream>
#include <vector>

namespace tree {

typedef std::size_t size_type;  // unsigned size type
typedef std::ptrdiff_t ssize_type;  // signed size type

// ********** <note> ************
// concept NodeT {
//     typedef implemention-defined value_type;
//     value_type value;
//     NodeT* left;
//     NodeT* right
//     NodeT* parent;
// };
// ********** </note> ***********

template <typename NodeT>
inline void set_left(NodeT* root, NodeT* new_left);

template <typename NodeT>
inline void set_right(NodeT* root, NodeT* new_right);

template <typename NodeT>
inline void replace_child(NodeT* old_child, NodeT* new_child);

template <typename NodeT>
inline void destory(NodeT*& root);

template <typename NodeT>
inline void print(const NodeT* root, std::ostream& out = std::cout);

template <typename NodeT>
inline void print_preorder(const NodeT* root, std::ostream& out = std::cout);

template <typename NodeT>
inline void print_inorder(const NodeT* root, std::ostream& out = std::cout);

template <typename NodeT>
inline void print_postorder(const NodeT* root, std::ostream& out = std::cout);

namespace internal {

template <typename NodeT>
void print_preorder(std::ostream& out,
        const NodeT* root, std::vector<int>& path);

template <typename NodeT>
void print_inorder(std::ostream& out,
        const NodeT* root, std::vector<int>& path);

template <typename NodeT>
void print_postorder(std::ostream& out,
        const NodeT* root, std::vector<int>& path);

}  // namespace internal

template <typename NodeT>
void rotate_left_left(NodeT*& root);  // left-left single rotation

template <typename NodeT>
void rotate_right_right(NodeT*& root);  // right-right single rotation

template <typename NodeT>
inline void rotate_left_right(NodeT*& root);  // left-right double rotation

template <typename NodeT>
inline void rotate_right_left(NodeT*& root);  // right-left double rotation

namespace internal {

template <typename NodeT>
NodeT* double_rotation(
        NodeT* old_root, NodeT* new_left, NodeT* new_right, NodeT* new_root);

}  // namespace internal

// ********** <note> ************
// concept NodePtrT {
//     NodeT* or const NodeT*
// };
// ********** </note> ***********

// ********** <note> ************
// concept UnaryFunction {
//     UnaryFunction op;
//     NodePtrT p;
//     requires op(p->value);
// };
// ********** </note> ***********

template <typename NodePtrT, typename UnaryFunction>
inline void traverse_preorder(NodePtrT root, UnaryFunction op);

template <typename NodePtrT, typename UnaryFunction>
inline void traverse_inorder(NodePtrT root, UnaryFunction op);

template <typename NodePtrT, typename UnaryFunction>
inline void traverse_postorder(NodePtrT root, UnaryFunction op);

template <typename NodePtrT, typename UnaryFunction>
void traverse_level_order(NodePtrT root, UnaryFunction op);

template <typename NodePtrT, typename UnaryFunction>
void traverse_preorder_recursive(NodePtrT root, UnaryFunction op);

template <typename NodePtrT, typename UnaryFunction>
void traverse_inorder_recursive(NodePtrT root, UnaryFunction op);

template <typename NodePtrT, typename UnaryFunction>
void traverse_postorder_recursive(NodePtrT root, UnaryFunction op);

template <typename NodePtrT, typename UnaryFunction>
void traverse_preorder_stack(NodePtrT root, UnaryFunction op);

template <typename NodePtrT, typename UnaryFunction>
void traverse_inorder_stack(NodePtrT root, UnaryFunction op);

template <typename NodePtrT, typename UnaryFunction>
void traverse_postorder_stack(NodePtrT root, UnaryFunction op);

template <typename NodePtrT, typename UnaryFunction>
void traverse_preorder_tricky(NodePtrT root, UnaryFunction op);

template <typename NodePtrT, typename UnaryFunction>
void traverse_inorder_tricky(NodePtrT root, UnaryFunction op);

template <typename NodePtrT, typename UnaryFunction>
void traverse_postorder_tricky(NodePtrT root, UnaryFunction op);

template <typename NodeT>
ssize_type height(const NodeT* root);  // NULL tree's height is -1

template <typename NodeT>
size_type count(const NodeT* root);  // how many nodes in tree

// search value in binary-search tree
// return node that contains value; if not found, return NULL
template <typename NodePtrT, typename T>
NodePtrT find(NodePtrT root, const T& value);

// binary-search tree insertion
// return node that contains new_node->value
template <typename NodeT, typename BinaryPredicate>
NodeT* insert(NodeT*& root, NodeT* new_node, BinaryPredicate pred);

template <typename NodeT>
inline NodeT* insert(NodeT*& root, NodeT* new_node);

namespace avl {  // AVL tree algorithm

// ********** <note> ************
// concept AVLNodeT : public NodeT{
//     ssize_type height;
// };
// ********** </note> ***********

template <typename AVLNodeT>
inline ssize_type height(const AVLNodeT* root);

template <typename AVLNodeT>
inline void update_height(AVLNodeT* root);

template <typename AVLNodeT>
inline void rotate_left_left(AVLNodeT*& root);  // left-left single rotation

template <typename AVLNodeT>
inline void rotate_right_right(AVLNodeT*& root);  // right-right single rotation

template <typename AVLNodeT>
inline void rotate_left_right(AVLNodeT*& root);  // left-right double rotation

template <typename AVLNodeT>
inline void rotate_right_left(AVLNodeT*& root);  // right-left double rotation

// insert new_node into AVL tree root
// return node that contains new_node->value
template <typename AVLNodeT, typename BinaryPredicate>
AVLNodeT* insert(AVLNodeT*& root, AVLNodeT* new_node, BinaryPredicate pred);

template <typename AVLNodeT>
inline AVLNodeT* insert(AVLNodeT*& root, AVLNodeT* new_node);

}  // namespace avl

namespace splay {

// return new root of splaying tree, i.e. target
template <typename NodeT>
NodeT* adjust(NodeT* target);

}  // namespace splay

template <typename RandomAccessIterator1,
         typename RandomAccessIterator2, typename OutputIterator>
OutputIterator preorder_inorder_to_postorder(
        RandomAccessIterator1 pre_first, RandomAccessIterator1 pre_last,
        RandomAccessIterator2 in_first, OutputIterator post_iter);

template <typename RandomAccessIterator1,
         typename RandomAccessIterator2, typename OutputIterator>
OutputIterator inorder_postorder_to_preorder(
        RandomAccessIterator1 in_first, RandomAccessIterator1 in_last,
        RandomAccessIterator2 post_first, OutputIterator pre_iter);

template <typename NodeT, typename RandomAccessIterator1,
         typename RandomAccessIterator2>
NodeT* preorder_inorder_to_tree(
        RandomAccessIterator1 pre_first, RandomAccessIterator1 pre_last,
        RandomAccessIterator2 in_first);

template <typename NodeT, typename RandomAccessIterator1,
         typename RandomAccessIterator2>
NodeT* inorder_postorder_to_tree(
        RandomAccessIterator1 in_first, RandomAccessIterator1 in_last,
        RandomAccessIterator2 post_first);

namespace heap {

// ********** <note> ************
// assumption: array index is zero based
// ********** </note> ***********

inline size_type left(size_type pos);

inline size_type right(size_type pos);

inline size_type parent(size_type pos);

template <typename RandomAccessIterator, typename BinaryPredicate>
void percolate_up(RandomAccessIterator first,
        RandomAccessIterator last, size_type pos, BinaryPredicate pred);

template <typename RandomAccessIterator, typename BinaryPredicate>
void percolate_down(RandomAccessIterator first,
        RandomAccessIterator last, size_type pos, BinaryPredicate pred);

// equivalent to std::make_heap()
// post-condition: for each none root position 'pos',
//  pred(first[pos], first[parent(pos)]) != false
template <typename RandomAccessIterator, typename BinaryPredicate>
void make(RandomAccessIterator first,
        RandomAccessIterator last, BinaryPredicate pred);

template <typename RandomAccessIterator>
inline void make(RandomAccessIterator first, RandomAccessIterator last);

// equivalent to std::push_heap()
template <typename RandomAccessIterator, typename BinaryPredicate>
void push(RandomAccessIterator first,
        RandomAccessIterator last, BinaryPredicate pred);

template <typename RandomAccessIterator>
inline void push(RandomAccessIterator first, RandomAccessIterator last);

// equivalent to std::pop_heap()
template <typename RandomAccessIterator, typename BinaryPredicate>
void pop(RandomAccessIterator first,
        RandomAccessIterator last, BinaryPredicate pred);

template <typename RandomAccessIterator>
inline void pop(RandomAccessIterator first, RandomAccessIterator last);

}  // namespace heap

}  // namespace tree

#include "tree-inl.h"

#endif  // TREE_H_
