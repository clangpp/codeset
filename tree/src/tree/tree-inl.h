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
void replace_child(NodeT* old_child, NodeT* new_child) {
    new_child->parent = old_child->parent;
    if (NULL==old_child->parent) return;
    if (old_child==old_child->parent->left) {
        old_child->parent->left = new_child;
    } else {
        old_child->parent->right = new_child;
    }
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
void rotate_left_left(NodeT*& root) {
	NodeT* new_root = root->left;
    replace_child(root, new_root);
	set_left(root, new_root->right);  // re-root new_root->right
	set_right(new_root, root);  // reform root, new_root
	root = new_root;
}

template <typename NodeT>
void rotate_right_right(NodeT*& root) {
	NodeT* new_root = root->right;
    replace_child(root, new_root);
	set_right(root, new_root->left);  // re-root new_root->left
	set_left(new_root, root);  // reform root, new_root
	root = new_root;
}

template <typename NodeT>
void rotate_left_right(NodeT*& root) {
	root = internal::double_rotation(
            root, root->left, root, root->left->right);
}

template <typename NodeT>
void rotate_right_left(NodeT*& root) {
	root = internal::double_rotation(
            root, root, root->right, root->right->left);
}

namespace internal {

// note: return new_root is necessary,
//	because left/right children of original root are changed
template <typename NodeT>
NodeT* double_rotation(
        NodeT* old_root, NodeT* new_left, NodeT* new_right, NodeT* new_root) {
    // re-parent new_root
    replace_child(old_root, new_root);

	// re-root new_root->left and new_root->right
	set_right(new_left, new_root->left);
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
ssize_type height(const NodeT* root) {
	if (NULL==root) return -1;
	return 1 + std::max(height(root->left), height(root->right));
}

template <typename NodePtrT, typename T>
NodePtrT find(NodePtrT root, const T& value) {
	while (root) {
		if (value < root->value) {
			root = root->left;
		} else if (root->value < value) {
			root = root->right;
		} else {
			return root;
		}
	}
	return root;
}

template <typename NodeT>
NodeT* insert(NodeT*& root, NodeT* new_node) {
    new_node->parent = new_node->left = new_node->right = NULL;
	if (NULL==root) {  // new_node become root of tree
		root = new_node;
		return new_node;
	}

    NodeT* curr = root;
    while (true) {
        if (new_node->value < curr->value) {  // insert on left subtree
            if (curr->left) {  // left subtree is not empty
                curr = curr->left;
            } else {  // left subtree is empty
                set_left(curr, new_node);
                return new_node;
            }
        } else if (curr->value < new_node->value) {  // insert on right subtree
            if (curr->right) {  // right subtree is not empty
                curr = curr->right;
            } else {  // right subtree is empty
                set_right(curr, new_node);
                return new_node;
            }
        } else {
            return curr;
        }
    }
}

namespace avl {  // AVL tree algorithm

template <typename AVLNodeT>
ssize_type height(const AVLNodeT* root) { return root ? root->height : -1; }

template <typename AVLNodeT>
void update_height(AVLNodeT* root) {
    if (NULL==root) return;
    root->height = 1 + 
        std::max(avl::height(root->left), avl::height(root->right));
}

template <typename AVLNodeT>
void rotate_left_left(AVLNodeT*& root) {
    tree::rotate_left_left(root);
    avl::update_height(root->right);
    avl::update_height(root);
}

template <typename AVLNodeT>
void rotate_right_right(AVLNodeT*& root) {
    tree::rotate_right_right(root);
    avl::update_height(root->left);
    avl::update_height(root);
}

template <typename AVLNodeT>
void rotate_left_right(AVLNodeT*& root) {
    tree::rotate_left_right(root);
    avl::update_height(root->left);
    avl::update_height(root->right);
    avl::update_height(root);
}

template <typename AVLNodeT>
void rotate_right_left(AVLNodeT*& root) {
    tree::rotate_right_left(root);
    avl::update_height(root->left);
    avl::update_height(root->right);
    avl::update_height(root);
}

template <typename AVLNodeT>
AVLNodeT* insert(AVLNodeT*& root, AVLNodeT* new_node) {
    AVLNodeT* position = tree::insert(root, new_node);  // bs-tree insertion
    if (position != new_node) return position;  // already exists

    // rebalance AVL tree
    for (AVLNodeT* p=new_node; p!=NULL; root=p, p=p->parent) {
        if (avl::height(p->left) >= avl::height(p->right)+2) {
            if (new_node->value < p->left->value) {  // left-left
                avl::rotate_left_left(p);
            } else {  // left-right
                avl::rotate_left_right(p);
            }
        } else if (avl::height(p->right) >= avl::height(p->left)+2) {
            if (p->right->value < new_node->value) {  // right-right
                avl::rotate_right_right(p);
            } else {  // right-left
                avl::rotate_right_left(p);
            }
        } else {  // no need for rebalance
            update_height(p);  // update height field
        }
    }
    return position;
}

}  // namespace avl

namespace splay {

template <typename NodeT>
NodeT* adjust(NodeT* target) {
    while (target->parent) {  // do until target become root

        // self's parent is root, rotate self and parent
        NodeT* p = target->parent;
        if (NULL==p->parent) {
            replace_child(p, target);
            if (target == p->left) {
                set_left(p, target->right);
                set_right(target, p);
            } else {
                set_right(p, target->left);
                set_left(target, p);
            }
            continue;
        }

        // self has grandparent, 4 kinds of rotation
        NodeT* g = p->parent;
        if (p==g->left && target==p->left) {  // left-left
            replace_child(g, target);
            set_left(g, p->right);
            set_left(p, target->right);
            set_right(p, g);
            set_right(target, p);
        } else if (p==g->right && target==p->right) {  // right-right
            replace_child(g, target);
            set_right(g, p->left);
            set_right(p, target->left);
            set_left(p, g);
            set_left(target, p);
        } else if (p==g->left && target==p->right) {  // left-right
            rotate_left_right(g);  // double rotation
        } else if (p==g->right && target==p->left) {  // right-left
            rotate_right_left(g);  // double rotation
        }
    }
    return target;
}

}  // namespace splay

}  // namespace tree

#endif  // TREE_INL_H_
