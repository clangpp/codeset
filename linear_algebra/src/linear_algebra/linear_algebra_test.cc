#include "linear_algebra.h"

#include <cassert>
#include <cmath>
#include <iostream>

using namespace std;

bool EQ(const double& lhs, const double& rhs) {
  return abs(lhs - rhs) < 1e-6;
}

void TestGaussEliminate() {
  Matrix<double> a = {
    {2, 3},
    {1, 2},
  }, aa = {
    {4},
    {5},
  };
  size_t rank = linear_algebra::GaussEliminate(&a, &aa);
  assert(2 == rank);
  assert(a.equal_to({
        {2, 3},
        {0, 0.5},
      }, EQ));
  assert(aa.equal_to({
        {4},
        {3},
      }, EQ));

  Matrix<double> b = {
    {1, 3, 4},
    {3, 2, 6},
    {2, 4, 8},
  }, bb = {
    {4},
    {2},
    {7},
  };
  rank = linear_algebra::GaussEliminate(&b, &bb);
  assert(3 == rank);
  assert(b.equal_to({
        {3, 2, 6},
        {0, 8.0/3, 4},
        {0, 0, -1.5},
      }, EQ));
  assert(bb.equal_to({
        {2},
        {17.0/3},
        {-13.0/8},
      }, EQ));

  Matrix<double> c = {
    {1, 3, 4},
    {3, 2, 6},
    {2, 4, 8},
  };
  rank = linear_algebra::GaussEliminate(&c);
  assert(3 == rank);
  assert(c.equal_to({
        {3, 2, 6},
        {0, 8.0/3, 4},
        {0, 0, -1.5},
      }, EQ));

  Matrix<double> d = {
    {1, 2, 3},
    {4, 5, 6},
    {2, 4, 6},
  };
  rank = linear_algebra::GaussEliminate(&d);
  assert(2 == rank);

  Matrix<double> e = {
    {1, 0, 0, 0},
    {0, 0, 0, 1},
    {0, 0, 1, 0},
  };
  rank = linear_algebra::GaussEliminate(&e);
  assert(3 == rank);
  assert(e.equal_to({
        {1, 0, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
      }, EQ));

  Matrix<double> f = {
    {2, 4, 0, 0},
    {0, 0, 0, 5},
    {0, 0, 3, 0},
  };
  typedef Matrix<double>::size_type size_type;
  vector<size_type> pivot_columns;
  rank = linear_algebra::GaussEliminate<double>(
      &f, nullptr, &pivot_columns, linear_algebra::kReducedRowEchelonForm);
  assert(3 == rank);
  assert(vector<size_type>({0, 2, 3}) == pivot_columns);
  assert(f.equal_to({
        {1, 2, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
      }, EQ));
}

void TestGaussJordanEliminate() {
  std::size_t rank = 0;

  // 8, -2
  Matrix<double> a = {
    {3, 9},
    {2, 4},
  }, aa = {
    {6},
    {8},
  };
  rank = linear_algebra::GaussJordanEliminate(&a, &aa);
  assert(2 == rank);
  assert(a.equal_to({
        {1, 0},
        {0, 1}
      }, EQ));
  assert(aa.equal_to({
        {8},
        {-2}
      }, EQ));

  // 2, 3, 4
  Matrix<double> b = {
    {5, 2, 1},
    {4, 3, 2},
    {2, 4, 3},
    {3, 5, 4},
    {1, 2, 3},
  }, bb = {
    {20},
    {25},
    {28},
    {37},
    {20},
  };
  rank = linear_algebra::GaussJordanEliminate(&b, &bb);
  assert(3 == rank);
  assert(b.equal_to({
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1},
        {0, 0, 0},
        {0, 0, 0},
      }, EQ));
  assert(bb.equal_to({
        {2},
        {3},
        {4},
        {0},
        {0},
      }, EQ));

  // 2, 0, 4
  Matrix<double> c = {
    {5, 2, 1},
    {4, 3, 2},
    {2, 4, 3},
    {3, 5, 4},
    {1, 2, 3},
  }, cc = {
    {14},
    {16},
    {16},
    {22},
    {14},
  };
  rank = linear_algebra::GaussJordanEliminate(&c, &cc);
  assert(3 == rank);
  assert(c.equal_to({
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1},
        {0, 0, 0},
        {0, 0, 0},
      }, EQ));
  assert(cc.equal_to({
        {2},
        {0},
        {4},
        {0},
        {0},
      }, EQ));

  // 2, 3, (4)
  Matrix<double> d = {
    {2, 3, 4},
    {4, 3, 2},
  }, dd = {
    {29},
    {25},
  };
  rank = linear_algebra::GaussJordanEliminate(&d, &dd);
  assert(2 == rank);
  assert(d.equal_to({
        {1, 0, -1},
        {0, 1, 2},
      }, EQ));
  assert(dd.equal_to({
        {-2},
        {11},
      }, EQ));

  Matrix<double> e(3, 0);
  rank = linear_algebra::GaussJordanEliminate(&e);
  assert(0 == rank);

  rank = linear_algebra::GaussJordanEliminate<double>(
      &e, nullptr, nullptr,
      [](const double& a, const double& b) {
        return abs(a) < abs(b);
      },
      [](const double& v) { return abs(v) < 1e-6; });
  assert(0 == rank);

  Matrix<double> f = {
    {1, 0, 0, 0},
    {0, 0, 0, 1},
    {0, 0, 1, 0},
  };
  rank = linear_algebra::GaussJordanEliminate(&f);
  assert(3 == rank);
  assert(f.equal_to({
        {1, 0, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
      }, EQ));
}

int main(int argc, char* argv[]) {
  TestGaussEliminate();
  TestGaussJordanEliminate();
  return 0;
}
