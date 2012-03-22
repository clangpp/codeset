// tree.h
#ifndef TREE_H_
#define TREE_H_

#include <iostream>
#include <vector>

namespace tree {

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
inline void print(const NodeT* root, std::ostream& os = std::cout);

namespace internal {

template <typename NodeT>
void print(std::ostream& os, const NodeT* root, std::vector<int>& path);

}  // namespace internal

template <typename NodeT>
NodeT* single_rotation_left(NodeT* root);  // left-left

template <typename NodeT>
NodeT* single_rotation_right(NodeT* root);  // right-right

template <typename NodeT>
inline NodeT* double_rotation_left(NodeT* root);  // left-right

template <typename NodeT>
inline NodeT* double_rotation_right(NodeT* root);  // right-left

namespace internal {

template <typename NodeT>
NodeT* double_rotation(NodeT* new_left, NodeT* new_right, NodeT* new_root);

}  // namespace internal

// ********** <note> ************
// concept UnaryFunction {
// 	UnaryFunction op;
// 	NodePtrT p;  // NodeT* or const NodeT*
// 	requires op(p->value);
// };
// ********** </note> ***********

template <typename NodePtrT, typename UnaryFunction>
void inorder_traversal(NodePtrT root, UnaryFunction op);

template <typename NodePtrT, typename UnaryFunction>
void postorder_traversal(NodePtrT root, UnaryFunction op);

template <typename NodePtrT, typename UnaryFunction>
void preorder_traversal(NodePtrT root, UnaryFunction op);

template <typename NodePtrT, typename UnaryFunction>
void level_order_traversal(NodePtrT root, UnaryFunction op);

template <typename NodeT>
long height(const NodeT* root);

}  // namespace tree

#include "tree-inl.h"

#endif  // TREE_H_
