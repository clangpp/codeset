#include "matrix.h"

#include <cassert>
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
  c = std::move(b);
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
  return 0;
}
