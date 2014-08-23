#include "math.h"

#include <cassert>
#include <cmath>
#include <iostream>

using namespace std;

bool EQ(const double& lhs, const double& rhs) {
  return abs(lhs - rhs) < 1e-6;
}

void TestGaussJordanEliminate() {
  // 8, -2
  Matrix<double> a = {
    {3, 9},
    {2, 4},
  }, aa = {
    {6},
    {8},
  };

  math::GaussJordanEliminate(&a, &aa);

  assert(EQ(a[0][0], 1)); assert(EQ(a[0][1], 0));
  assert(EQ(a[1][0], 0)); assert(EQ(a[1][1], 1));

  assert(EQ(aa[0][0], 8));
  assert(EQ(aa[1][0], -2));

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

  math::GaussJordanEliminate(&b, &bb);

  assert(EQ(b[0][0], 1)); assert(EQ(b[0][1], 0)); assert(EQ(b[0][2], 0));
  assert(EQ(b[1][0], 0)); assert(EQ(b[1][1], 1)); assert(EQ(b[1][2], 0));
  assert(EQ(b[2][0], 0)); assert(EQ(b[2][1], 0)); assert(EQ(b[2][2], 1));
  for (int j = 0; j < 3; ++j) {
    assert(EQ(b[3][j], 0));
    assert(EQ(b[4][j], 0));
  }

  assert(EQ(bb[0][0], 2));
  assert(EQ(bb[1][0], 3));
  assert(EQ(bb[2][0], 4));
  assert(EQ(bb[3][0], 0));
  assert(EQ(bb[4][0], 0));

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

  math::GaussJordanEliminate(&c, &cc);

  assert(EQ(c[0][0], 1)); assert(EQ(c[0][1], 0)); assert(EQ(c[0][2], 0));
  assert(EQ(c[1][0], 0)); assert(EQ(c[1][1], 1)); assert(EQ(c[1][2], 0));
  assert(EQ(c[2][0], 0)); assert(EQ(c[2][1], 0)); assert(EQ(c[2][2], 1));
  for (int j = 0; j < 3; ++j) {
    assert(EQ(c[3][j], 0));
    assert(EQ(c[4][j], 0));
  }

  assert(EQ(cc[0][0], 2));
  assert(EQ(cc[1][0], 0));
  assert(EQ(cc[2][0], 4));
  assert(EQ(cc[3][0], 0));
  assert(EQ(cc[4][0], 0));


  // 2, 3, (4)
  Matrix<double> d = {
    {2, 3, 4},
    {4, 3, 2},
  }, dd = {
    {29},
    {25},
  };

  math::GaussJordanEliminate(&d, &dd);

  assert(EQ(d[0][0], 1)); assert(EQ(d[0][1], 0)); assert(EQ(d[0][2], -1));
  assert(EQ(d[1][0], 0)); assert(EQ(d[1][1], 1)); assert(EQ(d[1][2], 2));

  assert(EQ(dd[0][0], -2));
  assert(EQ(dd[1][0], 11));

  Matrix<double> e(3, 0);
  math::GaussJordanEliminate(&e);  // exception thrown
}

int main(int argc, char* argv[]) {
  TestGaussJordanEliminate();
  return 0;
}
