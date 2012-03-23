// tree-inl.h
#ifndef TREE_INL_H_
#define TREE_INL_H_

#include "tree.h"

#include <algorithm>
#include <cstddef>
#include <queue>

namespace tree {

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

template <typename NodeT>
void print(const NodeT* root, std::ostream& out) { print_inorder(root, out); }

template <typename NodeT>
void print_preorder(const NodeT* root, std::ostream& out) {
	std::vector<int> path;
	internal::print_preorder(out, root, path);
}

template <typename NodeT>
void print_inorder(const NodeT* root, std::ostream& out) {
	std::vector<int> path;
	internal::print_inorder(out, root, path);
}

template <typename NodeT>
void print_postorder(const NodeT* root, std::ostream& out) {
	std::vector<int> path;
	internal::print_postorder(out, root, path);
}

namespace internal {

enum PathBitMask { BM_LEFT=0x01, BM_RIGHT=0x02, BM_BROTHER=0x04 };

template <typename NodeT>
void print_preorder(std::ostream& out,
		const NodeT* root, std::vector<int>& path) {
	if (NULL==root) return;

	// print root value
	if (!path.empty()) {
		bool more = true; size_t i = 0;
		for (i=0; i<path.size()-1; ++i) {
			more = (path[i] & BM_BROTHER) && (path[i] & BM_LEFT);
			out << (more ? "|   " : "    ");
		}
		more = (path[i] & BM_BROTHER) && (path[i] & BM_LEFT);
		out << (more ? "|" : "`") << ((path[i] & BM_LEFT) ? "---" : "===");
	}
	out << root->value << std::endl;

	// print children
	int brother_mask = (root->left && root->right) ? BM_BROTHER : 0;

	path.push_back(BM_LEFT|brother_mask);
	print_preorder(out, root->left, path);
	path.pop_back();

	path.push_back(BM_RIGHT|brother_mask);
	print_preorder(out, root->right, path);
	path.pop_back();
}

template <typename NodeT>
void print_inorder(std::ostream& out,
		const NodeT* root, std::vector<int>& path) {
	if (NULL==root) return;

	// print left child
	path.push_back(-1);
	print_inorder(out, root->left, path);
	path.pop_back();

	// print root value
	if (!path.empty()) {
		bool more = true; size_t i = 0;
		for (i=0; i<path.size()-1; ++i) {
			more = path[i]*path[i+1] < 0;  // opposite direction
			out << (more ? "|   " : "    ");
		}
		out << (path[i]<0 ? ",---" : "`===");
	}
	out << root->value << std::endl;

	// print right child
	path.push_back(1);
	print_inorder(out, root->right, path);
	path.pop_back();
}

template <typename NodeT>
void print_postorder(std::ostream& out,
		const NodeT* root, std::vector<int>& path) {
	if (NULL==root) return;

	// print children
	int brother_mask = (root->left && root->right) ? BM_BROTHER : 0;

	path.push_back(BM_LEFT|brother_mask);
	print_postorder(out, root->left, path);
	path.pop_back();

	path.push_back(BM_RIGHT|brother_mask);
	print_postorder(out, root->right, path);
	path.pop_back();

	// print root value
	if (!path.empty()) {
		bool more = true; size_t i = 0;
		for (i=0; i<path.size()-1; ++i) {
			more = (path[i] & BM_BROTHER) && (path[i] & BM_RIGHT);
			out << (more ? "|   " : "    ");
		}
		more = (path[i] & BM_BROTHER) && (path[i] & BM_RIGHT);
		out << (more ? "|" : ",") << ((path[i] & BM_LEFT) ? "---" : "===");
	}
	out << root->value << std::endl;
}

}  // namespace internal

template <typename NodeT>
NodeT* rotate_left_left(NodeT* root) {
	NodeT* new_root = root->left;
	
	// re-root new_root->right
	set_left(root, new_root->right);

	// reform root, new_root
	set_right(new_root, root);
	return new_root;
}

template <typename NodeT>
NodeT* rotate_right_right(NodeT* root) {
	NodeT* new_root = root->right;
	
	// re-root new_root->left
	set_right(root, new_root->left);

	// reform root, new_root
	set_left(new_root, root);
	return new_root;
}

template <typename NodeT>
NodeT* rotate_left_right(NodeT* root) {
	return internal::double_rotation(root->left, root, root->left->right);
}

template <typename NodeT>
NodeT* rotate_right_left(NodeT* root) {
	return internal::double_rotation(root, root->right, root->right->left);
}

namespace internal {

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
void traverse_preorder(NodePtrT root, UnaryFunction op) {
	if (NULL==root) return;
	op(root->value);
	traverse_preorder(root->left, op);
	traverse_preorder(root->right, op);
}

template <typename NodePtrT, typename UnaryFunction>
void traverse_inorder(NodePtrT root, UnaryFunction op) {
	if (NULL==root) return;
	traverse_inorder(root->left, op);
	op(root->value);
	traverse_inorder(root->right, op);
}

template <typename NodePtrT, typename UnaryFunction>
void traverse_postorder(NodePtrT root, UnaryFunction op) {
	if (NULL==root) return;
	traverse_postorder(root->left, op);
	traverse_postorder(root->right, op);
	op(root->value);
}

template <typename NodePtrT, typename UnaryFunction>
void traverse_level_order(NodePtrT root, UnaryFunction op) {
	if (NULL==root) return;

	// initalize node queue
	std::queue<NodePtrT> node_q;
	node_q.push(root);

	// access iterations
	while (!node_q.empty()) {
		NodePtrT curr = node_q.front();
		op(curr->value);  // access node value

		// maintain node queue
		node_q.pop();
		if (curr->left) node_q.push(curr->left);
		if (curr->right) node_q.push(curr->right);
	}
}

template <typename NodeT>
long height(const NodeT* root) {
	if (NULL==root) return -1;
	return 1 + std::max(height(root->left), height(root->right));
}

}  // namespace tree

#endif  // TREE_INL_H_
