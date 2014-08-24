#include "math.h"

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
  size_t rank = math::GaussEliminate(&a, &aa);
  assert(rank == 2);
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
  rank = math::GaussEliminate(&b, &bb);
  assert(rank == 3);
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
  rank = math::GaussEliminate(&c);
  assert(rank == 3);
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
  rank = math::GaussEliminate(&d);
  assert(rank = 2);
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

  math::GaussJordanEliminate(&b, &bb);

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

  math::GaussJordanEliminate(&c, &cc);

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

  math::GaussJordanEliminate(&d, &dd);

  assert(d.equal_to({
        {1, 0, -1},
        {0, 1, 2},
      }, EQ));
  assert(dd.equal_to({
        {-2},
        {11},
      }, EQ));

  Matrix<double> e(3, 0);
  math::GaussJordanEliminate(&e);

  math::GaussJordanEliminate(
      &e, static_cast<Matrix<double>*>(nullptr),
      [](const double& a, const double& b) {
        return abs(a) < abs(b);
      },
      [](const double& v) { return abs(v) < 1e-6; });
}

int main(int argc, char* argv[]) {
  TestGaussEliminate();
  TestGaussJordanEliminate();
  return 0;
}
