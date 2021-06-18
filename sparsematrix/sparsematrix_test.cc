#include "sparsematrix.h"

#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::Pair;

TEST(SparseMatrixTest, BasicOperations) {
  SparseMatrix<int> m1(10, 11), m2(3), m3(2, 2, -1);
  m1(0, 1) = 3;
  m1(0, 2) = 4;
  m1(1, 2) = 5;
  m1(1, 1) = 7;

  EXPECT_EQ(m1(0, 1), 3);
  EXPECT_EQ(m1(0, 2), 4);
  EXPECT_EQ(m1(1, 2), 5);
  EXPECT_EQ(m1(1, 1), 7);

  const SparseMatrix<int>& cm1(m1);
  EXPECT_EQ(cm1(0, 1), 3);
  EXPECT_EQ(cm1(0, 2), 4);
  EXPECT_EQ(cm1(1, 2), 5);
  EXPECT_EQ(cm1(1, 1), 7);

  SparseMatrix<int> m4(m1);
  EXPECT_EQ(m4.size(), 4);
  EXPECT_EQ(3, m4(0, 1));
  EXPECT_EQ(4, m4(0, 2));
  EXPECT_EQ(5, m4(1, 2));
  EXPECT_EQ(7, m4(1, 1));

  m2 = m1;
  EXPECT_EQ(m2.size(), 4);
  EXPECT_EQ(3, m2(0, 1));
  EXPECT_EQ(4, m2(0, 2));
  EXPECT_EQ(5, m2(1, 2));
  EXPECT_EQ(7, m2(1, 1));

  EXPECT_EQ(m1, m2);
  EXPECT_NE(m1, m3);

  m1.iset(3, 4, 8);
  EXPECT_EQ(cm1(3, 4), 8);

  EXPECT_EQ(m1.iget(3, 4), 8);
  EXPECT_EQ(m1.iget(3, 5), 0);

  EXPECT_TRUE(m1.iexist(3, 4));
  EXPECT_FALSE(m1.iexist(3, 5));

  m1.iat(3, 0) = 12;
  EXPECT_EQ(m1(3, 0), 12);

  EXPECT_EQ(cm1.iat(3, 0), 12);
  try {
    cm1.iat(3, 5);
    FAIL();
  } catch (const std::exception&) {
    SUCCEED();
  }

  EXPECT_TRUE(m1.ierase(3, 4));
  EXPECT_FALSE(m1.ierase(3, 5));
  EXPECT_FALSE(m1.iexist(3, 4));
}

TEST(SparseMatrixTest, Sparse) {
  const int R = 10, C = 12;
  SparseMatrix<int> m1(R, C);
  for (int i = 0; i < R * C; ++i) {
    m1(i / C, i % C) = i;
  }
  m1.sparse(5, 6);
  EXPECT_THAT(m1.sparse(), Pair(5, 6));

  m1.transpose();
  EXPECT_EQ(m1.row_count(), C);
  EXPECT_EQ(m1.column_count(), R);
  for (int i = 0; i < R * C; ++i) {
    EXPECT_EQ(m1(i % C, i / C), i);
  }
  EXPECT_THAT(m1.sparse(), Pair(6, 5));
}
