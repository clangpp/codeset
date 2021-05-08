#include "disjoint_set.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::ElementsAreArray;

TEST(UnionSetsTest, Plain) {
  std::vector<int> sets(8, -1);
  disjoint_set::union_sets_plain(sets.begin(), 4, 5);
  disjoint_set::union_sets_plain(sets.begin(), 6, 7);
  disjoint_set::union_sets_plain(sets.begin(), 4, 6);
  disjoint_set::union_sets_plain(sets.begin(), 3, 4);
  EXPECT_THAT(sets, ElementsAreArray({-1, -1, -1, -1, 3, 4, 4, 6}));
}

TEST(UnionSetsTest, BySize) {
  std::vector<int> sets(8, -1);
  disjoint_set::union_sets_by_size(sets.begin(), 4, 5);
  disjoint_set::union_sets_by_size(sets.begin(), 6, 7);
  disjoint_set::union_sets_by_size(sets.begin(), 4, 6);
  disjoint_set::union_sets_by_size(sets.begin(), 3, 4);
  EXPECT_THAT(sets, ElementsAreArray({-1, -1, -1, 4, -5, 4, 4, 6}));
}

TEST(UnionSetsTest, ByHeight) {
  std::vector<int> sets(8, -1);
  disjoint_set::union_sets_by_height(sets.begin(), 4, 5);
  disjoint_set::union_sets_by_height(sets.begin(), 6, 7);
  disjoint_set::union_sets_by_height(sets.begin(), 4, 6);
  disjoint_set::union_sets_by_height(sets.begin(), 3, 4);
  EXPECT_THAT(sets, ElementsAreArray({-1, -1, -1, 4, -3, 4, 4, 6}));
}

TEST(UnionSetsTest, BySize16) {
  std::vector<int> sets(16, -1);
  for (int step = 1; step < sets.size(); step *= 2) {
    for (int i = 0; i < sets.size(); i += 2 * step) {
      disjoint_set::union_sets_by_size(sets.begin(), i, i + step);
    }
  }
  EXPECT_THAT(sets, ElementsAreArray({-16, 0, 0, 2, 0, 4, 4, 6, 0, 8, 8, 10, 8,
                                      12, 12, 14}));
}

TEST(FindTest, Plain) {
  std::vector<int> worst = {-16, 0, 0, 2,  0, 4,  4,  6,
                            0,   8, 8, 10, 8, 12, 12, 14};
  std::vector<int> sets = worst;
  EXPECT_EQ(disjoint_set::find_plain(sets.begin(), 14), 0);
  EXPECT_THAT(sets, ElementsAreArray(worst));  // Tree unchanged.

  EXPECT_EQ(disjoint_set::find_compress_path(sets.begin(), 14), 0);
  EXPECT_THAT(sets, ElementsAreArray(
                        {-16, 0, 0, 2, 0, 4, 4, 6, 0, 8, 8, 10, 0, 12, 0, 14}));
}

TEST(FindTest, CompressPath) {
  std::vector<int> sets = {-16, 0, 0, 2,  0, 4,  4,  6,
                           0,   8, 8, 10, 8, 12, 12, 14};
  EXPECT_EQ(disjoint_set::find_compress_path(sets.begin(), 14), 0);
  EXPECT_THAT(sets, ElementsAreArray(
                        {-16, 0, 0, 2, 0, 4, 4, 6, 0, 8, 8, 10, 0, 12, 0, 14}));
}

class DisjointSetTest : public testing::Test {
 protected:
  DisjointSetTest() : ds(N), cds(ds) {}

  const int N = 8;
  DisjointSet ds;
  const DisjointSet& cds;
};

TEST_F(DisjointSetTest, UnionSets_EdgeCases) {
  try {
    ds.union_sets(-1, 0);
    FAIL();
  } catch (const std::exception& e) {
    SUCCEED();
  }
  try {
    ds.union_sets(0, -1);
    FAIL();
  } catch (const std::exception& e) {
    SUCCEED();
  }
  try {
    ds.union_sets(0, N);
    FAIL();
  } catch (const std::exception& e) {
    SUCCEED();
  }
  try {
    ds.union_sets(N, 0);
    FAIL();
  } catch (const std::exception& e) {
    SUCCEED();
  }
  try {
    ds.union_sets(-1, N);
    FAIL();
  } catch (const std::exception& e) {
    SUCCEED();
  }
  try {
    ds.union_sets(N, -1);
    FAIL();
  } catch (const std::exception& e) {
    SUCCEED();
  }
}

TEST_F(DisjointSetTest, SetCount) {
  EXPECT_EQ(ds.set_count(), N);
  ds.union_sets(4, 5);
  EXPECT_EQ(cds.set_count(), N - 1);
}

TEST_F(DisjointSetTest, SetSizeConst) {
  try {
    cds.set_size(-1);
    FAIL();
  } catch (const std::exception& e) {
    SUCCEED();
  }
  try {
    cds.set_size(N);
    FAIL();
  } catch (const std::exception& e) {
    SUCCEED();
  }
  ds.union_sets(4, 5);
  EXPECT_EQ(cds.set_size(4), 2);
  EXPECT_EQ(cds.set_size(5), 2);
}

TEST_F(DisjointSetTest, Size) {
  EXPECT_EQ(cds.size(), N);
  ds.union_sets(6, 7);
  EXPECT_EQ(cds.size(), N);
}

TEST_F(DisjointSetTest, SetSize) {
  ds.union_sets(4, 5);
  ds.union_sets(6, 7);
  EXPECT_EQ(ds.set_size(4), 2);
  EXPECT_EQ(ds.set_size(5), 2);
  ds.union_sets(4, 6);
  EXPECT_EQ(ds.set_size(4), 4);
  EXPECT_EQ(ds.set_size(7), 4);
}

TEST_F(DisjointSetTest, UnionSets) {
  ds.union_sets(4, 5);
  ds.union_sets(6, 7);
  ds.union_sets(4, 6);
  ds.union_sets(3, 4);
  EXPECT_EQ(ds.set_count(), 4);
  EXPECT_EQ(ds.set_size(4), 5);
  EXPECT_FALSE(ds.union_sets(3, 6));
  EXPECT_FALSE(ds.union_sets(4, 4));
  EXPECT_EQ(cds.set_count(), 4);
}

class DisjointSetFindTest : public testing::Test {
 protected:
  DisjointSetFindTest() : ds(N), cds(ds) {
    for (int step = 1; step < N; step *= 2) {
      for (int i = 0; i < N; i += 2 * step) {
        ds.union_sets(i, i + step);
      }
    }
  }

  const int N = 16;
  DisjointSet ds;
  const DisjointSet& cds;
};

TEST_F(DisjointSetFindTest, Const) {
  try {
    cds.find(-1);
    FAIL();
  } catch (const std::exception& e) {
    SUCCEED();
  }
  try {
    cds.find(N);
    FAIL();
  } catch (const std::exception& e) {
    SUCCEED();
  }
  EXPECT_EQ(cds.find(14), 0);
}

TEST_F(DisjointSetFindTest, NonConst) {
  try {
    ds.find(-1);
    FAIL();
  } catch (const std::exception& e) {
    SUCCEED();
  }
  try {
    ds.find(N);
    FAIL();
  } catch (const std::exception& e) {
    SUCCEED();
  }
  EXPECT_EQ(ds.find(14), 0);
}
