// tree.h
#ifndef TREE_H_
#define TREE_H_

#include <cstddef>
#include <iostream>
#include <vector>

namespace tree {

typedef std::size_t size_type;  // unsigned size type
typedef long ssize_type;  // signed size type

// ********** <note> ************
// concept NodeT {
//	value_type value;
// 	NodeT* left;
// 	NodeT* right
// 	NodeT* parent;
// };
// ********** </note> ***********

template <typename NodeT>
inline void set_left(NodeT* root, NodeT* new_left);

template <typename NodeT>
inline void set_right(NodeT* root, NodeT* new_right);

template <typename NodeT>
inline void replace_child(NodeT* old_child, NodeT* new_child);

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
//	NodeT* or const NodeT*
// };
// ********** </note> ***********

// ********** <note> ************
// concept UnaryFunction {
// 	UnaryFunction op;
// 	NodePtrT p;
// 	requires op(p->value);
// };
// ********** </note> ***********

template <typename NodePtrT, typename UnaryFunction>
void traverse_preorder(NodePtrT root, UnaryFunction op);

template <typename NodePtrT, typename UnaryFunction>
void traverse_inorder(NodePtrT root, UnaryFunction op);

template <typename NodePtrT, typename UnaryFunction>
void traverse_postorder(NodePtrT root, UnaryFunction op);

template <typename NodePtrT, typename UnaryFunction>
void traverse_level_order(NodePtrT root, UnaryFunction op);

template <typename NodeT>
ssize_type height(const NodeT* root);  // NULL tree's height is -1

template <typename NodeT>
size_type size(const NodeT* root);  // how many nodes in tree

// search value in binary-search tree
// return node that contains value; if not found, return NULL
template <typename NodePtrT, typename T>
NodePtrT find(NodePtrT root, const T& value);

// binary-search tree insertion
// return node that contains new_node->value
template <typename NodeT>
NodeT* insert(NodeT*& root, NodeT* new_node);

namespace avl {  // AVL tree algorithm

// ********** <note> ************
// concept AVLNodeT : public NodeT{
//	ssize_type height;
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
template <typename AVLNodeT>
AVLNodeT* insert(AVLNodeT*& root, AVLNodeT* new_node);

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

}  // namespace tree

#include "tree-inl.h"

#endif  // TREE_H_
