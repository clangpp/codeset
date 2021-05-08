#include "graph.h"

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <limits>
#include <sstream>

#include "crosslist/crosslist.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::ElementsAreArray;

TEST(TopologicalSortTest, ItWorks) {
  CrossList<bool> g(7, 7);
  g.rinsert(0, 1, true);
  g.rinsert(0, 2, true);
  g.rinsert(0, 3, true);
  g.rinsert(1, 3, true);
  g.rinsert(1, 4, true);
  g.rinsert(2, 5, true);
  g.rinsert(3, 2, true);
  g.rinsert(3, 5, true);
  g.rinsert(3, 6, true);
  g.rinsert(4, 3, true);
  g.rinsert(4, 6, true);
  g.rinsert(6, 5, true);

  std::vector<graph::vertex_type> seq(7);
  auto iter = digraph::topological_sort(g, seq.begin());
  EXPECT_EQ(iter, seq.end());
  EXPECT_THAT(std::vector<graph::vertex_type>(seq.begin(), iter),
              ElementsAreArray({0, 1, 4, 3, 2, 6, 5}));

  g.rinsert(3, 4, true);
  g.rinsert(4, 5, true);
  g.rinsert(5, 3, true);
  try {
    digraph::topological_sort(g, seq.begin());
    FAIL();
  } catch (const std::exception&) {
    SUCCEED();
  }
}

TEST(DijkstraTest, Exercise) {
  const int N = 7;
  CrossList<int> g(N, N);
  std::vector<graph::vertex_type> prevs(N, 0);
  std::vector<int> dists(N, 0);
  g.rinsert(0, 1, 2);
  g.rinsert(0, 3, 1);
  g.rinsert(1, 3, 3);
  g.rinsert(1, 4, 10);
  g.rinsert(2, 0, 4);
  g.rinsert(2, 5, 5);
  g.rinsert(3, 2, 2);
  g.rinsert(3, 4, 2);
  g.rinsert(3, 5, 8);
  g.rinsert(3, 6, 4);
  g.rinsert(4, 6, 6);
  g.rinsert(6, 5, 1);
  digraph::dijkstra_shortest(g, 0, prevs.begin(), dists.begin());
  EXPECT_THAT(prevs, ElementsAreArray({0, 0, 3, 0, 3, 6, 3}));
  EXPECT_THAT(dists, ElementsAreArray({0, 2, 3, 1, 3, 6, 5}));
}

TEST(DijkstraTest, MyOwn) {
  const int N = 5;
  CrossList<int> g(N, N);
  std::vector<graph::vertex_type> prevs(N, 0);
  std::vector<int> dists(N, 0);
  g.rinsert(0, 1, 10);
  g.rinsert(0, 2, 5);
  g.rinsert(1, 3, 9);
  g.rinsert(2, 3, 13);
  g.rinsert(2, 4, 1);
  g.rinsert(4, 1, 2);
  digraph::dijkstra_shortest(g, 0, prevs.begin(), dists.begin());
  EXPECT_THAT(prevs, ElementsAreArray({0, 4, 0, 1, 2}));
  EXPECT_THAT(dists, ElementsAreArray({0, 8, 5, 17, 6}));
}

TEST(DijkstraTest, Longest) {
  const int N = 14;
  CrossList<int> g(N, N);
  std::vector<graph::vertex_type> prevs(N, 0);
  std::vector<int> dists(N, 0);
  g.rinsert(0, 1, 3);
  g.rinsert(0, 2, 2);
  g.rinsert(1, 3, 0);
  g.rinsert(1, 4, 3);
  g.rinsert(2, 3, 0);
  g.rinsert(2, 6, 1);
  g.rinsert(3, 5, 2);
  g.rinsert(4, 7, 0);
  g.rinsert(5, 7, 0);
  g.rinsert(5, 8, 0);
  g.rinsert(6, 8, 0);
  g.rinsert(6, 11, 4);
  g.rinsert(7, 9, 3);
  g.rinsert(8, 10, 2);
  g.rinsert(9, 12, 0);
  g.rinsert(10, 12, 0);
  g.rinsert(11, 12, 0);
  g.rinsert(12, 13, 1);
  digraph::dijkstra_longest(g, 0, prevs.begin(), dists.begin());
  EXPECT_THAT(prevs,
              ElementsAreArray({0, 0, 0, 1, 1, 3, 2, 4, 5, 7, 8, 6, 9, 12}));
  EXPECT_THAT(dists,
              ElementsAreArray({0, 3, 2, 3, 6, 5, 3, 6, 5, 9, 7, 7, 9, 10}));
}

TEST(DijkstraTest, Acyclic) {
  const int N = 11;
  CrossList<int> g(N, N);
  std::vector<graph::vertex_type> prevs(N, 0);
  std::vector<int> dists(N, 0);
  g.rinsert(0, 3, 3);
  g.rinsert(1, 3, 5);
  g.rinsert(2, 4, 2);
  g.rinsert(3, 5, 2);
  g.rinsert(4, 5, 3);
  g.rinsert(5, 6, 4);
  g.rinsert(5, 7, 2);
  g.rinsert(6, 8, 2);
  g.rinsert(6, 9, 3);
  g.rinsert(7, 8, 5);
  g.rinsert(8, 10, 5);
  g.rinsert(9, 10, 6);
  digraph::acyclic_dijkstra_shortest(g, 0, prevs.begin(), dists.begin());
  const graph::vertex_type NV = graph::null_vertex;
  const int INF = std::numeric_limits<int>::max();
  EXPECT_THAT(prevs, ElementsAreArray(std::initializer_list<graph::vertex_type>{
                         0, NV, NV, 0, NV, 3, 5, 5, 6, 6, 8}));
  EXPECT_THAT(dists,
              ElementsAreArray({0, INF, INF, 3, INF, 5, 9, 7, 11, 12, 16}));
}

TEST(DijkstraTest, LongestAcyclic) {
  const int N = 14;
  CrossList<int> g(N, N);
  std::vector<graph::vertex_type> prevs(N, 0);
  std::vector<int> dists(N, 0);
  g.rinsert(0, 1, 3);
  g.rinsert(0, 2, 2);
  g.rinsert(1, 3, 0);
  g.rinsert(1, 4, 3);
  g.rinsert(2, 3, 0);
  g.rinsert(2, 6, 1);
  g.rinsert(3, 5, 2);
  g.rinsert(4, 7, 0);
  g.rinsert(5, 7, 0);
  g.rinsert(5, 8, 0);
  g.rinsert(6, 8, 0);
  g.rinsert(6, 11, 4);
  g.rinsert(7, 9, 3);
  g.rinsert(8, 10, 2);
  g.rinsert(9, 12, 0);
  g.rinsert(10, 12, 0);
  g.rinsert(11, 12, 0);
  g.rinsert(12, 13, 1);
  digraph::acyclic_dijkstra_longest(g, 0, prevs.begin(), dists.begin());
  EXPECT_THAT(prevs,
              ElementsAreArray({0, 0, 0, 1, 1, 3, 2, 4, 5, 7, 8, 6, 9, 12}));
  EXPECT_THAT(dists,
              ElementsAreArray({0, 3, 2, 3, 6, 5, 3, 6, 5, 9, 7, 7, 9, 10}));
}

template <typename T>
class FillMinMaxTest : public testing::Test {
 protected:
  FillMinMaxTest() : values_(3) {}
  std::vector<T> values_;
};

using Implementations = testing::Types<int, unsigned int, double>;
TYPED_TEST_SUITE(FillMinMaxTest, Implementations);

TYPED_TEST(FillMinMaxTest, Max) {
  graph::fill_max(this->values_.begin(), this->values_.end());
  EXPECT_THAT(this->values_, ElementsAreArray(std::vector<TypeParam>(
                                 this->values_.size(),
                                 std::numeric_limits<TypeParam>::max())));
}

TYPED_TEST(FillMinMaxTest, Min) {
  graph::fill_min(this->values_.begin(), this->values_.end());
  EXPECT_THAT(this->values_, ElementsAreArray(std::vector<TypeParam>(
                                 this->values_.size(),
                                 std::numeric_limits<TypeParam>::lowest())));
}

TEST(WeightedVertex, ItWorks) {
  graph::WeightedVertex<int> v1(30, 2), v2(5), v3;
  EXPECT_EQ(v1.weight, 30);
  EXPECT_EQ(v1.vertex, 2);
  EXPECT_EQ(v2.weight, 5);
  EXPECT_EQ(v2.vertex, graph::null_vertex);
  EXPECT_EQ(v3.weight, 0);
  EXPECT_EQ(v3.vertex, graph::null_vertex);

  graph::WeightedVertex<long> v4 = graph::make_weighted_vertex(12L, 3);
  EXPECT_EQ(v4.weight, 12);
  EXPECT_EQ(v4.vertex, 3);

  v3 = v4;
  EXPECT_EQ(v3.weight, 12);
  EXPECT_EQ(v3.vertex, 3);

  graph::WeightedCompare<std::less<int>> comp;
  EXPECT_TRUE(comp(v2, v1));
  EXPECT_FALSE(comp(v1, v2));

  graph::WeightedReverseCompare<std::less<int>> rcomp;
  EXPECT_TRUE(rcomp(v1, v2));
  EXPECT_FALSE(rcomp(v2, v1));

  EXPECT_TRUE(graph::make_weighted_compare(std::less<int>())(v2, v1));
  EXPECT_FALSE(graph::make_weighted_compare(std::less<int>())(v1, v2));

  EXPECT_TRUE(graph::make_weighted_rcompare(std::less<int>())(v1, v2));
  EXPECT_FALSE(graph::make_weighted_rcompare(std::less<int>())(v2, v1));
}

TEST(PrintTest, OutputStream) {
  CrossList<int> g(12, 12);
  g.rinsert(0, 1, 1);
  g.rinsert(0, 2, 1);
  g.rinsert(3, 4, 1);
  g.rinsert(1, 11, 1);
  g.rinsert(2, 5, 1);
  g.rinsert(4, 5, 1);
  g.rinsert(4, 6, 1);
  g.rinsert(11, 6, 1);
  g.rinsert(5, 7, 1);
  g.rinsert(7, 9, 1);
  g.rinsert(6, 8, 1);
  g.rinsert(8, 9, 1);
  g.rinsert(8, 10, 1);
  std::stringstream ss;
  digraph::print_acyclic(g, ss);
  EXPECT_EQ(ss.str(),
            R"(0
|-->1
|   `-->11
|       |   3
|       |   `-->4
|       `------>|-->6
|               |   `-->8
`---------------|-------|-->2
                `-------|-->`-->5
                        |       `-->7
                        |---------->`-->9
                        `-------------->10
)");
}

TEST(PrintTest, PrefixesAndVertices) {
  CrossList<int> g(22, 22);
  g.rinsert(1, 2, 1);
  g.rinsert(1, 3, 1);
  g.rinsert(2, 4, 1);
  g.rinsert(2, 5, 1);
  g.rinsert(3, 5, 1);
  g.rinsert(3, 6, 1);
  g.rinsert(5, 7, 1);
  g.rinsert(6, 13, 1);
  g.rinsert(8, 9, 1);
  g.rinsert(8, 10, 1);
  g.rinsert(8, 11, 1);
  g.rinsert(9, 12, 1);
  g.rinsert(11, 14, 1);
  g.rinsert(11, 15, 1);
  g.rinsert(12, 13, 1);
  g.rinsert(13, 17, 1);
  g.rinsert(13, 18, 1);
  g.rinsert(14, 16, 1);
  g.rinsert(15, 16, 1);
  g.rinsert(18, 19, 1);
  g.rinsert(18, 20, 1);

  std::vector<std::string> prefixes(g.row_count());
  std::vector<graph::vertex_type> vertices(g.row_count());
  digraph::print_acyclic(g, prefixes.begin(), vertices.begin());
  EXPECT_THAT(prefixes,
              ElementsAreArray(
                  {"",
                   "",
                   "",
                   "|-->",
                   "|   |-->",
                   "`---|-->",
                   "    `-->|-->",
                   "        |   `-->",
                   "        `------>",
                   "                |   ",
                   "                |   |-->",
                   "                |   |   `-->",
                   "                `---|------>`-->",
                   "                    |           |-->",
                   "                    |           `-->",
                   "                    |               |-->",
                   "                    |               `-->",
                   "                    |------------------>",
                   "                    `------------------>",
                   "                                        |-->",
                   "                                        `---|-->",
                   "                                            `-->`-->"}));
  EXPECT_THAT(vertices,
              ElementsAreArray({0,  21, 1,  2,  4,  3,  5,  7,  6,  8,  9,
                                12, 13, 17, 18, 19, 20, 10, 11, 14, 15, 16}));
}
