#include "crosslist.h"

#include <algorithm>
#include <cstdlib>
#include <exception>
#include <iostream>

#include "gtest/gtest.h"

TEST(CrosslistTest, BasicOperations) {
  CrossList<int> c;
  c.reserve(3, 4);
  EXPECT_EQ(c.row_count(), 3);
  EXPECT_EQ(c.column_count(), 4);
  EXPECT_EQ(c.size(), 0);
  EXPECT_TRUE(c.empty());

  CrossList<int> c1(c);
  EXPECT_EQ(c1.row_count(), 3);
  EXPECT_EQ(c1.column_count(), 4);
  EXPECT_TRUE(c1.empty());
  EXPECT_EQ(c1, c);
  EXPECT_FALSE(c1 != c);

  CrossList<int> c2;
  c2 = c;
  EXPECT_EQ(c2, c);

  EXPECT_TRUE(c.insert(1, 2, 5));
  EXPECT_EQ(c.size(), 1);
  EXPECT_TRUE(c.rinsert(2, 1, 3));
  EXPECT_EQ(c(2, 1), 3);
  EXPECT_EQ(c.row_size(1), 1);
  EXPECT_EQ(c.row_size(0), 0);
  EXPECT_EQ(c.column_size(2), 1);
  EXPECT_EQ(c.column_size(0), 0);
  EXPECT_EQ(c.at(1, 2), 5);
  EXPECT_EQ(c.rat(1, 2), 5);
  EXPECT_EQ(c(1, 2), 5);

  const CrossList<int>& rc(c);
  EXPECT_EQ(rc.at(1, 2), 5);
  try {
    rc.at(1, 3);
    FAIL();
  } catch (const std::exception& e) {
    SUCCEED();
  }
  EXPECT_EQ(rc.rat(1, 2), 5);
  try {
    rc.rat(1, 3);
    FAIL();
  } catch (const std::exception& e) {
    SUCCEED();
  }
  EXPECT_EQ(rc(1, 2), 5);
  try {
    rc(1, 3);
    FAIL();
  } catch (const std::exception& e) {
    SUCCEED();
  }

  c.transpose();
  EXPECT_EQ(c.row_count(), 4);
  EXPECT_EQ(c.column_count(), 3);
  EXPECT_EQ(c.size(), 2);
  EXPECT_EQ(c(2, 1), 5);
  EXPECT_EQ(c(1, 2), 3);

  EXPECT_TRUE(c.erase(1u, 2u));
  EXPECT_FALSE(c.erase(0, 0));

  EXPECT_TRUE(c.rerase(2, 1));
  EXPECT_FALSE(c.rerase(0, 0));

  c.set(1, 2, 5);
  EXPECT_EQ(c(1, 2), 5);

  c.rset(2, 1, 4);
  EXPECT_EQ(c(2, 1), 4);

  EXPECT_EQ(rc.get(1, 2), 5);
  EXPECT_EQ(rc.get(0, 0), 0);

  EXPECT_EQ(rc.rget(1, 2), 5);
  EXPECT_EQ(rc.rget(0, 0), 0);

  EXPECT_TRUE(rc.exist(1, 2));
  EXPECT_FALSE(rc.exist(0, 0));

  EXPECT_TRUE(rc.rexist(1, 2));
  EXPECT_FALSE(rc.rexist(0, 0));

  c.clear();
  EXPECT_TRUE(c.empty());

  c.row_reserve(5);
  EXPECT_EQ(c.row_count(), 5);

  c.column_reserve(5);
  EXPECT_EQ(c.column_count(), 5);
}

TEST(CrosslistTest, Iterators) {
  CrossList<int> c(3, 4);
  const CrossList<int>& r(c);

  c.insert(0, 0, 0);
  c.insert(0, 1, 1);
  c.insert(0, 2, 2);
  c.insert(0, 3, 3);
  c.insert(1, 0, 10);
  c.insert(1, 2, 12);
  c.insert(1, 3, 13);
  c.insert(2, 1, 21);
  c.insert(2, 2, 22);

  int entire_checks[] = {0, 1, 2, 3, 10, 12, 13, 21, 22};
  int row_checks[][4] = {{0, 1, 2, 3}, {10, 12, 13}, {21, 22}};
  int column_checks[][4] = {{0, 10}, {1, 21}, {2, 12, 22}, {3, 13}};

  EXPECT_TRUE(std::equal(c.begin(), c.end(), entire_checks));

  EXPECT_TRUE(std::equal(r.begin(), r.end(), entire_checks));

  for (size_t i = 0; i < c.row_count(); ++i) {
    EXPECT_TRUE(std::equal(c.row_begin(i), c.row_end(i), row_checks[i])) << i;
  }

  for (size_t i = 0; i < r.row_count(); ++i) {
    EXPECT_TRUE(std::equal(r.row_begin(i), r.row_end(i), row_checks[i])) << i;
  }

  for (size_t i = 0; i < c.column_count(); ++i) {
    EXPECT_TRUE(
        std::equal(c.column_begin(i), c.column_end(i), column_checks[i]))
        << i;
  }

  for (size_t i = 0; i < r.column_count(); ++i) {
    EXPECT_TRUE(
        std::equal(r.column_begin(i), r.column_end(i), column_checks[i]))
        << i;
  }

  int entire_rchecks[] = {22, 21, 13, 12, 10, 3, 2, 1, 0};
  int row_rchecks[][4] = {{3, 2, 1, 0}, {13, 12, 10}, {22, 21}};
  int column_rchecks[][4] = {{10, 0}, {21, 1}, {22, 12, 2}, {13, 3}};

  EXPECT_TRUE(std::equal(c.rbegin(), c.rend(), entire_rchecks));

  EXPECT_TRUE(std::equal(r.rbegin(), r.rend(), entire_rchecks));

  for (size_t i = 0; i < c.row_count(); ++i) {
    EXPECT_TRUE(std::equal(c.row_rbegin(i), c.row_rend(i), row_rchecks[i]))
        << i;
  }

  for (size_t i = 0; i < r.row_count(); ++i) {
    EXPECT_TRUE(std::equal(r.row_rbegin(i), r.row_rend(i), row_rchecks[i]))
        << i;
  }

  for (size_t i = 0; i < c.column_count(); ++i) {
    EXPECT_TRUE(
        std::equal(c.column_rbegin(i), c.column_rend(i), column_rchecks[i]))
        << i;
  }

  for (size_t i = 0; i < r.column_count(); ++i) {
    EXPECT_TRUE(
        std::equal(r.column_rbegin(i), r.column_rend(i), column_rchecks[i]))
        << i;
  }

  CrossList<int>::iterator iter1, iter2;
  CrossList<int>::row_iterator row_iter1, row_iter2;
  CrossList<int>::column_iterator col_iter1, col_iter2;

  iter1 = c.begin();
  c.erase(iter1);
  EXPECT_FALSE(c.exist(0, 0));

  row_iter1 = c.row_begin(0);
  c.erase(row_iter1);
  EXPECT_FALSE(c.exist(0, 1));

  col_iter1 = c.column_begin(3);
  c.erase(col_iter1);
  EXPECT_FALSE(c.exist(0, 3));

  row_iter1 = c.row_begin(1);
  row_iter2 = c.row_end(1);
  c.erase_range(row_iter1, row_iter2);
  EXPECT_EQ(c.row_size(1), 0);
  col_iter1 = c.column_begin(2);
  col_iter2 = c.column_end(2);
  c.erase_range(col_iter1, col_iter2);
  EXPECT_EQ(c.column_size(2), 0);
  iter1 = c.begin();
  iter2 = c.end();
  c.erase_range(iter1, iter2);
  EXPECT_EQ(c.size(), 0);
}
