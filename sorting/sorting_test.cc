#include "sorting.h"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <iterator>
#include <list>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace sorting {
namespace {

using ::testing::ElementsAreArray;

class SortingTest : public testing::Test {
 protected:
  std::vector<int> values_ = {3, 4, 1, 2, 8, 7, 10, 20, 15, 12, 15, -3};
  std::vector<int> lengths_ = {static_cast<int>(values_.size()), 3, 2, 1, 0};
  std::vector<std::vector<int>> sorted_ = {
      {-3, 1, 2, 3, 4, 7, 8, 10, 12, 15, 15, 20}, {1, 3, 4}, {3, 4}, {3}, {}};
};

TEST_F(SortingTest, BubbleSort) {
  std::list<int> seq;
  for (size_t t = 0; t < lengths_.size(); ++t) {
    seq.assign(values_.begin(), values_.begin() + lengths_[t]);
    bubble_sort(seq.begin(), seq.end());
    EXPECT_THAT(seq, ElementsAreArray(sorted_[t]));
  }
}

TEST_F(SortingTest, InsertionSort) {
  std::list<int> seq;
  for (size_t t = 0; t < lengths_.size(); ++t) {
    seq.assign(values_.begin(), values_.begin() + lengths_[t]);
    sorting::insertion_sort(seq.begin(), seq.end());
    EXPECT_THAT(seq, ElementsAreArray(sorted_[t]));
  }
}

TEST_F(SortingTest, SelectionSort) {
  std::list<int> seq;
  for (size_t t = 0; t < lengths_.size(); ++t) {
    seq.assign(values_.begin(), values_.begin() + lengths_[t]);
    sorting::selection_sort(seq.begin(), seq.end());
    EXPECT_THAT(seq, ElementsAreArray(sorted_[t]));
  }
}

TEST_F(SortingTest, ShellSort) {
  std::vector<int> seq;
  for (size_t t = 0; t < lengths_.size(); ++t) {
    seq.assign(values_.begin(), values_.begin() + lengths_[t]);
    sorting::shell_sort(seq.begin(), seq.end());
    EXPECT_THAT(seq, ElementsAreArray(sorted_[t]));
  }
}

TEST_F(SortingTest, HeapSort) {
  std::vector<int> seq;
  for (size_t t = 0; t < lengths_.size(); ++t) {
    seq.assign(values_.begin(), values_.begin() + lengths_[t]);
    sorting::heap_sort(seq.begin(), seq.end());
    EXPECT_THAT(seq, ElementsAreArray(sorted_[t]));
  }
}

TEST(MergeTest, ItWorks) {
  std::vector<int> mvalues1 = {1, 3, 5, 8, 12, 29};
  std::vector<int> mvalues2 = {4, 7, 10, 18, 25, 32};
  std::vector<int> mvalues3 = {2};
  std::vector<int> mvalues4 = {};
  std::vector<int> mvalues5 = {};
  std::vector<int> mvalues12_check = {1, 3, 4, 5, 7, 8, 10, 12, 18, 25, 29, 32};
  std::vector<int> mvalues34_check = {2};
  std::vector<int> mvalues45_check = {};
  std::vector<int> mvalues13_check = {1, 2, 3, 5, 8, 12, 29};
  std::vector<int> mvalues24_check = {4, 7, 10, 18, 25, 32};

  std::vector<int> check;

  check.resize(mvalues12_check.size());
  auto iter_check =
      sorting::merge(mvalues1.begin(), mvalues1.end(), mvalues2.begin(),
                     mvalues2.end(), check.begin());
  EXPECT_EQ(iter_check, check.end());
  EXPECT_THAT(check, ElementsAreArray(mvalues12_check));

  check.resize(mvalues34_check.size());
  iter_check = sorting::merge(mvalues3.begin(), mvalues3.end(),
                              mvalues4.begin(), mvalues4.end(), check.begin());
  EXPECT_EQ(iter_check, check.end());
  EXPECT_THAT(check, ElementsAreArray(mvalues34_check));

  check.resize(mvalues45_check.size());
  iter_check = sorting::merge(mvalues4.begin(), mvalues4.end(),
                              mvalues5.begin(), mvalues5.end(), check.begin());
  EXPECT_EQ(iter_check, check.end());
  EXPECT_THAT(check, ElementsAreArray(mvalues45_check));

  check.resize(mvalues13_check.size());
  iter_check = sorting::merge(mvalues1.begin(), mvalues1.end(),
                              mvalues3.begin(), mvalues3.end(), check.begin());
  EXPECT_EQ(iter_check, check.end());
  EXPECT_THAT(check, ElementsAreArray(mvalues13_check));

  check.resize(mvalues24_check.size());
  iter_check = sorting::merge(mvalues2.begin(), mvalues2.end(),
                              mvalues4.begin(), mvalues4.end(), check.begin());
  EXPECT_EQ(iter_check, check.end());
  EXPECT_THAT(check, ElementsAreArray(mvalues24_check));
}

TEST_F(SortingTest, MergeSort) {
  std::vector<int> seq, buffer;
  for (size_t t = 0; t < lengths_.size(); ++t) {
    seq.assign(values_.begin(), values_.begin() + lengths_[t]);
    buffer.resize(seq.size());
    sorting::merge_sort(seq.begin(), seq.end(), buffer.begin());
    EXPECT_THAT(seq, ElementsAreArray(sorted_[t]));
  }
}

TEST(PartitionTest, ItWorks) {
  std::vector<std::vector<int>> parts = {
      {},
      {1},
      {1, -1},
      {1, -1, 25, -2, -8, 8, 22},
      {25, 25, 25, 25, 25},
      {-8, -8, -8, -8, -8},
  };
  std::vector<std::vector<int>> checks = {
      {},
      {1},
      {-1, 1},
      {-8, -1, -2, 25, 1, 8, 22},
      {25, 25, 25, 25, 25},
      {-8, -8, -8, -8, -8},
  };
  std::vector<int> check_poss = {0, 0, 1, 3, 0, 5};

  std::list<int> lst;
  for (size_t t = 0; t < parts.size(); ++t) {
    lst.assign(parts[t].begin(), parts[t].end());
    auto iter =
        sorting::partition(lst.begin(), lst.end(), [](int v) { return v < 0; });
    EXPECT_EQ(std::distance(lst.begin(), iter), check_poss[t]) << t;
    EXPECT_THAT(lst, ElementsAreArray(checks[t])) << t;
  }
}

TEST_F(SortingTest, QuickSort) {
  std::vector<int> seq;
  for (size_t t = 0; t < lengths_.size(); ++t) {
    seq.assign(values_.begin(), values_.begin() + lengths_[t]);
    sorting::quick_sort(seq.begin(), seq.end());
    EXPECT_THAT(seq, ElementsAreArray(sorted_[t]));
  }
}

TEST_F(SortingTest, IndirectSort) {
  std::vector<int> seq;
  for (size_t t = 0; t < lengths_.size(); ++t) {
    seq.assign(values_.begin(), values_.begin() + lengths_[t]);
    sorting::indirect_sort(seq.begin(), seq.end());
    EXPECT_THAT(seq, ElementsAreArray(sorted_[t]));
  }
}

TEST(BucketSort, ItWorks) {
  std::vector<int> values = {3, 4, 1, 2, 8, 7, 10, 15, 12, 20, 0x7fffffff, -3};
  int len = static_cast<int>(values.size());
  std::vector<int> lengths = {len, len - 1, len - 2, 3, 2, 1, 0};
  std::vector<std::vector<int>> sorted = {
      {}, {}, {1, 2, 3, 4, 7, 8, 10, 12, 15, 20}, {1, 3, 4}, {3, 4}, {3}, {}};

  std::list<int> seq;
  for (int t = 0; t < 2; ++t) {
    seq.assign(values.begin(), values.begin() + lengths[t]);
    try {
      sorting::bucket_sort(seq.begin(), seq.end());
      FAIL();
    } catch (const std::exception&) {
      SUCCEED();
    }
  }

  for (size_t t = 2; t < lengths.size(); ++t) {
    for (const auto& ascending : {false, true}) {
      seq.assign(values.begin(), values.begin() + lengths[t]);
      sorting::bucket_sort(seq.begin(), seq.end(), ascending);
      if (!ascending) {
        reverse(seq.begin(), seq.end());
      }
      EXPECT_THAT(seq, ElementsAreArray(sorted[t])) << t << " " << ascending;
    }
  }
}

class SelectTest : public SortingTest {
 protected:
  std::vector<int> poss_ = {11, 1, 1, -1, 0};
  std::vector<bool> poss_legal_ = {true, true, true, false, false};
  std::vector<int> sorted_ = {20, 3, 4, 1, 0};
};

TEST_F(SelectTest, QuickSelect) {
  std::vector<int> seq;
  for (size_t t = 0; t < lengths_.size(); ++t) {
    seq.assign(values_.begin(), values_.begin() + lengths_[t]);
    auto iter = sorting::quick_select(seq.begin(), seq.end(), poss_[t]);
    EXPECT_EQ(iter != seq.end(), poss_legal_[t]) << t;
    if (iter != seq.end()) {
      EXPECT_EQ(*iter, sorted_[t]);
    }
  }
}

TEST_F(SelectTest, HeapSelect) {
  std::vector<int> seq;
  for (size_t t = 0; t < lengths_.size(); ++t) {
    seq.assign(values_.begin(), values_.begin() + lengths_[t]);
    auto iter = sorting::heap_select(seq.begin(), seq.end(), poss_[t]);
    EXPECT_EQ(iter != seq.end(), poss_legal_[t]) << t;
    if (iter != seq.end()) {
      EXPECT_EQ(*iter, sorted_[t]);
    }
  }
}

}  // namespace
}  // namespace sorting
