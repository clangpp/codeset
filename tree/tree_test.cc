#include "tree.h"

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace tree {
namespace {

using ::testing::ElementsAreArray;

// binary search tree node type
template <typename T>
struct BSNode {
  typedef T value_type;
  value_type value;
  BSNode* left = nullptr;
  BSNode* right = nullptr;
  BSNode* parent = nullptr;
  BSNode(const value_type& val = value_type(), BSNode* l = nullptr,
         BSNode* r = nullptr, BSNode* p = nullptr)
      : value(val), left(l), right(r), parent(p) {}
};

struct Collector {
  std::vector<int> values;
  void collect(int value) { values.push_back(value); }
};

template <typename T>
struct AVLNode {
  typedef T value_type;
  value_type value;
  AVLNode* left = nullptr;
  AVLNode* right = nullptr;
  AVLNode* parent = nullptr;
  tree::ssize_type height = 0;
  AVLNode(const value_type& val = value_type(), AVLNode* l = nullptr,
          AVLNode* r = nullptr, AVLNode* p = nullptr, tree::ssize_type h = 0)
      : value(val), left(l), right(r), parent(p), height(h) {}
};

template <typename NodeT, typename Print>
std::string PrintTreeWithPrefixNewLine(const NodeT* root, const Print& print) {
  std::stringstream ss;
  print(root, ss);
  return '\n' + ss.str();
}

template <typename NodeT>
std::string PrintTreeWithPrefixNewLine(const NodeT* root) {
  return PrintTreeWithPrefixNewLine(root, &tree::print<NodeT>);
}

TEST(UtilityTest, ItWorks) {
  // make tree
  std::vector<BSNode<int>> nodes(6);
  for (size_t i = 0; i < nodes.size(); ++i) {
    nodes[i] = BSNode<int>(i);
  }
  tree::set_left(&nodes[0], &nodes[1]);
  tree::set_right(&nodes[0], &nodes[2]);
  tree::set_left(&nodes[1], &nodes[3]);
  tree::set_right(&nodes[3], &nodes[4]);
  tree::set_right(&nodes[2], &nodes[5]);
  tree::set_left(&nodes[5], (BSNode<int>*)nullptr);
  tree::set_right(&nodes[5], (BSNode<int>*)nullptr);

  EXPECT_EQ(nodes[0].left, &nodes[1]);
  EXPECT_EQ(nodes[1].parent, &nodes[0]);
  EXPECT_EQ(nodes[5].left, nullptr);

  EXPECT_EQ(nodes[0].right, &nodes[2]);
  EXPECT_EQ(nodes[2].parent, &nodes[0]);
  EXPECT_EQ(nodes[5].right, nullptr);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(&nodes[0]), R"(
    ,---3
    |   `===4
,---1
0
`===2
    `===5
)");
  EXPECT_THAT(
      PrintTreeWithPrefixNewLine(&nodes[0], &tree::print_preorder<BSNode<int>>),
      R"(
0
|---1
|   `---3
|       `===4
`===2
    `===5
)");
  EXPECT_THAT(
      PrintTreeWithPrefixNewLine(&nodes[0], &tree::print_inorder<BSNode<int>>),
      R"(
    ,---3
    |   `===4
,---1
0
`===2
    `===5
)");
  EXPECT_THAT(PrintTreeWithPrefixNewLine(&nodes[0],
                                         &tree::print_postorder<BSNode<int>>),
              R"(
        ,===4
    ,---3
,---1
|   ,===5
|===2
0
)");
}

TEST(RotationTest, LeftLeft) {
  std::vector<BSNode<int>> nodes(4);
  for (size_t i = 0; i < nodes.size(); ++i) {
    nodes[i] = BSNode<int>(i);
  }
  auto* p = &nodes[3];
  tree::set_left(&nodes[3], &nodes[1]);
  tree::set_left(&nodes[1], &nodes[0]);
  tree::set_right(&nodes[1], &nodes[2]);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(p), R"(
    ,---0
,---1
|   `===2
3
)");

  tree::rotate_left_left(p);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(p), R"(
,---0
1
|   ,---2
`===3
)");

  EXPECT_EQ(nullptr, p->parent);
  EXPECT_EQ(p, &nodes[1]);
  EXPECT_EQ(p->right, &nodes[3]);
  EXPECT_EQ(nodes[3].left, &nodes[2]);
}

TEST(RotationTest, RightRight) {
  std::vector<BSNode<int>> nodes(4);
  for (size_t i = 0; i < nodes.size(); ++i) {
    nodes[i] = BSNode<int>(i);
  }
  auto* p = &nodes[0];
  tree::set_right(&nodes[0], &nodes[2]);
  tree::set_right(&nodes[2], &nodes[3]);
  tree::set_left(&nodes[2], &nodes[1]);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(p), R"(
0
|   ,---1
`===2
    `===3
)");

  tree::rotate_right_right(p);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(p), R"(
,---0
|   `===1
2
`===3
)");

  EXPECT_EQ(nullptr, p->parent);
  EXPECT_EQ(p, &nodes[2]);
  EXPECT_EQ(p->left, &nodes[0]);
  EXPECT_EQ(nodes[0].right, &nodes[1]);
}

TEST(RotationTest, LeftRight) {
  std::vector<BSNode<int>> nodes(6);
  for (size_t i = 0; i < nodes.size(); ++i) {
    nodes[i] = BSNode<int>(i);
  }
  auto* p = &nodes[4];
  tree::set_left(&nodes[4], &nodes[1]);
  tree::set_right(&nodes[4], &nodes[5]);
  tree::set_left(&nodes[1], &nodes[0]);
  tree::set_right(&nodes[1], &nodes[3]);
  tree::set_left(&nodes[3], &nodes[2]);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(p), R"(
    ,---0
,---1
|   |   ,---2
|   `===3
4
`===5
)");

  tree::rotate_left_right(p);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(p), R"(
    ,---0
,---1
|   `===2
3
`===4
    `===5
)");

  EXPECT_EQ(nullptr, p->parent);
  EXPECT_EQ(p, &nodes[3]);

  EXPECT_EQ(nodes[1].right, &nodes[2]);
  EXPECT_EQ(nullptr, nodes[4].left);
  EXPECT_EQ(nodes[3].left, &nodes[1]);
  EXPECT_EQ(nodes[3].right, &nodes[4]);
}

TEST(RotationTest, RightLeft) {
  std::vector<BSNode<int>> nodes(6);
  for (size_t i = 0; i < nodes.size(); ++i) {
    nodes[i] = BSNode<int>(i);
  }
  auto* p = &nodes[1];
  tree::set_left(&nodes[1], &nodes[0]);
  tree::set_right(&nodes[1], &nodes[4]);
  tree::set_left(&nodes[4], &nodes[3]);
  tree::set_right(&nodes[4], &nodes[5]);
  tree::set_left(&nodes[3], &nodes[2]);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(p), R"(
,---0
1
|       ,---2
|   ,---3
`===4
    `===5
)");

  tree::rotate_right_left(p);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(p), R"(
    ,---0
,---1
|   `===2
3
`===4
    `===5
)");

  EXPECT_EQ(nullptr, p->parent);
  EXPECT_EQ(p, &nodes[3]);
  EXPECT_EQ(nodes[1].right, &nodes[2]);
  EXPECT_EQ(nullptr, nodes[4].left);
  EXPECT_EQ(nodes[3].left, &nodes[1]);
  EXPECT_EQ(nodes[3].right, &nodes[4]);
}

TEST(TraversalTest, ItWorks) {
  std::vector<BSNode<int>> nodes(7);
  for (size_t i = 0; i < nodes.size(); ++i) {
    nodes[i] = BSNode<int>(i);
  }
  auto* p = &nodes[0];
  tree::set_left(&nodes[0], &nodes[1]);
  tree::set_right(&nodes[0], &nodes[2]);
  tree::set_left(&nodes[1], &nodes[3]);
  tree::set_right(&nodes[1], &nodes[4]);
  tree::set_left(&nodes[2], &nodes[5]);
  tree::set_left(&nodes[4], &nodes[6]);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(p), R"(
    ,---3
,---1
|   |   ,---6
|   `===4
0
|   ,---5
`===2
)");

  int inorder[] = {3, 1, 6, 4, 0, 5, 2};
  int postorder[] = {3, 6, 4, 1, 5, 2, 0};
  int preorder[] = {0, 1, 3, 4, 6, 2, 5};
  int level_order[] = {0, 1, 2, 3, 4, 5, 6};

  Collector c;
  c.values.clear();
  tree::traverse_inorder(p, [&c](int v) { c.collect(v); });
  EXPECT_THAT(c.values, ElementsAreArray(inorder));

  c.values.clear();
  tree::traverse_postorder(p, [&c](int v) { c.collect(v); });
  EXPECT_THAT(c.values, ElementsAreArray(postorder));

  c.values.clear();
  tree::traverse_preorder(p, [&c](int v) { c.collect(v); });
  EXPECT_THAT(c.values, ElementsAreArray(preorder));

  c.values.clear();
  tree::traverse_level_order(p, [&c](int v) { c.collect(v); });
  EXPECT_THAT(c.values, ElementsAreArray(level_order));

  c.values.clear();
  tree::traverse_inorder_recursive(p, [&c](int v) { c.collect(v); });
  EXPECT_THAT(c.values, ElementsAreArray(inorder));

  c.values.clear();
  tree::traverse_postorder_recursive(p, [&c](int v) { c.collect(v); });
  EXPECT_THAT(c.values, ElementsAreArray(postorder));

  c.values.clear();
  tree::traverse_preorder_recursive(p, [&c](int v) { c.collect(v); });
  EXPECT_THAT(c.values, ElementsAreArray(preorder));

  c.values.clear();
  tree::traverse_inorder_stack(p, [&c](int v) { c.collect(v); });
  EXPECT_THAT(c.values, ElementsAreArray(inorder));

  c.values.clear();
  tree::traverse_postorder_stack(p, [&c](int v) { c.collect(v); });
  EXPECT_THAT(c.values, ElementsAreArray(postorder));

  c.values.clear();
  tree::traverse_preorder_stack(p, [&c](int v) { c.collect(v); });
  EXPECT_THAT(c.values, ElementsAreArray(preorder));

  c.values.clear();
  tree::traverse_inorder_tricky(p, [&c](int v) { c.collect(v); });
  EXPECT_THAT(c.values, ElementsAreArray(inorder));

  c.values.clear();
  tree::traverse_postorder_tricky(p, [&c](int v) { c.collect(v); });
  EXPECT_THAT(c.values, ElementsAreArray(postorder));

  c.values.clear();
  tree::traverse_preorder_tricky(p, [&c](int v) { c.collect(v); });
  EXPECT_THAT(c.values, ElementsAreArray(preorder));
}

TEST(ObserverTest, ItWorks) {
  BSNode<int>* p = nullptr;
  BSNode<int>* root = nullptr;
  BSNode<int>* curr = nullptr;

  std::vector<BSNode<int>> nodes(7);
  for (size_t i = 0; i < nodes.size(); ++i) {
    nodes[i] = BSNode<int>(i);
  }
  EXPECT_THAT(PrintTreeWithPrefixNewLine(root), R"(
)");

  curr = &nodes[3];
  p = tree::insert(root, curr);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(root), R"(
3
)");

  EXPECT_EQ(root, &nodes[3]);
  EXPECT_EQ(p, curr);

  curr = &nodes[1];
  p = tree::insert(root, &nodes[1]);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(root), R"(
,---1
3
)");

  EXPECT_EQ(root, &nodes[3]);
  EXPECT_EQ(p, curr);
  EXPECT_EQ(nodes[3].left, curr);
  EXPECT_EQ(curr->parent, &nodes[3]);

  curr = &nodes[4];
  p = tree::insert(root, &nodes[4]);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(root), R"(
,---1
3
`===4
)");

  EXPECT_EQ(root, &nodes[3]);
  EXPECT_EQ(p, curr);
  EXPECT_EQ(nodes[3].right, curr);
  EXPECT_EQ(curr->parent, &nodes[3]);

  curr = &nodes[0];
  p = tree::insert(root, curr);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(root), R"(
    ,---0
,---1
3
`===4
)");

  EXPECT_EQ(root, &nodes[3]);
  EXPECT_EQ(p, curr);
  EXPECT_EQ(nodes[1].left, curr);
  EXPECT_EQ(curr->parent, &nodes[1]);

  curr = &nodes[2];
  p = tree::insert(root, curr);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(root), R"(
    ,---0
,---1
|   `===2
3
`===4
)");

  EXPECT_EQ(root, &nodes[3]);
  EXPECT_EQ(p, curr);
  EXPECT_EQ(nodes[1].right, curr);
  EXPECT_EQ(curr->parent, &nodes[1]);

  curr = &nodes[6];
  p = tree::insert(root, curr);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(root), R"(
    ,---0
,---1
|   `===2
3
`===4
    `===6
)");

  EXPECT_EQ(root, &nodes[3]);
  EXPECT_EQ(p, curr);
  EXPECT_EQ(nodes[4].right, curr);
  EXPECT_EQ(curr->parent, &nodes[4]);

  curr = &nodes[5];
  p = tree::insert(root, curr);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(root), R"(
    ,---0
,---1
|   `===2
3
`===4
    |   ,---5
    `===6
)");

  EXPECT_EQ(root, &nodes[3]);
  EXPECT_EQ(p, curr);
  EXPECT_EQ(nodes[6].left, curr);
  EXPECT_EQ(curr->parent, &nodes[6]);

  EXPECT_EQ(tree::height((BSNode<int>*)nullptr), -1);
  EXPECT_EQ(tree::height(&nodes[3]), 3);
  EXPECT_EQ(tree::height(&nodes[4]), 2);
  EXPECT_EQ(tree::height(&nodes[1]), 1);
  EXPECT_EQ(tree::height(&nodes[5]), 0);

  EXPECT_EQ(tree::count((BSNode<int>*)nullptr), 0);
  EXPECT_EQ(tree::count(&nodes[3]), 7);
  EXPECT_EQ(tree::count(&nodes[4]), 3);
  EXPECT_EQ(tree::count(&nodes[1]), 3);
  EXPECT_EQ(tree::count(&nodes[5]), 1);

  EXPECT_EQ(tree::find(root, 6), &nodes[6]);
  EXPECT_EQ(tree::find(root, 7), nullptr);
  EXPECT_EQ(tree::find(root, -1), nullptr);
}

TEST(AVLTreeTest, ItWorks) {
  AVLNode<int>* root = nullptr;
  AVLNode<int>* p = nullptr;
  AVLNode<int>* curr = nullptr;

  std::vector<AVLNode<int>> nodes(100);
  for (size_t i = 0; i < nodes.size(); ++i) {
    nodes[i] = AVLNode<int>(i);
  }
  EXPECT_THAT(PrintTreeWithPrefixNewLine(root), R"(
)");

  curr = &nodes[50];
  p = tree::avl::insert(root, curr);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(root), R"(
50
)");

  EXPECT_EQ(p, curr);
  EXPECT_EQ(root, &nodes[50]);

  curr = &nodes[45];
  p = tree::avl::insert(root, curr);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(root), R"(
,---45
50
)");

  EXPECT_EQ(p, curr);
  EXPECT_EQ(root, &nodes[50]);
  EXPECT_EQ(root->left, &nodes[45]);

  curr = &nodes[40];
  p = tree::avl::insert(root, curr);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(root), R"(
,---40
45
`===50
)");

  EXPECT_EQ(p, curr);
  EXPECT_EQ(root, &nodes[45]);
  EXPECT_EQ(root, &nodes[45]);
  EXPECT_EQ(root->right, &nodes[50]);
  EXPECT_EQ(root->left, &nodes[40]);

  curr = &nodes[55];
  p = tree::avl::insert(root, curr);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(root), R"(
,---40
45
`===50
    `===55
)");

  EXPECT_EQ(p, curr);
  EXPECT_EQ(root, &nodes[45]);

  curr = &nodes[60];
  p = tree::avl::insert(root, curr);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(root), R"(
,---40
45
|   ,---50
`===55
    `===60
)");

  EXPECT_EQ(p, curr);
  EXPECT_EQ(root->right, &nodes[55]);
  EXPECT_EQ(nodes[55].left, &nodes[50]);
  EXPECT_EQ(nodes[55].right, &nodes[60]);

  curr = &nodes[48];
  p = tree::avl::insert(root, curr);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(root), R"(
    ,---40
,---45
|   `===48
50
`===55
    `===60
)");

  EXPECT_EQ(p, curr);
  EXPECT_EQ(root, &nodes[50]);
  EXPECT_EQ(root->left, &nodes[45]);
  EXPECT_EQ(root->right, &nodes[55]);
  EXPECT_EQ(root->left->right, &nodes[48]);
  EXPECT_EQ(root->right->left, nullptr);

  curr = &nodes[30];
  p = tree::avl::insert(root, curr);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(root), R"(
        ,---30
    ,---40
,---45
|   `===48
50
`===55
    `===60
)");

  EXPECT_EQ(p, curr);
  EXPECT_EQ(root, &nodes[50]);

  curr = &nodes[35];
  p = tree::avl::insert(root, curr);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(root), R"(
        ,---30
    ,---35
    |   `===40
,---45
|   `===48
50
`===55
    `===60
)");

  EXPECT_EQ(p, curr);
  EXPECT_EQ(nodes[45].left, &nodes[35]);
  EXPECT_EQ(nodes[35].left, &nodes[30]);
  EXPECT_EQ(nodes[35].right, &nodes[40]);
  EXPECT_EQ(nodes[30].right, nullptr);
  EXPECT_EQ(nodes[40].left, nullptr);

  AVLNode<int> node60(60);
  curr = &node60;
  p = tree::avl::insert(root, curr);
  EXPECT_THAT(PrintTreeWithPrefixNewLine(root), R"(
        ,---30
    ,---35
    |   `===40
,---45
|   `===48
50
`===55
    `===60
)");

  EXPECT_EQ(p, &nodes[60]);
  EXPECT_NE(p, &node60);
}

TEST(SplayTreeTest, ItWorks) {
  BSNode<int>* root = nullptr;
  BSNode<int>* curr = nullptr;

  std::vector<BSNode<int>> nodes(100);
  for (size_t i = 0; i < nodes.size(); ++i) {
    nodes[i] = BSNode<int>(i);
  }
  for (int i = 7; i >= 1; --i) {
    tree::insert(root, &nodes[i]);
  }

  std::string trees[] = {
      R"(
                    ,---1
                ,---2
            ,---3
        ,---4
    ,---5
,---6
7
)",
      R"(
1
|       ,---2
|       |   `===3
|   ,---4
|   |   `===5
`===6
    `===7
)",
      R"(
,---1
2
|   ,---3
`===4
    |   ,---5
    `===6
        `===7
)",
      R"(
    ,---1
,---2
3
`===4
    |   ,---5
    `===6
        `===7
)",
      R"(
        ,---1
    ,---2
,---3
4
|   ,---5
`===6
    `===7
)",
      R"(
            ,---1
        ,---2
    ,---3
,---4
5
`===6
    `===7
)",
      R"(
                ,---1
            ,---2
        ,---3
    ,---4
,---5
6
`===7
)",
      R"(
                    ,---1
                ,---2
            ,---3
        ,---4
    ,---5
,---6
7
)",
  };
  EXPECT_THAT(PrintTreeWithPrefixNewLine(root), trees[0]);

  for (int i = 1; i <= 7; ++i) {
    curr = &nodes[i];
    root = tree::splay::adjust(curr);
    EXPECT_THAT(PrintTreeWithPrefixNewLine(root), trees[i]);

    EXPECT_EQ(root, curr);
    EXPECT_EQ(nullptr, root->parent);
  }
}

TEST(TraversalOrdersTest, ItWorks) {
  BSNode<int>* root = nullptr;

  std::vector<BSNode<int>> nodes(100);
  for (size_t i = 0; i < nodes.size(); ++i) {
    nodes[i] = BSNode<int>(i);
  }
  for (auto i :
       {50, 30, 20, 15, 10, 25, 40, 35, 45, 47, 70, 55, 60, 65, 85, 80, 75}) {
    tree::insert(root, &nodes[i]);
  }
  EXPECT_THAT(PrintTreeWithPrefixNewLine(root), R"(
            ,---10
        ,---15
    ,---20
    |   `===25
,---30
|   |   ,---35
|   `===40
|       `===45
|           `===47
50
|   ,---55
|   |   `===60
|   |       `===65
`===70
    |       ,---75
    |   ,---80
    `===85
)");

  Collector c;
  c.values.clear();
  tree::traverse_preorder(root, [&c](int v) { c.collect(v); });
  std::vector<int> preorders = c.values;
  EXPECT_THAT(preorders, ElementsAreArray({50, 30, 20, 15, 10, 25, 40, 35, 45,
                                           47, 70, 55, 60, 65, 85, 80, 75}));

  c.values.clear();
  tree::traverse_inorder(root, [&c](int v) { c.collect(v); });
  std::vector<int> inorders = c.values;
  EXPECT_THAT(inorders, ElementsAreArray({10, 15, 20, 25, 30, 35, 40, 45, 47,
                                          50, 55, 60, 65, 70, 75, 80, 85}));

  c.values.clear();
  tree::traverse_postorder(root, [&c](int v) { c.collect(v); });
  std::vector<int> postorders = c.values;
  EXPECT_THAT(postorders, ElementsAreArray({10, 15, 25, 20, 35, 47, 45, 40, 30,
                                            65, 60, 55, 75, 80, 85, 70, 50}));

  std::vector<int> result(inorders.size());
  auto iter = tree::preorder_inorder_to_postorder(
      preorders.begin(), preorders.end(), inorders.begin(), result.begin());
  EXPECT_EQ(iter, result.end());
  EXPECT_THAT(result, ElementsAreArray(postorders));

  iter = tree::inorder_postorder_to_preorder(
      inorders.begin(), inorders.end(), postorders.begin(), result.begin());
  EXPECT_EQ(iter, result.end());
  EXPECT_THAT(result, ElementsAreArray(preorders));

  auto* tree1 = tree::preorder_inorder_to_tree<BSNode<int>>(
      preorders.begin(), preorders.end(), inorders.begin());
  EXPECT_THAT(PrintTreeWithPrefixNewLine(tree1), R"(
            ,---10
        ,---15
    ,---20
    |   `===25
,---30
|   |   ,---35
|   `===40
|       `===45
|           `===47
50
|   ,---55
|   |   `===60
|   |       `===65
`===70
    |       ,---75
    |   ,---80
    `===85
)");

  tree::destory(tree1);
  auto* tree2 = tree::inorder_postorder_to_tree<BSNode<int>>(
      inorders.begin(), inorders.end(), postorders.begin());
  EXPECT_THAT(PrintTreeWithPrefixNewLine(tree2), R"(
            ,---10
        ,---15
    ,---20
    |   `===25
,---30
|   |   ,---35
|   `===40
|       `===45
|           `===47
50
|   ,---55
|   |   `===60
|   |       `===65
`===70
    |       ,---75
    |   ,---80
    `===85
)");

  tree::destory(tree2);
}

TEST(HeapTest, Position) {
  EXPECT_EQ(tree::heap::left(0), 1);
  EXPECT_EQ(tree::heap::left(1), 3);
  EXPECT_EQ(tree::heap::left(2), 5);
  EXPECT_EQ(tree::heap::left(5), 11);

  EXPECT_EQ(tree::heap::right(0), 2);
  EXPECT_EQ(tree::heap::right(1), 4);
  EXPECT_EQ(tree::heap::right(2), 6);
  EXPECT_EQ(tree::heap::right(5), 12);

  EXPECT_EQ(tree::heap::parent(0), 0);
  EXPECT_EQ(tree::heap::parent(1), 0);
  EXPECT_EQ(tree::heap::parent(2), 0);
  EXPECT_EQ(tree::heap::parent(5), 2);
}

TEST(HeapTest, PercolateUp) {
  std::vector<int> values = {0, 1, 2, 3, 4, 5, 6, 7, 8, -9, 10};
  std::vector<BSNode<int>> nodes(values.size());
  for (size_t i = 0; i < values.size(); ++i) {
    nodes[i] = BSNode<int>(values[i]);
    if (tree::heap::left(i) < values.size()) {
      tree::set_left(&nodes[i], &nodes[tree::heap::left(i)]);
    }
    if (tree::heap::right(i) < values.size()) {
      tree::set_right(&nodes[i], &nodes[tree::heap::right(i)]);
    }
  }
  EXPECT_THAT(PrintTreeWithPrefixNewLine(&nodes[0]), R"(
        ,---7
    ,---3
    |   `===8
,---1
|   |   ,----9
|   `===4
|       `===10
0
|   ,---5
`===2
    `===6
)");

  int i = 9;
  tree::heap::percolate_up(values.begin(), values.end(), i,
                           std::greater<int>());
  for (size_t i = 0; i < values.size(); ++i) {
    nodes[i].value = values[i];
  }
  EXPECT_THAT(PrintTreeWithPrefixNewLine(&nodes[0]), R"(
        ,---7
    ,---3
    |   `===8
,---0
|   |   ,---4
|   `===1
|       `===10
-9
|   ,---5
`===2
    `===6
)");

  EXPECT_THAT(values, ElementsAreArray({-9, 0, 2, 3, 1, 5, 6, 7, 8, 4, 10}));
}

TEST(HeapTest, PercolateDown) {
  std::vector<int> values = {0, 19, 2, 4, 3, 5, 6, 7, 8, 9, 10, 11, 12};
  std::vector<BSNode<int>> nodes(values.size());
  for (size_t i = 0; i < values.size(); ++i) {
    nodes[i] = BSNode<int>(values[i]);
    if (tree::heap::left(i) < values.size()) {
      tree::set_left(&nodes[i], &nodes[tree::heap::left(i)]);
    }
    if (tree::heap::right(i) < values.size()) {
      tree::set_right(&nodes[i], &nodes[tree::heap::right(i)]);
    }
  }
  EXPECT_THAT(PrintTreeWithPrefixNewLine(&nodes[0]), R"(
        ,---7
    ,---4
    |   `===8
,---19
|   |   ,---9
|   `===3
|       `===10
0
|       ,---11
|   ,---5
|   |   `===12
`===2
    `===6
)");

  tree::heap::percolate_down(values.begin(), values.end(), 1,
                             std::greater<int>());
  for (size_t i = 0; i < values.size(); ++i) {
    nodes[i].value = values[i];
  }
  EXPECT_THAT(PrintTreeWithPrefixNewLine(&nodes[0]), R"(
        ,---7
    ,---4
    |   `===8
,---3
|   |   ,---19
|   `===9
|       `===10
0
|       ,---11
|   ,---5
|   |   `===12
`===2
    `===6
)");

  EXPECT_THAT(values,
              ElementsAreArray({0, 3, 2, 4, 9, 5, 6, 7, 8, 19, 10, 11, 12}));
}

TEST(HeapTest, Make) {
  std::vector<int> values = {150, 80, 40, 30, 10,  70,  110, 100,
                             20,  90, 60, 50, 120, 140, 130};
  std::vector<BSNode<int>> nodes(values.size());
  for (size_t i = 0; i < values.size(); ++i) {
    nodes[i] = BSNode<int>(values[i]);
    if (tree::heap::left(i) < values.size()) {
      tree::set_left(&nodes[i], &nodes[tree::heap::left(i)]);
    }
    if (tree::heap::right(i) < values.size()) {
      tree::set_right(&nodes[i], &nodes[tree::heap::right(i)]);
    }
  }
  EXPECT_THAT(PrintTreeWithPrefixNewLine(&nodes[0]), R"(
        ,---100
    ,---30
    |   `===20
,---80
|   |   ,---90
|   `===10
|       `===60
150
|       ,---50
|   ,---70
|   |   `===120
`===40
    |   ,---140
    `===110
        `===130
)");

  tree::heap::make(values.begin(), values.end(), std::greater<int>());

  for (size_t i = 0; i < values.size(); ++i) {
    nodes[i].value = values[i];
  }
  EXPECT_THAT(PrintTreeWithPrefixNewLine(&nodes[0]), R"(
        ,---100
    ,---30
    |   `===150
,---20
|   |   ,---90
|   `===60
|       `===80
10
|       ,---70
|   ,---50
|   |   `===120
`===40
    |   ,---140
    `===110
        `===130
)");

  EXPECT_THAT(values, ElementsAreArray({10, 20, 40, 30, 60, 50, 110, 100, 150,
                                        90, 80, 70, 120, 140, 130}));
}

TEST(HeapTest, Push) {
  std::vector<int> values = {13, 21, 16, 24, 31, 19, 68, 65, 26, 32, 14};
  std::vector<BSNode<int>> nodes(values.size());
  for (size_t i = 0; i < values.size(); ++i) {
    nodes[i] = BSNode<int>(values[i]);
    if (tree::heap::left(i) < values.size()) {
      tree::set_left(&nodes[i], &nodes[tree::heap::left(i)]);
    }
    if (tree::heap::right(i) < values.size()) {
      tree::set_right(&nodes[i], &nodes[tree::heap::right(i)]);
    }
  }
  EXPECT_THAT(PrintTreeWithPrefixNewLine(&nodes[0]), R"(
        ,---65
    ,---24
    |   `===26
,---21
|   |   ,---32
|   `===31
|       `===14
13
|   ,---19
`===16
    `===68
)");

  tree::heap::push(values.begin(), values.end(), std::greater<int>());

  for (size_t i = 0; i < values.size(); ++i) {
    nodes[i].value = values[i];
  }
  EXPECT_THAT(PrintTreeWithPrefixNewLine(&nodes[0]), R"(
        ,---65
    ,---24
    |   `===26
,---14
|   |   ,---32
|   `===21
|       `===31
13
|   ,---19
`===16
    `===68
)");

  EXPECT_THAT(values,
              ElementsAreArray({13, 14, 16, 24, 21, 19, 68, 65, 26, 32, 31}));
}

TEST(HeapTest, Pop) {
  std::vector<int> values = {13, 14, 16, 19, 21, 19, 68, 65, 26, 32, 31};
  std::vector<BSNode<int>> nodes(values.size());
  for (size_t i = 0; i < values.size(); ++i) {
    nodes[i] = BSNode<int>(values[i]);
    if (tree::heap::left(i) < values.size()) {
      tree::set_left(&nodes[i], &nodes[tree::heap::left(i)]);
    }
    if (tree::heap::right(i) < values.size()) {
      tree::set_right(&nodes[i], &nodes[tree::heap::right(i)]);
    }
  }
  EXPECT_THAT(PrintTreeWithPrefixNewLine(&nodes[0]), R"(
        ,---65
    ,---19
    |   `===26
,---14
|   |   ,---32
|   `===21
|       `===31
13
|   ,---19
`===16
    `===68
)");

  tree::heap::pop(values.begin(), values.end(), std::greater<int>());

  for (size_t i = 0; i < values.size(); ++i) {
    nodes[i].value = values[i];
  }
  EXPECT_THAT(PrintTreeWithPrefixNewLine(&nodes[0]), R"(
        ,---65
    ,---26
    |   `===31
,---19
|   |   ,---32
|   `===21
|       `===13
14
|   ,---19
`===16
    `===68
)");

  EXPECT_THAT(values,
              ElementsAreArray({14, 19, 16, 26, 21, 19, 68, 65, 31, 32, 13}));
}

}  // namespace
}  // namespace tree
