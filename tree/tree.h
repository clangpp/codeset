#ifndef TREE_H_
#define TREE_H_

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iostream>
#include <iterator>
#include <queue>
#include <stack>
#include <stdexcept>
#include <vector>

namespace tree {

typedef std::size_t size_type;      // unsigned size type
typedef std::ptrdiff_t ssize_type;  // signed size type

// concept NodeT {
//     using value_type = <implemention-defined type>;
//     value_type value;
//     NodeT* left;
//     NodeT* right
//     NodeT* parent;
// };

template <typename NodeT>
void set_left(NodeT* root, NodeT* new_left) {
  root->left = new_left;
  if (new_left) {
    new_left->parent = root;
  }
}

template <typename NodeT>
void set_right(NodeT* root, NodeT* new_right) {
  root->right = new_right;
  if (new_right) {
    new_right->parent = root;
  }
}

template <typename NodeT>
void replace_child(NodeT* old_child, NodeT* new_child) {
  new_child->parent = old_child->parent;
  if (nullptr == old_child->parent) {
    return;
  }
  if (old_child == old_child->parent->left) {
    old_child->parent->left = new_child;
  } else {
    old_child->parent->right = new_child;
  }
}

template <typename NodeT>
void destory(NodeT*& root) {
  if (nullptr == root) {
    return;
  }
  destory(root->left);
  destory(root->right);
  delete root;
  root = nullptr;
}

namespace internal {

enum PathBitMask { BM_LEFT = 0x01, BM_RIGHT = 0x02, BM_BROTHER = 0x04 };

template <typename NodeT>
void print_preorder(std::ostream& out, const NodeT* root,
                    std::vector<int>& path) {
  if (nullptr == root) {
    return;
  }

  // print root value
  if (!path.empty()) {
    bool more = true;
    size_t i = 0;
    for (i = 0; i < path.size() - 1; ++i) {
      more = (path[i] & BM_BROTHER) && (path[i] & BM_LEFT);
      out << (more ? "|   " : "    ");
    }
    more = (path[i] & BM_BROTHER) && (path[i] & BM_LEFT);
    out << (more ? "|" : "`") << ((path[i] & BM_LEFT) ? "---" : "===");
  }
  out << root->value << std::endl;

  // print children
  int brother_mask = (root->left && root->right) ? BM_BROTHER : 0;

  path.push_back(BM_LEFT | brother_mask);
  print_preorder(out, root->left, path);
  path.pop_back();

  path.push_back(BM_RIGHT | brother_mask);
  print_preorder(out, root->right, path);
  path.pop_back();
}

template <typename NodeT>
void print_inorder(std::ostream& out, const NodeT* root,
                   std::vector<int>& path) {
  if (nullptr == root) {
    return;
  }

  // print left child
  path.push_back(-1);
  print_inorder(out, root->left, path);
  path.pop_back();

  // print root value
  if (!path.empty()) {
    bool more = true;
    size_t i = 0;
    for (i = 0; i < path.size() - 1; ++i) {
      more = path[i] * path[i + 1] < 0;  // opposite direction
      out << (more ? "|   " : "    ");
    }
    out << (path[i] < 0 ? ",---" : "`===");
  }
  out << root->value << std::endl;

  // print right child
  path.push_back(1);
  print_inorder(out, root->right, path);
  path.pop_back();
}

template <typename NodeT>
void print_postorder(std::ostream& out, const NodeT* root,
                     std::vector<int>& path) {
  if (nullptr == root) {
    return;
  }

  // print children
  int brother_mask = (root->left && root->right) ? BM_BROTHER : 0;

  path.push_back(BM_LEFT | brother_mask);
  print_postorder(out, root->left, path);
  path.pop_back();

  path.push_back(BM_RIGHT | brother_mask);
  print_postorder(out, root->right, path);
  path.pop_back();

  // print root value
  if (!path.empty()) {
    bool more = true;
    size_t i = 0;
    for (i = 0; i < path.size() - 1; ++i) {
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
void print_preorder(const NodeT* root, std::ostream& out = std::cout) {
  std::vector<int> path;
  internal::print_preorder(out, root, path);
}

template <typename NodeT>
void print_inorder(const NodeT* root, std::ostream& out = std::cout) {
  std::vector<int> path;
  internal::print_inorder(out, root, path);
}

template <typename NodeT>
void print_postorder(const NodeT* root, std::ostream& out = std::cout) {
  std::vector<int> path;
  internal::print_postorder(out, root, path);
}

template <typename NodeT>
void print(const NodeT* root, std::ostream& out = std::cout) {
  print_inorder(root, out);
}

// left-left single rotation
template <typename NodeT>
void rotate_left_left(NodeT*& root) {
  NodeT* new_root = root->left;
  replace_child(root, new_root);
  set_left(root, new_root->right);  // re-root new_root->right
  set_right(new_root, root);        // reform root, new_root
  root = new_root;
}

// right-right single rotation
template <typename NodeT>
void rotate_right_right(NodeT*& root) {
  NodeT* new_root = root->right;
  replace_child(root, new_root);
  set_right(root, new_root->left);  // re-root new_root->left
  set_left(new_root, root);         // reform root, new_root
  root = new_root;
}

namespace internal {

// note: return new_root is necessary,
//    because left/right children of original root are changed
template <typename NodeT>
NodeT* double_rotation(NodeT* old_root, NodeT* new_left, NodeT* new_right,
                       NodeT* new_root) {
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

// left-right double rotation
template <typename NodeT>
void rotate_left_right(NodeT*& root) {
  root = internal::double_rotation(root, root->left, root, root->left->right);
}

// right-left double rotation
template <typename NodeT>
void rotate_right_left(NodeT*& root) {
  root = internal::double_rotation(root, root, root->right, root->right->left);
}

// concept NodePtrT {
//     NodeT* or const NodeT*
// };

// concept UnaryFunction {
//     UnaryFunction op;
//     NodePtrT p;
//     requires op(p->value);
// };

template <typename NodePtrT, typename UnaryFunction>
void traverse_preorder_recursive(NodePtrT root, UnaryFunction op) {
  if (nullptr == root) {
    return;
  }
  op(root->value);
  traverse_preorder_recursive(root->left, op);
  traverse_preorder_recursive(root->right, op);
}

template <typename NodePtrT, typename UnaryFunction>
void traverse_inorder_recursive(NodePtrT root, UnaryFunction op) {
  if (nullptr == root) {
    return;
  }
  traverse_inorder_recursive(root->left, op);
  op(root->value);
  traverse_inorder_recursive(root->right, op);
}

template <typename NodePtrT, typename UnaryFunction>
void traverse_postorder_recursive(NodePtrT root, UnaryFunction op) {
  if (nullptr == root) {
    return;
  }
  traverse_postorder_recursive(root->left, op);
  traverse_postorder_recursive(root->right, op);
  op(root->value);
}

template <typename NodePtrT, typename UnaryFunction>
void traverse_preorder_stack(NodePtrT root, UnaryFunction op) {
  std::stack<NodePtrT> s;
  s.push(root);
  while (!s.empty()) {
    NodePtrT curr = s.top();
    s.pop();
    if (nullptr == curr) {
      continue;
    }
    op(curr->value);      // visit current node firstly
    s.push(curr->right);  // visit right subtree thirdly
    s.push(curr->left);   // visit left subtree secondly
  }
}

template <typename NodePtrT, typename UnaryFunction>
void traverse_inorder_stack(NodePtrT root, UnaryFunction op) {
  std::stack<std::pair<NodePtrT, bool> > s;  // bool expanded
  s.push(std::make_pair(root, false));
  while (!s.empty()) {
    NodePtrT curr = s.top().first;
    bool expanded = s.top().second;
    s.pop();
    if (nullptr == curr) {
      continue;
    }
    if (expanded) {
      op(curr->value);  // visit current node
      continue;
    }

    // expand current subtree, push right, root, left
    s.push(std::make_pair(curr->right, false));  // visit right subtree thirdly
    s.push(std::make_pair(curr, true));          // visit current node secondly
    s.push(std::make_pair(curr->left, false));   // visit left subtree firstly
  }
}

template <typename NodePtrT, typename UnaryFunction>
void traverse_postorder_stack(NodePtrT root, UnaryFunction op) {
  std::stack<std::pair<NodePtrT, bool> > s;  // bool expanded
  s.push(std::make_pair(root, false));
  while (!s.empty()) {
    NodePtrT curr = s.top().first;
    bool expanded = s.top().second;
    s.pop();
    if (nullptr == curr) {
      continue;
    }
    if (expanded) {
      op(curr->value);  // visit current node
      continue;
    }

    // expand current subtree, push root, right, left
    s.push(std::make_pair(curr, true));          // visit current node thirdly
    s.push(std::make_pair(curr->right, false));  // visit right subtree secondly
    s.push(std::make_pair(curr->left, false));   // visit left subtree firstly
  }
}

template <typename NodePtrT, typename UnaryFunction>
void traverse_preorder_tricky(NodePtrT root, UnaryFunction op) {
  std::stack<NodePtrT> s;
  NodePtrT curr = root;
  while (!s.empty() || curr != nullptr) {
    if (curr != nullptr) {
      op(curr->value);  // visit before pushed
      s.push(curr);
      curr = curr->left;
    } else {
      curr = s.top();
      s.pop();
      curr = curr->right;
    }
  }
}

template <typename NodePtrT, typename UnaryFunction>
void traverse_inorder_tricky(NodePtrT root, UnaryFunction op) {
  std::stack<NodePtrT> s;
  NodePtrT curr = root;
  while (!s.empty() || curr != nullptr) {
    if (curr != nullptr) {
      s.push(curr);
      curr = curr->left;
    } else {
      curr = s.top();
      op(curr->value);  // visit before popped
      s.pop();
      curr = curr->right;
    }
  }
}

// template <typename NodePtrT, typename UnaryFunction>
// void traverse_postorder_tricky(NodePtrT root, UnaryFunction op) {
//     std::stack<std::pair<NodePtrT, NodePtrT> > s;
//     const NodePtrT RIGHT_VISITED_FLAG = nullptr;
//     NodePtrT curr=root;
//     while (!s.empty() || curr!=nullptr) {
//         if (curr!=nullptr) {
//             s.push(std::make_pair(curr, curr->right));
//             curr = curr->left;
//             continue;
//         }

//         NodePtrT self = s.top().first;
//         NodePtrT right = s.top().second;
//         s.pop();
//         if (RIGHT_VISITED_FLAG==right) {
//             op(self->value);  // visit after right subtree visited
//             continue;
//         }

//         s.push(std::make_pair(self, RIGHT_VISITED_FLAG));
//         s.push(std::make_pair(right, right->right));
//         curr = right->left;
//     }
// }

// This is Wu Lihua's original idea (2012.08.06), I implement it in a way.
// Basic idea: every left node, is acccessed through its parent.
// Additional work: every right node (root node included), has to be
//    accessed in other ways, I choose a separate stack.
template <typename NodePtrT, typename UnaryFunction>
void traverse_postorder_tricky(NodePtrT root, UnaryFunction op) {
  std::stack<NodePtrT> s, s_rtrace;
  NodePtrT curr = root;
  while (!s.empty() || curr != nullptr) {
    while (curr != nullptr) {  // go deepest into left direction
      s.push(curr);
      curr = curr->left;
    }

    NodePtrT parent = s.top();  // pop a left node
    s.pop();

    curr = parent->left;  // access its left child
    if (curr != nullptr) {
      op(curr->value);
    }

    curr = parent->right;  // curr turn to right subtree

    s_rtrace.push(parent);  // record every popped node into another stack
    if (nullptr == curr) {  // access right direction path, in reverse order
      NodePtrT sentry = s.empty() ? nullptr : s.top();
      while (!s_rtrace.empty()) {
        NodePtrT trace = s_rtrace.top();
        s_rtrace.pop();
        if (sentry != nullptr && trace == sentry->left) {
          break;
        }
        op(trace->value);
      }
    }
  }
}

template <typename NodePtrT, typename UnaryFunction>
void traverse_preorder(NodePtrT root, UnaryFunction op) {
  traverse_preorder_recursive(root, op);
}

template <typename NodePtrT, typename UnaryFunction>
void traverse_inorder(NodePtrT root, UnaryFunction op) {
  traverse_inorder_recursive(root, op);
}

template <typename NodePtrT, typename UnaryFunction>
void traverse_postorder(NodePtrT root, UnaryFunction op) {
  traverse_postorder_recursive(root, op);
}

template <typename NodePtrT, typename UnaryFunction>
void traverse_level_order(NodePtrT root, UnaryFunction op) {
  if (nullptr == root) {
    return;
  }

  // initalize node queue
  std::queue<NodePtrT> node_q;
  node_q.push(root);

  // access iterations
  while (!node_q.empty()) {
    NodePtrT curr = node_q.front();
    op(curr->value);  // access node value

    // maintain node queue
    node_q.pop();
    if (curr->left) {
      node_q.push(curr->left);
    }
    if (curr->right) {
      node_q.push(curr->right);
    }
  }
}

// nullptr tree's height is -1
template <typename NodeT>
ssize_type height(const NodeT* root) {
  if (nullptr == root) {
    return -1;
  }
  return 1 + std::max(height(root->left), height(root->right));
}

// how many nodes in tree
template <typename NodeT>
size_type count(const NodeT* root) {
  if (nullptr == root) {
    return 0;
  }
  return 1 + count(root->left) + count(root->right);
}

// search value in binary-search tree
// return node that contains value; if not found, return nullptr
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

// binary-search tree insertion
// return node that contains new_node->value
template <typename NodeT, typename BinaryPredicate>
NodeT* insert(NodeT*& root, NodeT* new_node, BinaryPredicate pred) {
  new_node->parent = new_node->left = new_node->right = nullptr;
  if (nullptr == root) {  // new_node become root of tree
    root = new_node;
    return new_node;
  }

  NodeT* curr = root;
  while (true) {
    if (pred(new_node->value, curr->value)) {  // insert on left subtree
      if (curr->left) {                        // left subtree is not empty
        curr = curr->left;
      } else {  // left subtree is empty
        set_left(curr, new_node);
        return new_node;
      }
    } else if (pred(curr->value, new_node->value)) {  // insert on right subtree
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

template <typename NodeT>
NodeT* insert(NodeT*& root, NodeT* new_node) {
  return insert(root, new_node, std::less<typename NodeT::value_type>());
}

namespace avl {  // AVL tree algorithm

// concept AVLNodeT : public NodeT {
//     ssize_type height;
// };

template <typename AVLNodeT>
ssize_type height(const AVLNodeT* root) {
  return root ? root->height : -1;
}

template <typename AVLNodeT>
void update_height(AVLNodeT* root) {
  if (nullptr == root) {
    return;
  }
  root->height =
      1 + std::max(avl::height(root->left), avl::height(root->right));
}

// left-left single rotation
template <typename AVLNodeT>
void rotate_left_left(AVLNodeT*& root) {
  tree::rotate_left_left(root);
  avl::update_height(root->right);
  avl::update_height(root);
}

// right-right single rotation
template <typename AVLNodeT>
void rotate_right_right(AVLNodeT*& root) {
  tree::rotate_right_right(root);
  avl::update_height(root->left);
  avl::update_height(root);
}

// left-right double rotation
template <typename AVLNodeT>
void rotate_left_right(AVLNodeT*& root) {
  tree::rotate_left_right(root);
  avl::update_height(root->left);
  avl::update_height(root->right);
  avl::update_height(root);
}

// right-left double rotation
template <typename AVLNodeT>
void rotate_right_left(AVLNodeT*& root) {
  tree::rotate_right_left(root);
  avl::update_height(root->left);
  avl::update_height(root->right);
  avl::update_height(root);
}

// insert new_node into AVL tree root
// return node that contains new_node->value
template <typename AVLNodeT, typename BinaryPredicate>
AVLNodeT* insert(AVLNodeT*& root, AVLNodeT* new_node, BinaryPredicate pred) {
  AVLNodeT* position = tree::insert(root, new_node);  // bs-tree insertion
  if (position != new_node) {
    return position;  // already exists
  }

  // rebalance AVL tree
  for (AVLNodeT* p = new_node; p != nullptr; root = p, p = p->parent) {
    if (avl::height(p->left) >= avl::height(p->right) + 2) {
      if (pred(new_node->value, p->left->value)) {  // left-left
        avl::rotate_left_left(p);
      } else {  // left-right
        avl::rotate_left_right(p);
      }
    } else if (avl::height(p->right) >= avl::height(p->left) + 2) {
      if (pred(p->right->value, new_node->value)) {  // right-right
        avl::rotate_right_right(p);
      } else {  // right-left
        avl::rotate_right_left(p);
      }
    } else {                  // no need for rebalance
      avl::update_height(p);  // update height field
    }
  }
  return position;
}

template <typename AVLNodeT>
AVLNodeT* insert(AVLNodeT*& root, AVLNodeT* new_node) {
  return avl::insert(root, new_node,
                     std::less<typename AVLNodeT::value_type>());
}

}  // namespace avl

namespace splay {

// return new root of splaying tree, i.e. target
template <typename NodeT>
NodeT* adjust(NodeT* target) {
  while (target->parent) {  // do until target become root

    // self's parent is root, rotate self and parent
    NodeT* p = target->parent;
    if (nullptr == p->parent) {
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
    if (p == g->left && target == p->left) {  // left-left
      replace_child(g, target);
      set_left(g, p->right);
      set_left(p, target->right);
      set_right(p, g);
      set_right(target, p);
    } else if (p == g->right && target == p->right) {  // right-right
      replace_child(g, target);
      set_right(g, p->left);
      set_right(p, target->left);
      set_left(p, g);
      set_left(target, p);
    } else if (p == g->left && target == p->right) {  // left-right
      rotate_left_right(g);                           // double rotation
    } else if (p == g->right && target == p->left) {  // right-left
      rotate_right_left(g);                           // double rotation
    }
  }
  return target;
}

}  // namespace splay

template <typename RandomAccessIterator1, typename RandomAccessIterator2,
          typename OutputIterator>
OutputIterator preorder_inorder_to_postorder(RandomAccessIterator1 pre_first,
                                             RandomAccessIterator1 pre_last,
                                             RandomAccessIterator2 in_first,
                                             OutputIterator post_iter) {
  if (pre_first == pre_last) {
    return post_iter;  // empty tree
  }

  // find root position in inorder sequence
  typedef typename std::iterator_traits<RandomAccessIterator1>::difference_type
      difference_type;
  difference_type len = pre_last - pre_first;
  RandomAccessIterator2 in_root =
      std::find(in_first, in_first + len, *pre_first);
  if (in_root == in_first + len) {
    throw std::runtime_error("mismatched input sequences");
  }
  difference_type left_len = in_root - in_first;  // left subtree size

  // traverse left subtree
  post_iter = preorder_inorder_to_postorder(
      pre_first + 1, pre_first + 1 + left_len, in_first, post_iter);

  // traverse right subtree
  post_iter = preorder_inorder_to_postorder(pre_first + 1 + left_len, pre_last,
                                            in_root + 1, post_iter);

  *(post_iter++) = *in_root;  // output root node
  return post_iter;
}  // O(n * log(n))

template <typename RandomAccessIterator1, typename RandomAccessIterator2,
          typename OutputIterator>
OutputIterator inorder_postorder_to_preorder(RandomAccessIterator1 in_first,
                                             RandomAccessIterator1 in_last,
                                             RandomAccessIterator2 post_first,
                                             OutputIterator pre_iter) {
  if (in_first == in_last) {
    return pre_iter;  // empty tree
  }

  // find root position in inorder sequence
  typedef typename std::iterator_traits<RandomAccessIterator1>::difference_type
      difference_type;
  difference_type len = in_last - in_first;
  RandomAccessIterator1 in_root =
      std::find(in_first, in_last, *(post_first + len - 1));
  if (in_root == in_last) {
    throw std::runtime_error("mismatched input sequences");
  }
  difference_type left_len = in_root - in_first;  // left subtree size

  *(pre_iter++) = *in_root;  // output root node

  // traverse left subtree
  pre_iter =
      inorder_postorder_to_preorder(in_first, in_root, post_first, pre_iter);

  // traverse right subtree
  pre_iter = inorder_postorder_to_preorder(in_root + 1, in_last,
                                           post_first + left_len, pre_iter);
  return pre_iter;
}  // O(n * log(n))

template <typename NodeT, typename RandomAccessIterator1,
          typename RandomAccessIterator2>
NodeT* preorder_inorder_to_tree(RandomAccessIterator1 pre_first,
                                RandomAccessIterator1 pre_last,
                                RandomAccessIterator2 in_first) {
  if (pre_first == pre_last) {
    return nullptr;  // empty tree
  }

  // find root position in inorder sequence
  typedef typename std::iterator_traits<RandomAccessIterator1>::difference_type
      difference_type;
  difference_type len = pre_last - pre_first;
  RandomAccessIterator2 in_root =
      std::find(in_first, in_first + len, *pre_first);
  if (in_root == in_first + len) {
    throw std::runtime_error("mismatched input sequences");
  }
  difference_type left_len = in_root - in_first;  // left subtree size

  NodeT* root = new NodeT();
  root->value = *in_root;  // output root node

  // traverse left subtree
  set_left(root, preorder_inorder_to_tree<NodeT>(
                     pre_first + 1, pre_first + 1 + left_len, in_first));

  // traverse right subtree
  set_right(root, preorder_inorder_to_tree<NodeT>(pre_first + 1 + left_len,
                                                  pre_last, in_root + 1));

  return root;
}

template <typename NodeT, typename RandomAccessIterator1,
          typename RandomAccessIterator2>
NodeT* inorder_postorder_to_tree(RandomAccessIterator1 in_first,
                                 RandomAccessIterator1 in_last,
                                 RandomAccessIterator2 post_first) {
  if (in_first == in_last) {
    return nullptr;  // empty tree
  }

  // find root position in inorder sequence
  typedef typename std::iterator_traits<RandomAccessIterator1>::difference_type
      difference_type;
  difference_type len = in_last - in_first;
  RandomAccessIterator1 in_root =
      std::find(in_first, in_last, *(post_first + len - 1));
  if (in_root == in_last) {
    throw std::runtime_error("mismatched input sequences");
  }
  difference_type left_len = in_root - in_first;  // left subtree size

  NodeT* root = new NodeT();
  root->value = *in_root;  // output root node

  // traverse left subtree
  set_left(root,
           inorder_postorder_to_tree<NodeT>(in_first, in_root, post_first));

  // traverse right subtree
  set_right(root, inorder_postorder_to_tree<NodeT>(in_root + 1, in_last,
                                                   post_first + left_len));
  return root;
}

namespace heap {

// assumption: array index is zero based

size_type left(size_type pos) { return (pos << 1) + 1; }

size_type right(size_type pos) { return (pos << 1) + 2; }

size_type parent(size_type pos) { return pos ? (pos - 1) >> 1 : 0; }

template <typename RandomAccessIterator, typename BinaryPredicate>
void percolate_up(RandomAccessIterator first, RandomAccessIterator last,
                  size_type pos, BinaryPredicate pred) {
  typedef std::iterator_traits<RandomAccessIterator> traits;
  typedef typename traits::value_type value_type;
  typedef typename traits::difference_type difference_type;
  difference_type len = last - first;
  if (len <= 1 || difference_type(pos) >= len) {
    return;  // no need to percolate
  }
  value_type value = first[pos];
  size_type ppos = heap::parent(pos);
  while (pos > 0 && pred(first[ppos], value)) {
    first[pos] = first[ppos];  // nodes flow down
    pos = ppos;
    ppos = heap::parent(pos);
  }
  first[pos] = value;
}

template <typename RandomAccessIterator, typename BinaryPredicate>
void percolate_down(RandomAccessIterator first, RandomAccessIterator last,
                    size_type pos, BinaryPredicate pred) {
  typedef std::iterator_traits<RandomAccessIterator> traits;
  typedef typename traits::value_type value_type;
  typedef typename traits::difference_type difference_type;
  difference_type len = last - first;
  if (len <= 1 || difference_type(pos) >= len) {
    return;  // no need to percolate
  }
  value_type value = first[pos];
  for (difference_type child = 0; (child = heap::left(pos)) < len;
       pos = child) {
    difference_type r_child = heap::right(pos);
    if (r_child < len && pred(first[child], first[r_child])) {
      child = r_child;
    }
    if (pred(first[child], value)) {
      break;  // no more percolate
    }
    first[pos] = first[child];  // nodes flow up
  }
  first[pos] = value;
}

// equivalent to std::make_heap()
// post-condition: for each none root position 'pos',
//  pred(first[pos], first[parent(pos)]) != false
template <typename RandomAccessIterator, typename BinaryPredicate>
void make(RandomAccessIterator first, RandomAccessIterator last,
          BinaryPredicate pred) {
  typedef typename std::iterator_traits<RandomAccessIterator>::difference_type
      difference_type;
  difference_type len = last - first;
  if (len <= 1) {
    return;  // no need to adjustment
  }
  for (difference_type pos = heap::parent(len - 1); pos >= 0; --pos)
    percolate_down(first, last, pos, pred);
}

template <typename RandomAccessIterator>
void make(RandomAccessIterator first, RandomAccessIterator last) {
  typedef typename std::iterator_traits<RandomAccessIterator>::value_type
      value_type;
  make(first, last, std::less<value_type>());
}

// equivalent to std::push_heap()
template <typename RandomAccessIterator, typename BinaryPredicate>
void push(RandomAccessIterator first, RandomAccessIterator last,
          BinaryPredicate pred) {
  typedef typename std::iterator_traits<RandomAccessIterator>::difference_type
      difference_type;
  difference_type len = last - first;
  if (len <= 1) {
    return;  // no need to adjustment
  }
  percolate_up(first, last, len - 1, pred);
}

template <typename RandomAccessIterator>
void push(RandomAccessIterator first, RandomAccessIterator last) {
  typedef typename std::iterator_traits<RandomAccessIterator>::value_type
      value_type;
  push(first, last, std::less<value_type>());
}

// equivalent to std::pop_heap()
template <typename RandomAccessIterator, typename BinaryPredicate>
void pop(RandomAccessIterator first, RandomAccessIterator last,
         BinaryPredicate pred) {
  typedef typename std::iterator_traits<RandomAccessIterator>::difference_type
      difference_type;
  difference_type len = last - first;
  if (len <= 1) {
    return;  // no need to adjustment
  }
  std::iter_swap(first, last - 1);
  percolate_down(first, last - 1, 0, pred);
}

template <typename RandomAccessIterator>
void pop(RandomAccessIterator first, RandomAccessIterator last) {
  typedef typename std::iterator_traits<RandomAccessIterator>::value_type
      value_type;
  pop(first, last, std::less<value_type>());
}

}  // namespace heap
}  // namespace tree

#endif  // TREE_H_
