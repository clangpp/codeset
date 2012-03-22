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
	set_left(root, new_root->right);

	// reform root, new_root
	set_right(new_root, root);
	return new_root;
}

template <typename NodeT>
NodeT* single_rotation_right(NodeT* root) {
	NodeT* new_root = root->right;
	
	// re-root new_root->left
	set_right(root, new_root->left);

	// reform root, new_root
	set_left(new_root, root);
	return new_root;
}

template <typename NodeT>
NodeT* double_rotation_left(NodeT* root) {
	return internal::double_rotation(root->left, root, root->left->right);
}

template <typename NodeT>
NodeT* double_rotation_right(NodeT* root) {
	return internal::double_rotation(root, root->right, root->right->left);
}

namespace internal {

template <typename NodeT>
void set_left(NodeT* root, NodeT* new_left) {
	root->left = new_left;
	if (new_left) new_left->parent = root;
}

template <typename NodeT>
void set_right(NodeT* root, NodeT* new_right) {
	root->right = new_right;
	if (new_right) new_right->parent = root;
}

// note: return new_root is necessary,
//	because left/right children of original root are changed
template <typename NodeT>
NodeT* double_rotation(NodeT* new_left, NodeT* new_right, NodeT* new_root) {
	// re-root new_root->left
	set_right(new_left, new_root->left);

	// re-root new_root->right
	set_left(new_right, new_root->right);

	// reform new_root, new_left, new_right
	set_left(new_root, new_left);
	set_right(new_root, new_right);
	return new_root;
}

}  // namespace internal

template <typename NodePtrT, typename UnaryFunction>
void inorder_traversal(NodePtrT root, UnaryFunction op) {
	if (NULL==root) return;
	inorder_traversal(root->left, op);
	op(root->value);
	inorder_traversal(root->right, op);
}

template <typename NodePtrT, typename UnaryFunction>
void postorder_traversal(NodePtrT root, UnaryFunction op) {
	if (NULL==root) return;
	postorder_traversal(root->left, op);
	postorder_traversal(root->right, op);
	op(root->value);
}

template <typename NodePtrT, typename UnaryFunction>
void preorder_traversal(NodePtrT root, UnaryFunction op) {
	if (NULL==root) return;
	op(root->value);
	preorder_traversal(root->left, op);
	preorder_traversal(root->right, op);
}

template <typename NodePtrT, typename UnaryFunction>
void level_order_traversal(NodePtrT root, UnaryFunction op) {
	if (NULL==root) return;

	// initalize node queue
	queue<NodePtrT> node_q;
	node_q.push(root);

	// access iterations
	while (!node_q.empty()) {
		NodePtrT curr = node_q.top();
		op(curr->value);  // access node value

		// maintain node queue
		node_q.pop();
		if (curr->left) node_q.push(curr->left);
		if (curr->right) node_q.push(curr->right);
	}
}

template <typename NodeT>
void print_tree(const NodeT* root, std::ostream& os) {
	std::vector<int> path;
	internal::print_tree(os, root, path);
}

namespace internal {

enum PathBitMask { BM_LEFT=0x01, BM_RIGHT=0x02, BM_BROTHER=0x04 };

template <typename NodeT>
void print_tree(std::ostream& os, const NodeT* root, std::vector<int>& path) {
	if (NULL==root) return;

	// print root value
	if (!path.empty()) {
		for (size_t i=0; i<path.size()-1; ++i) {
			bool more = (path[i] & BM_BROTHER) && (path[i] & BM_LEFT);
			os << (more ? "|   " : "    ");
		}
		os << ((path.back() & BM_LEFT) ? "|---" : "|===");
	}
	os << root->value << endl;

	// print children
	int brother_mask = (root->left && root->right) ? BM_BROTHER : 0;

	path.push_back(BM_LEFT|brother_mask);
	print_tree(os, root->left, path);
	path.pop_back();

	path.push_back(BM_RIGHT|brother_mask);
	print_tree(os, root->right, path);
	path.pop_back();
}

}  // namespace internal

}  // namespace tree

#endif  // TREE_INL_H_
