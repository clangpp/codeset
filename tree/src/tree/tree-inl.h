// tree-inl.h
#ifndef TREE_INL_H_
#define TREE_INL_H_

#include "tree.h"

#include <cstddef>
#include <queue>

namespace tree {

template <typename NodeT>
NodeT* single_rotation_left(NodeT* root) {
	NodeT* new_root = root->left;
	
	// re-root new_root->right
	root->left = new_root->right;
	if (root->left)
		root->left->parent = root;

	// reform root, new_root
	new_root->right = root;
	root->parent = new_root;
	return new_root;
}

template <typename NodeT>
NodeT* single_rotation_right(NodeT* root) {
	NodeT* new_root = root->right;
	
	// re-root new_root->left
	root->right = new_root->left;
	if (root->right)
		root->right->parent = root;

	// reform root, new_root
	new_root->left = root;
	root->parent = new_root;
	return new_root;
}

template <typename NodeT>
NodeT* double_rotation_left(NodeT* root) {
	return double_rotation(root->left, root, root->left->right);
}

template <typename NodeT>
NodeT* double_rotation_right(NodeT* root) {
	return double_rotation(root, root->right, root->right->left);
}

namespace internal {

// note: return new_root is necessary,
//	because left/right children of original root are changed
template <typename NodeT>
NodeT* double_rotation(NodeT* new_left, NodeT* new_right, NodeT* new_root) {
	// re-root new_root->left
	new_left->right = new_root->left;
	if (new_left->right)
		new_left->right->parent = new_left;

	// re-root new_root->right
	new_right->left = new_root->right;
	if (new_right->left)
		new_right->left->parent = new_right;

	// reform new_root, new_left, new_right
	new_root->left = new_left;
	new_left->parent = new_root;
	new_root->right = new_right;
	new_right->parent = new_root;
	return new_root;
}

}  // namespace internal

template <typename NodeT, typename UnaryFunction>
void inorder_traversal(NodeT* root, UnaryFunction op) {
	if (NULL==root) return;
	inorder_traversal(root->left, op);
	op(root->value);
	inorder_traversal(root->right, op);
}

template <typename NodeT, typename UnaryFunction>
void postorder_traversal(NodeT* root, UnaryFunction op) {
	if (NULL==root) return;
	postorder_traversal(root->left, op);
	postorder_traversal(root->right, op);
	op(root->value);
}

template <typename NodeT, typename UnaryFunction>
void preorder_traversal(NodeT* root, UnaryFunction op) {
	if (NULL==root) return;
	op(root->value);
	preorder_traversal(root->left, op);
	preorder_traversal(root->right, op);
}

template <typename NodeT, typename UnaryFunction>
void level_order_traversal(NodeT* root, UnaryFunction op) {
	if (NULL==root) return;

	// initalize node queue
	queue<NodeT*> node_q;
	node_q.push(root);

	// access iterations
	while (!node_q.empty()) {
		NodeT* curr = node_q.top();
		op(curr->value);  // access node value

		// maintain node queue
		node_q.pop();
		if (curr->left) node_q.push(curr->left);
		if (curr->right) node_q.push(curr->right);
	}
}

}  // namespace tree

#endif  // TREE_INL_H_
