#include "linear_algebra.h"

#include <cassert>
#include <cmath>
#include <iostream>

#include "gtest/gtest.h"

bool EQ(const double& lhs, const double& rhs) {
  return std::abs(lhs - rhs) < 1e-6;
}

TEST(GaussEliminateTest, ItWorks) {
  matrix::Matrix<double> a =
                             {
                                 {2, 3},
                                 {1, 2},
                             },
                         aa = {
                             {4},
                             {5},
                         };
  size_t rank = linear_algebra::GaussEliminate(&a, &aa);
  EXPECT_EQ(2, rank);
  EXPECT_TRUE(a.equal_to(
      {
          {2, 3},
          {0, 0.5},
      },
      EQ));
  EXPECT_TRUE(aa.equal_to(
      {
          {4},
          {3},
      },
      EQ));

  matrix::Matrix<double> b =
                             {
                                 {1, 3, 4},
                                 {3, 2, 6},
                                 {2, 4, 8},
                             },
                         bb = {
                             {4},
                             {2},
                             {7},
                         };
  rank = linear_algebra::GaussEliminate(&b, &bb);
  EXPECT_EQ(3, rank);
  EXPECT_TRUE(b.equal_to(
      {
          {3, 2, 6},
          {0, 8.0 / 3, 4},
          {0, 0, -1.5},
      },
      EQ));
  EXPECT_TRUE(bb.equal_to(
      {
          {2},
          {17.0 / 3},
          {-13.0 / 8},
      },
      EQ));

  matrix::Matrix<double> c = {
      {1, 3, 4},
      {3, 2, 6},
      {2, 4, 8},
  };
  rank = linear_algebra::GaussEliminate(&c);
  EXPECT_EQ(3, rank);
  EXPECT_TRUE(c.equal_to(
      {
          {3, 2, 6},
          {0, 8.0 / 3, 4},
          {0, 0, -1.5},
      },
      EQ));

  matrix::Matrix<double> d = {
      {1, 2, 3},
      {4, 5, 6},
      {2, 4, 6},
  };
  rank = linear_algebra::GaussEliminate(&d);
  EXPECT_EQ(2, rank);

  matrix::Matrix<double> e = {
      {1, 0, 0, 0},
      {0, 0, 0, 1},
      {0, 0, 1, 0},
  };
  rank = linear_algebra::GaussEliminate(&e);
  EXPECT_EQ(3, rank);
  EXPECT_TRUE(e.equal_to(
      {
          {1, 0, 0, 0},
          {0, 0, 1, 0},
          {0, 0, 0, 1},
      },
      EQ));

  matrix::Matrix<double> f = {
      {2, 4, 0, 0},
      {0, 0, 0, 5},
      {0, 0, 3, 0},
  };
  typedef matrix::Matrix<double>::size_type size_type;
  std::vector<size_type> pivot_columns;
  rank = linear_algebra::GaussEliminate<double>(
      &f, nullptr, &pivot_columns, linear_algebra::kReducedRowEchelonForm);
  EXPECT_EQ(3, rank);
  EXPECT_EQ(std::vector<size_type>({0, 2, 3}), pivot_columns);
  EXPECT_TRUE(f.equal_to(
      {
          {1, 2, 0, 0},
          {0, 0, 1, 0},
          {0, 0, 0, 1},
      },
      EQ));
}

TEST(GaussJordanEliminateTest, ItWorks) {
  std::size_t rank = 0;

  // 8, -2
  matrix::Matrix<double> a =
                             {
                                 {3, 9},
                                 {2, 4},
                             },
                         aa = {
                             {6},
                             {8},
                         };
  rank = linear_algebra::GaussJordanEliminate(&a, &aa);
  EXPECT_EQ(2, rank);
  EXPECT_TRUE(a.equal_to({{1, 0}, {0, 1}}, EQ));
  EXPECT_TRUE(aa.equal_to({{8}, {-2}}, EQ));

  // 2, 3, 4
  matrix::Matrix<double> b =
                             {
                                 {5, 2, 1}, {4, 3, 2}, {2, 4, 3},
                                 {3, 5, 4}, {1, 2, 3},
                             },
                         bb = {
                             {20}, {25}, {28}, {37}, {20},
                         };
  rank = linear_algebra::GaussJordanEliminate(&b, &bb);
  EXPECT_EQ(3, rank);
  EXPECT_TRUE(b.equal_to(
      {
          {1, 0, 0},
          {0, 1, 0},
          {0, 0, 1},
          {0, 0, 0},
          {0, 0, 0},
      },
      EQ));
  EXPECT_TRUE(bb.equal_to(
      {
          {2},
          {3},
          {4},
          {0},
          {0},
      },
      EQ));

  // 2, 0, 4
  matrix::Matrix<double> c =
                             {
                                 {5, 2, 1}, {4, 3, 2}, {2, 4, 3},
                                 {3, 5, 4}, {1, 2, 3},
                             },
                         cc = {
                             {14}, {16}, {16}, {22}, {14},
                         };
  rank = linear_algebra::GaussJordanEliminate(&c, &cc);
  EXPECT_EQ(3, rank);
  EXPECT_TRUE(c.equal_to(
      {
          {1, 0, 0},
          {0, 1, 0},
          {0, 0, 1},
          {0, 0, 0},
          {0, 0, 0},
      },
      EQ));
  EXPECT_TRUE(cc.equal_to(
      {
          {2},
          {0},
          {4},
          {0},
          {0},
      },
      EQ));

  // 2, 3, (4)
  matrix::Matrix<double> d =
                             {
                                 {2, 3, 4},
                                 {4, 3, 2},
                             },
                         dd = {
                             {29},
                             {25},
                         };
  rank = linear_algebra::GaussJordanEliminate(&d, &dd);
  EXPECT_EQ(2, rank);
  EXPECT_TRUE(d.equal_to(
      {
          {1, 0, -1},
          {0, 1, 2},
      },
      EQ));
  EXPECT_TRUE(dd.equal_to(
      {
          {-2},
          {11},
      },
      EQ));

  matrix::Matrix<double> e(3, 0);
  rank = linear_algebra::GaussJordanEliminate(&e);
  EXPECT_EQ(0, rank);

  rank = linear_algebra::GaussJordanEliminate<double>(
      &e, nullptr, nullptr,
      [](const double& a, const double& b) {
        return std::abs(a) < std::abs(b);
      },
      [](const double& v) { return std::abs(v) < 1e-6; });
  EXPECT_EQ(0, rank);

  matrix::Matrix<double> f = {
      {1, 0, 0, 0},
      {0, 0, 0, 1},
      {0, 0, 1, 0},
  };
  rank = linear_algebra::GaussJordanEliminate(&f);
  EXPECT_EQ(3, rank);
  EXPECT_TRUE(f.equal_to(
      {
          {1, 0, 0, 0},
          {0, 0, 1, 0},
          {0, 0, 0, 1},
      },
      EQ));
}

TEST(SolveLinearSystemTest, ItWorks) {
  matrix::Matrix<double> a =
                             {
                                 {1, 2, 2, 2},
                                 {2, 4, 6, 8},
                                 {3, 6, 8, 10},
                             },
                         b =
                             {
                                 {1},
                                 {5},
                                 {6},
                             },
                         null_space, particular_solution;
  bool solvable = linear_algebra::SolveLinearSystem(
      &a, &b, &null_space, &particular_solution,
      linear_algebra::TrivialAbsLess<double>(),
      [](double value) { return std::abs(value) < 1e-6; });
  EXPECT_TRUE(solvable);
  EXPECT_TRUE(null_space.equal_to(
      {
          {-2, 2},
          {1, 0},
          {0, -2},
          {0, 1},
      },
      EQ));
  EXPECT_TRUE(particular_solution.equal_to(
      {
          {-2},
          {0},
          {3.0 / 2},
          {0},
      },
      EQ));
}
