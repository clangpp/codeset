// tree.h
#ifndef TREE_H_
#define TREE_H_

#include <iostream>
#include <vector>

namespace tree {

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
NodeT* rotate_left_left(NodeT* root);  // left-left single rotation

template <typename NodeT>
NodeT* rotate_right_right(NodeT* root);  // right-right single rotation

template <typename NodeT>
inline NodeT* rotate_left_right(NodeT* root);  // left-right double rotation

template <typename NodeT>
inline NodeT* rotate_right_left(NodeT* root);  // right-left double rotation

namespace internal {

template <typename NodeT>
NodeT* double_rotation(NodeT* new_left, NodeT* new_right, NodeT* new_root);

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

// search value in binary-search tree
// return node pointer that contains value; if not found, return NULL
template <typename NodePtrT, typename T>
NodePtrT find(NodePtrT root, const T& value);

namespace avl {  // AVL tree algorithm

// ********** <note> ************
// concept AVLNodeT : public NodeT{
//	ssize_type height;
// };
// ********** </note> ***********

template <typename AVLNodeT>
inline ssize_type height(const AVLNodeT* root);

}  // namespace avl

}  // namespace tree

#include "tree-inl.h"

#endif  // TREE_H_
