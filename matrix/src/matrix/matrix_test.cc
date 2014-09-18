#include "matrix.h"

#include <cassert>
#include <cmath>
#include <cstddef>  // size_t
#include <chrono>
#include <future>
#include <iostream>

using namespace std;

void TestConstructors() {
  Matrix<int> a(2, 4, 3);
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(a(i, j) == 3);
    }
  }
  Matrix<int> b(3, 5);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 5; ++j) {
      assert(b(i, j) == 0);
    }
  }
  Matrix<int> c;
  assert(c.row_size() == 0);
  assert(c.column_size() == 0);
  Matrix<int> d(a);
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(d(i, j) == 3);
    }
  }
  Matrix<int> e(Matrix<int>(2, 4, 3));
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(e(i, j) == 3);
    }
  }
  Matrix<int> f(std::move(e));
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(f(i, j) == 3);
    }
  }
  Matrix<int> g = {
    {0, 1, 2},
    {1, 2, 3},
  };
  assert(g.row_size() == 2);
  assert(g.column_size() == 3);
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 3; ++j) {
      assert(g(i, j) == i + j);
    }
  }
}

void TestObservers() {
  Matrix<int> a(2, 4, 3), b;
  assert(a.row_size() == 2);
  assert(a.column_size() == 4);
  assert(a.size() == 2 * 4);
  assert(a.dimension().first == 2);
  assert(a.dimension().second == 4);
  assert(!a.empty());
  assert(b.empty());
  const Matrix<int>& ra(a);
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(a(i, j) == 3);
      assert(ra(i, j) == 3);
      assert(a[i][j] == 3);
      assert(ra[i][j] == 3);
      a(i, j) = 4;
      assert(a(i, j) == 4);
      assert(a[i][j] == 4);
      a[i][j] = 5;
      assert(a(i, j) == 5);
      assert(a[i][j] == 5);
    }
  }
}

void TestAssignments() {
  Matrix<int> a(2, 4, 3), b, c;
  b = a;
  assert(a.row_size() == b.row_size());
  assert(a.column_size() == b.column_size());
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(a[i][j] == b[i][j]);
    }
  }
  b = b;
  assert(a.row_size() == b.row_size());
  assert(a.column_size() == b.column_size());
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(a[i][j] == b[i][j]);
    }
  }
  c = std::move(b);
  assert(a.row_size() == c.row_size());
  assert(a.column_size() == c.column_size());
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(a[i][j] == c[i][j]);
    }
  }
  c = std::move(c);
  assert(a.row_size() == c.row_size());
  assert(a.column_size() == c.column_size());
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(a[i][j] == c[i][j]);
    }
  }
}

void TestIterators() {
  Matrix<int> a(2, 3);
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 3; ++j) {
      a[i][j] = i + j;
    }
  }
  for (int row = 0; row < 2; ++row) {
    int column = 0;
    for (auto row_iter = a.row_begin(row);
        row_iter != a.row_end(row); ++row_iter, ++column) {
      assert(*row_iter == row + column);
    }
  }
  for (int column = 0; column < 2; ++column) {
    int row = 0;
    for (auto column_iter = a.column_begin(column);
        column_iter != a.column_end(column); ++column_iter, ++row) {
      assert(*column_iter == row + column);
    }
  }
  const Matrix<int>& cra(a);
  for (int row = 0; row < 2; ++row) {
    int column = 0;
    for (auto row_iter = cra.row_begin(row);
        row_iter != cra.row_end(row); ++row_iter, ++column) {
      assert(*row_iter == row + column);
    }
  }
  for (int column = 0; column < 2; ++column) {
    int row = 0;
    for (auto column_iter = cra.column_begin(column);
        column_iter != cra.column_end(column); ++column_iter, ++row) {
      assert(*column_iter == row + column);
    }
  }
}

void TestManipulators() {
  Matrix<int> a(3, 5, 3);
  a.row_resize(4, 30);
  assert(a.row_size() == 4);
  assert(a.column_size() == 5);
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 5; ++j) {
      assert(a[i][j] == (i<3 ? 3 : 30));
    }
  }
  a.row_resize(2);
  assert(a.row_size() == 2);
  assert(a.column_size() == 5);
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 5; ++j) {
      assert(a[i][j] == 3);
    }
  }
  a.column_resize(7, 70);
  assert(a.row_size() == 2);
  assert(a.column_size() == 7);
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 7; ++j) {
      assert(a[i][j] == (j<5 ? 3 : 70));
    }
  }
  a.column_resize(4);
  assert(a.row_size() == 2);
  assert(a.column_size() == 4);
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(a[i][j] == 3);
    }
  }
}

void TestArithmetrics_Plus() {
  Matrix<int> a(3, 4, 10), b(3, 4, 20);
  a += b;
  assert(a.row_size() == 3);
  assert(a.column_size() == 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(a[i][j] == 30);
    }
  }
  Matrix<int> d = a + b;
  assert(d.row_size() == 3);
  assert(d.column_size() == 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(d[i][j] == 50);
    }
  }
  Matrix<int> e = Matrix<int>(3, 4, 40) + b;
  assert(e.row_size() == 3);
  assert(e.column_size() == 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(e[i][j] == 60);
    }
  }
  Matrix<int> f = b + Matrix<int>(3, 4, 40);
  assert(f.row_size() == 3);
  assert(f.column_size() == 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(f[i][j] == 60);
    }
  }
}

void TestArithmetrics_Minus() {
  Matrix<int> a(3, 4, 10), b(3, 4, 20), c(a);
  c -= b;
  assert(c.row_size() == 3);
  assert(c.column_size() == 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(c[i][j] == -10);
    }
  }
  Matrix<int> d = a - b;
  assert(d.row_size() == 3);
  assert(d.column_size() == 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(d[i][j] == -10);
    }
  }
  Matrix<int> e = Matrix<int>(3, 4, 40) - b;
  assert(e.row_size() == 3);
  assert(e.column_size() == 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(e[i][j] == 20);
    }
  }
  Matrix<int> f = b - Matrix<int>(3, 4, 40);
  assert(f.row_size() == 3);
  assert(f.column_size() == 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(f[i][j] == -20);
    }
  }
}

void TestArithmetrics_MultiplyByValue() {
  Matrix<int> a(3, 4, 10), b(3, 4, 20), c(a);
  c *= 3;
  assert(c.row_size() == 3);
  assert(c.column_size() == 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(c[i][j] == 30);
    }
  }
  Matrix<int> d = a * 4;
  assert(d.row_size() == 3);
  assert(d.column_size() == 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(d[i][j] == 40);
    }
  }
  Matrix<int> e = 4 * a;
  assert(e.row_size() == 3);
  assert(e.column_size() == 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(e[i][j] == 40);
    }
  }
  Matrix<int> f = 4 * Matrix<int>(3, 4, 10);
  assert(f.row_size() == 3);
  assert(f.column_size() == 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(f[i][j] == 40);
    }
  }
  Matrix<int> g = Matrix<int>(3, 4, 10) * 4;
  assert(g.row_size() == 3);
  assert(g.column_size() == 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(g[i][j] == 40);
    }
  }
}

void TestArithmetrics_DivideByValue() {
  Matrix<int> a(3, 4, 10), b(3, 4, 20), c(a);
  c /= 2;
  assert(c.row_size() == 3);
  assert(c.column_size() == 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(c[i][j] == 5);
    }
  }
  Matrix<int> d = a / 2;
  assert(d.row_size() == 3);
  assert(d.column_size() == 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(d[i][j] == 5);
    }
  }
  Matrix<int> e = Matrix<int>(3, 4, 40) / 4;
  assert(e.row_size() == 3);
  assert(e.column_size() == 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(e[i][j] == 10);
    }
  }
}

void TestArithmetrics_ModuleByValue() {
  Matrix<int> a(3, 4, 10), b(3, 4, 20), c(a);
  c %= 3;
  assert(c.row_size() == 3);
  assert(c.column_size() == 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(c[i][j] == 1);
    }
  }
  Matrix<int> d = a % 4;
  assert(d.row_size() == 3);
  assert(d.column_size() == 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(d[i][j] == 2);
    }
  }
  Matrix<int> e = Matrix<int>(3, 4, 10) % 7;
  assert(e.row_size() == 3);
  assert(e.column_size() == 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      assert(e[i][j] == 3);
    }
  }
}

void TestArithmetrics_MultiplyByMatrix() {
  Matrix<int> a(3, 4, 10), b(4, 5, 20), c(a);
  c *= b;
  assert(c.row_size() == 3);
  assert(c.column_size() == 5);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 5; ++j) {
      assert(c[i][j] == 800);
    }
  }
  Matrix<int> d = a * b;
  assert(d.row_size() == 3);
  assert(d.column_size() == 5);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 5; ++j) {
      assert(d[i][j] == 800);
    }
  }
  Matrix<int> e = a * Matrix<int>(4, 5, 20);
  assert(e.row_size() == 3);
  assert(e.column_size() == 5);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 5; ++j) {
      assert(e[i][j] == 800);
    }
  }
  Matrix<int> f = Matrix<int>(3, 4, 10) * b;
  assert(f.row_size() == 3);
  assert(f.column_size() == 5);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 5; ++j) {
      assert(f[i][j] == 800);
    }
  }
}

void TestConcurrentSpeed() {
  using clock = chrono::system_clock;
  clock::time_point start, stop;
  chrono::microseconds microsecs;
  size_t num_row = 1000, num_column = 100000;
  vector<size_t> dummy_results(num_row);

  // single thread
  start = clock::now();
  for (size_t i = 0; i < num_row; ++i) {
    for (size_t j = 0; j < num_column; ++j) {
      dummy_results[i] += i + j;
    }
  }
  stop = clock::now();
  microsecs = stop - start;
  cout << microsecs.count() << " microseconds"
       << " (single-threaded double-for-loop)" << endl;

  // multi-threaded
  start = clock::now();
  vector<future<size_t>> futures;
  for (size_t i = 0; i < num_row; ++i) {
    futures.push_back(std::async(std::launch::async, [i, num_column]() {
      size_t result = 0;
      for (size_t j = 0; j < num_column; ++j) {
        result += i + j;
      }
      return result;
    }));
  }
  for (size_t i = 0; i < num_row; ++i) {
    dummy_results[i] = futures[i].get();
  }
  stop = clock::now();
  microsecs = stop - start;
  cout << microsecs.count() << " microseconds"
       << " (multi-threaded double-for-loop)" << endl;
}

void TestElementaryOperations() {
  Matrix<int> a(3, 4);
  for (int i = 0; i < 3; ++i) {
    std::fill(a.row_begin(i), a.row_end(i), i+1);
  }
  a.elementary_row_switch(0, 1);
  for (int j = 0; j < 4; ++j) {
    assert(a[0][j] == 2);
    assert(a[1][j] == 1);
    assert(a[2][j] == 3);
  }
  // a.elementary_row_multiply(1, 0);  // exception thrown
  a.elementary_row_multiply(1, 4);
  for (int j = 0; j < 4; ++j) {
    assert(a[0][j] == 2);
    assert(a[1][j] == 4);
    assert(a[2][j] == 3);
  }
  // a.elementary_row_add(1, 1, 3);  // expection thrown
  a.elementary_row_add(0, 2, 4);
  for (int j = 0; j < 4; ++j) {
    assert(a[0][j] == 14);
    assert(a[1][j] == 4);
    assert(a[2][j] == 3);
  }

  Matrix<int> b(3, 4);
  for (int j = 0; j < 4; ++j) {
    std::fill(b.column_begin(j), b.column_end(j), j+1);
  }
  b.elementary_column_switch(1, 2);
  for (int i = 0; i < 3; ++i) {
    assert(b[i][0] == 1);
    assert(b[i][1] == 3);
    assert(b[i][2] == 2);
    assert(b[i][3] == 4);
  }
  // b.elementary_column_multiply(0, 0);  // exception thrown
  b.elementary_column_multiply(0, 5);
  for (int i = 0; i < 3; ++i) {
    assert(b[i][0] == 5);
    assert(b[i][1] == 3);
    assert(b[i][2] == 2);
    assert(b[i][3] == 4);
  }
  // b.elementary_column_add(1, 1, 6);  // exception thrown
  b.elementary_column_add(1, 2, 6);
  for (int i = 0; i < 3; ++i) {
    assert(b[i][0] == 5);
    assert(b[i][1] == 15);
    assert(b[i][2] == 2);
    assert(b[i][3] == 4);
  }
}

void TestComparasions() {
  Matrix<int> a = {
    {1, 2},
    {3, 4},
  }, b = {
    {1, 2},
    {3, 3},
  }, c(a);
  assert(a != b);
  assert(a == c);
  assert(a.equal_to(
      b, [](const int& lhs, const int& rhs) {
        return abs(lhs - rhs) <= 1;
      }));
}

void TestArithmetrics_Temporaries() {
  cout << "prepare a, b, c, scaler, expected" << endl;
  Matrix<int> a = {
    {1, 2},
    {3, 4},
  }, b = {
    {2, 3},
    {4, 5},
  }, c = {
    {1, 1},
    {1, 1},
  }, expected = {
    {2, 2},
    {2, 2},
  };
  int scaler = 3;
  cout << "Matrix<int> d = std::move(c) * scaler + a - b;" << endl;
  Matrix<int> d = std::move(c) * scaler + a - b;
  assert(d.equal_to(expected));
  cout << "done." << endl;
}

void TestPrint() {
  Matrix<int> a = {
    {1, 2},
    {3, 4},
    {5, 6},
  }, b = {
    {7, 8, 9},
    {10, 11, 12},
    {13, 14, 15},
  };
  matrix::Print(a, &cout);
  matrix::PrintAugmented(a, b, &cout);
}

int main(int argc, char* argv[]) {
  TestConstructors();
  TestObservers();
  TestAssignments();
  TestIterators();
  TestManipulators();
  TestArithmetrics_Plus();
  TestArithmetrics_Minus();
  TestArithmetrics_MultiplyByValue();
  TestArithmetrics_DivideByValue();
  TestArithmetrics_ModuleByValue();
  TestArithmetrics_MultiplyByMatrix();
  TestConcurrentSpeed();
  TestElementaryOperations();
  TestComparasions();
  TestArithmetrics_Temporaries();
  TestPrint();
  return 0;
}
