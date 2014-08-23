#include "math.h"

#include <cassert>
#include <cmath>
#include <iostream>

using namespace std;

bool EQ(double lhs, double rhs) {
  return abs(lhs - rhs) < 1e-6;
}

void TestGaussJordanEliminate() {
  // 8, -2
  Matrix<double> a(2, 3);
  a[0][0] = 3; a[0][1] = 9; a[0][2] = 6;
  a[1][0] = 2; a[1][1] = 4; a[1][2] = 8;

  math::GaussJordanEliminate(&a);

  assert(EQ(a[0][0], 1)); assert(EQ(a[0][1], 0)); assert(EQ(a[0][2], 8));
  assert(EQ(a[1][0], 0)); assert(EQ(a[1][1], 1)); assert(EQ(a[1][2], -2));

  // 2, 3, 4
  Matrix<double> b(5, 4);
  b[0][0] = 5; b[0][1] = 2; b[0][2] = 1; b[0][3] = 20;
  b[1][0] = 4; b[1][1] = 3; b[1][2] = 2; b[1][3] = 25;
  b[2][0] = 2; b[2][1] = 4; b[2][2] = 3; b[2][3] = 28;
  b[3][0] = 3; b[3][1] = 5; b[3][2] = 4; b[3][3] = 37;
  b[4][0] = 1; b[4][1] = 2; b[4][2] = 3; b[4][3] = 20;

  math::GaussJordanEliminate(&b);

  assert(EQ(b[0][0], 1)); assert(EQ(b[0][1], 0));
  assert(EQ(b[0][2], 0)); assert(EQ(b[0][3], 2));

  assert(EQ(b[1][0], 0)); assert(EQ(b[1][1], 1));
  assert(EQ(b[1][2], 0)); assert(EQ(b[1][3], 3));

  assert(EQ(b[2][0], 0)); assert(EQ(b[2][1], 0));
  assert(EQ(b[2][2], 1)); assert(EQ(b[2][3], 4));

  for (int j = 0; j < 4; ++j) {
    assert(EQ(b[3][j], 0));
    assert(EQ(b[4][j], 0));
  }

  // 2, 0, 4
  Matrix<double> c(5, 4);
  c[0][0] = 5; c[0][1] = 2; c[0][2] = 1; c[0][3] = 14;
  c[1][0] = 4; c[1][1] = 3; c[1][2] = 2; c[1][3] = 16;
  c[2][0] = 2; c[2][1] = 4; c[2][2] = 3; c[2][3] = 16;
  c[3][0] = 3; c[3][1] = 5; c[3][2] = 4; c[3][3] = 22;
  c[4][0] = 1; c[4][1] = 2; c[4][2] = 3; c[4][3] = 14;

  math::GaussJordanEliminate(&c);

  assert(EQ(c[0][0], 1)); assert(EQ(c[0][1], 0));
  assert(EQ(c[0][2], 0)); assert(EQ(c[0][3], 2));

  assert(EQ(c[1][0], 0)); assert(EQ(c[1][1], 1));
  assert(EQ(c[1][2], 0)); assert(EQ(c[1][3], 0));

  assert(EQ(c[2][0], 0)); assert(EQ(c[2][1], 0));
  assert(EQ(c[2][2], 1)); assert(EQ(c[2][3], 4));

  for (int j = 0; j < 4; ++j) {
    assert(EQ(c[3][j], 0));
    assert(EQ(c[4][j], 0));
  }

  // 2, 3, (4)
  Matrix<double> d(2, 4);
  d[0][0] = 2; d[0][1] = 3; d[0][2] = 4; d[0][3] = 29;
  d[1][0] = 4; d[1][1] = 3; d[1][2] = 2; d[1][3] = 25;

  math::GaussJordanEliminate(&d);

  assert(EQ(d[0][0], 1)); assert(EQ(d[0][1], 0));
  assert(EQ(d[0][2], -1)); assert(EQ(d[0][3], -2));

  assert(EQ(d[1][0], 0)); assert(EQ(d[1][1], 1));
  assert(EQ(d[1][2], 2)); assert(EQ(d[1][3], 11));

  // not augmented matrix
  Matrix<double> e(1, 0);
  // math::GaussJordanEliminate(&e);  // exception thrown
}

int main(int argc, char* argv[]) {
  TestGaussJordanEliminate();
  return 0;
}
