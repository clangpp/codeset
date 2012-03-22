// tree.h
#ifndef TREE_H_
#define TREE_H_

namespace tree {

// ********** concept ***********
// concept NodeT {
//	value_type value;
// 	NodeT* left;
// 	NodeT* right
// 	NodeT* parent;
// };
// ********** concept ***********

template <typename NodeT>
NodeT* single_rotation_left(NodeT* root);  // left-left

template <typename NodeT>
NodeT* single_rotation_right(NodeT* root);  // right-right

template <typename NodeT>
NodeT* double_rotation_left(NodeT* root);  // left-right

template <typename NodeT>
NodeT* double_rotation_right(NodeT* root);  // right-left

namespace internal {

template <typename NodeT>
NodeT* double_rotation(NodeT* new_left, NodeT* new_right, NodeT* new_root);

}  // namespace internal

template <typename NodeT, typename UnaryFunction>
void inorder_traversal(NodeT* root, UnaryFunction op);

template <typename NodeT, typename UnaryFunction>
void postorder_traversal(NodeT* root, UnaryFunction op);

template <typename NodeT, typename UnaryFunction>
void preorder_traversal(NodeT* root, UnaryFunction op);

template <typename NodeT, typename UnaryFunction>
void level_order_traversal(NodeT* root, UnaryFunction op);

}  // namespace tree

#include "tree-inl.h"

#endif  // TREE_H_
