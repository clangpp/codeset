#include "matrix.h"

#include <cassert>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <future>
#include <iostream>

#include "gtest/gtest.h"

TEST(ConstructorTest, InitialValue) {
  matrix::Matrix<int> a(2, 4, 3);
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(a(i, j), 3);
    }
  }
}

TEST(ConstructorTest, DefaultValue) {
  matrix::Matrix<int> b(3, 5);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 5; ++j) {
      EXPECT_EQ(b(i, j), 0);
    }
  }
}

TEST(ConstructorTest, Default) {
  matrix::Matrix<int> c;
  EXPECT_EQ(c.row_size(), 0);
  EXPECT_EQ(c.column_size(), 0);
}

TEST(ConstructorTest, Copy) {
  matrix::Matrix<int> a(2, 4, 3);
  matrix::Matrix<int> d(a);
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(d(i, j), 3);
    }
  }
}

TEST(ConstructorTest, Move) {
  matrix::Matrix<int> e(matrix::Matrix<int>(2, 4, 3));
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(e(i, j), 3);
    }
  }
  matrix::Matrix<int> f(std::move(e));
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(f(i, j), 3);
    }
  }
}

TEST(ConstructorTest, InitializerList) {
  matrix::Matrix<int> g = {
      {0, 1, 2},
      {1, 2, 3},
  };
  EXPECT_EQ(g.row_size(), 2);
  EXPECT_EQ(g.column_size(), 3);
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 3; ++j) {
      EXPECT_EQ(g(i, j), i + j);
    }
  }
}

TEST(ObserverTest, ItWorks) {
  matrix::Matrix<int> a(2, 4, 3), b;
  EXPECT_EQ(a.row_size(), 2);
  EXPECT_EQ(a.column_size(), 4);
  EXPECT_EQ(a.size(), 2 * 4);
  EXPECT_EQ(a.dimension().first, 2);
  EXPECT_EQ(a.dimension().second, 4);
  EXPECT_FALSE(a.empty());
  EXPECT_TRUE(b.empty());
  const matrix::Matrix<int>& ra(a);
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(a(i, j), 3);
      EXPECT_EQ(ra(i, j), 3);
      EXPECT_EQ(a[i][j], 3);
      EXPECT_EQ(ra[i][j], 3);
      a(i, j) = 4;
      EXPECT_EQ(a(i, j), 4);
      EXPECT_EQ(a[i][j], 4);
      a[i][j] = 5;
      EXPECT_EQ(a(i, j), 5);
      EXPECT_EQ(a[i][j], 5);
    }
  }
}

TEST(Assignments, ItWorks) {
  matrix::Matrix<int> a(2, 4, 3), b, c;
  b = a;
  EXPECT_EQ(a.row_size(), b.row_size());
  EXPECT_EQ(a.column_size(), b.column_size());
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(a[i][j], b[i][j]);
    }
  }
  b = b;
  EXPECT_EQ(a.row_size(), b.row_size());
  EXPECT_EQ(a.column_size(), b.column_size());
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(a[i][j], b[i][j]);
    }
  }
  c = std::move(b);
  EXPECT_EQ(a.row_size(), c.row_size());
  EXPECT_EQ(a.column_size(), c.column_size());
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(a[i][j], c[i][j]);
    }
  }
  c = std::move(c);
  EXPECT_EQ(a.row_size(), c.row_size());
  EXPECT_EQ(a.column_size(), c.column_size());
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(a[i][j], c[i][j]);
    }
  }
}

TEST(IteratorTest, ItWorks) {
  matrix::Matrix<int> a(2, 3);
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 3; ++j) {
      a[i][j] = i + j;
    }
  }
  for (int row = 0; row < 2; ++row) {
    int column = 0;
    for (auto row_iter = a.row_begin(row); row_iter != a.row_end(row);
         ++row_iter, ++column) {
      EXPECT_EQ(*row_iter, row + column);
    }
  }
  for (int column = 0; column < 2; ++column) {
    int row = 0;
    for (auto column_iter = a.column_begin(column);
         column_iter != a.column_end(column); ++column_iter, ++row) {
      EXPECT_EQ(*column_iter, row + column);
    }
  }
  const matrix::Matrix<int>& cra(a);
  for (int row = 0; row < 2; ++row) {
    int column = 0;
    for (auto row_iter = cra.row_begin(row); row_iter != cra.row_end(row);
         ++row_iter, ++column) {
      EXPECT_EQ(*row_iter, row + column);
    }
  }
  for (int column = 0; column < 2; ++column) {
    int row = 0;
    for (auto column_iter = cra.column_begin(column);
         column_iter != cra.column_end(column); ++column_iter, ++row) {
      EXPECT_EQ(*column_iter, row + column);
    }
  }
}

TEST(Manipulators, ItWorks) {
  matrix::Matrix<int> a(3, 5, 3);
  a.row_resize(4, 30);
  EXPECT_EQ(a.row_size(), 4);
  EXPECT_EQ(a.column_size(), 5);
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 5; ++j) {
      EXPECT_EQ(a[i][j], (i < 3 ? 3 : 30));
    }
  }
  a.row_resize(2);
  EXPECT_EQ(a.row_size(), 2);
  EXPECT_EQ(a.column_size(), 5);
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 5; ++j) {
      EXPECT_EQ(a[i][j], 3);
    }
  }
  a.column_resize(7, 70);
  EXPECT_EQ(a.row_size(), 2);
  EXPECT_EQ(a.column_size(), 7);
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 7; ++j) {
      EXPECT_EQ(a[i][j], (j < 5 ? 3 : 70));
    }
  }
  a.column_resize(4);
  EXPECT_EQ(a.row_size(), 2);
  EXPECT_EQ(a.column_size(), 4);
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(a[i][j], 3);
    }
  }
}

TEST(ArithmetricsTest, Plus) {
  matrix::Matrix<int> a(3, 4, 10), b(3, 4, 20);
  a += b;
  EXPECT_EQ(a.row_size(), 3);
  EXPECT_EQ(a.column_size(), 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(a[i][j], 30);
    }
  }
  matrix::Matrix<int> d = a + b;
  EXPECT_EQ(d.row_size(), 3);
  EXPECT_EQ(d.column_size(), 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(d[i][j], 50);
    }
  }
  matrix::Matrix<int> e = matrix::Matrix<int>(3, 4, 40) + b;
  EXPECT_EQ(e.row_size(), 3);
  EXPECT_EQ(e.column_size(), 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(e[i][j], 60);
    }
  }
  matrix::Matrix<int> f = b + matrix::Matrix<int>(3, 4, 40);
  EXPECT_EQ(f.row_size(), 3);
  EXPECT_EQ(f.column_size(), 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(f[i][j], 60);
    }
  }
}

TEST(ArithmetricsTest, Minus) {
  matrix::Matrix<int> a(3, 4, 10), b(3, 4, 20), c(a);
  c -= b;
  EXPECT_EQ(c.row_size(), 3);
  EXPECT_EQ(c.column_size(), 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(c[i][j], -10);
    }
  }
  matrix::Matrix<int> d = a - b;
  EXPECT_EQ(d.row_size(), 3);
  EXPECT_EQ(d.column_size(), 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(d[i][j], -10);
    }
  }
  matrix::Matrix<int> e = matrix::Matrix<int>(3, 4, 40) - b;
  EXPECT_EQ(e.row_size(), 3);
  EXPECT_EQ(e.column_size(), 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(e[i][j], 20);
    }
  }
  matrix::Matrix<int> f = b - matrix::Matrix<int>(3, 4, 40);
  EXPECT_EQ(f.row_size(), 3);
  EXPECT_EQ(f.column_size(), 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(f[i][j], -20);
    }
  }
}

TEST(ArithmetricsTest, MultiplyByValue) {
  matrix::Matrix<int> a(3, 4, 10), b(3, 4, 20), c(a);
  c *= 3;
  EXPECT_EQ(c.row_size(), 3);
  EXPECT_EQ(c.column_size(), 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(c[i][j], 30);
    }
  }
  matrix::Matrix<int> d = a * 4;
  EXPECT_EQ(d.row_size(), 3);
  EXPECT_EQ(d.column_size(), 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(d[i][j], 40);
    }
  }
  matrix::Matrix<int> e = 4 * a;
  EXPECT_EQ(e.row_size(), 3);
  EXPECT_EQ(e.column_size(), 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(e[i][j], 40);
    }
  }
  matrix::Matrix<int> f = 4 * matrix::Matrix<int>(3, 4, 10);
  EXPECT_EQ(f.row_size(), 3);
  EXPECT_EQ(f.column_size(), 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(f[i][j], 40);
    }
  }
  matrix::Matrix<int> g = matrix::Matrix<int>(3, 4, 10) * 4;
  EXPECT_EQ(g.row_size(), 3);
  EXPECT_EQ(g.column_size(), 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(g[i][j], 40);
    }
  }
}

TEST(ArithmetricsTest, DivideByValue) {
  matrix::Matrix<int> a(3, 4, 10), b(3, 4, 20), c(a);
  c /= 2;
  EXPECT_EQ(c.row_size(), 3);
  EXPECT_EQ(c.column_size(), 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(c[i][j], 5);
    }
  }
  matrix::Matrix<int> d = a / 2;
  EXPECT_EQ(d.row_size(), 3);
  EXPECT_EQ(d.column_size(), 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(d[i][j], 5);
    }
  }
  matrix::Matrix<int> e = matrix::Matrix<int>(3, 4, 40) / 4;
  EXPECT_EQ(e.row_size(), 3);
  EXPECT_EQ(e.column_size(), 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(e[i][j], 10);
    }
  }
}

TEST(ArithmetricsTest, ModuleByValue) {
  matrix::Matrix<int> a(3, 4, 10), b(3, 4, 20), c(a);
  c %= 3;
  EXPECT_EQ(c.row_size(), 3);
  EXPECT_EQ(c.column_size(), 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(c[i][j], 1);
    }
  }
  matrix::Matrix<int> d = a % 4;
  EXPECT_EQ(d.row_size(), 3);
  EXPECT_EQ(d.column_size(), 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(d[i][j], 2);
    }
  }
  matrix::Matrix<int> e = matrix::Matrix<int>(3, 4, 10) % 7;
  EXPECT_EQ(e.row_size(), 3);
  EXPECT_EQ(e.column_size(), 4);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_EQ(e[i][j], 3);
    }
  }
}

TEST(ArithmetricsTest, MultiplyByMatrix) {
  matrix::Matrix<int> a(3, 4, 10), b(4, 5, 20), c(a);
  c *= b;
  EXPECT_EQ(c.row_size(), 3);
  EXPECT_EQ(c.column_size(), 5);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 5; ++j) {
      EXPECT_EQ(c[i][j], 800);
    }
  }
  matrix::Matrix<int> d = a * b;
  EXPECT_EQ(d.row_size(), 3);
  EXPECT_EQ(d.column_size(), 5);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 5; ++j) {
      EXPECT_EQ(d[i][j], 800);
    }
  }
  matrix::Matrix<int> e = a * matrix::Matrix<int>(4, 5, 20);
  EXPECT_EQ(e.row_size(), 3);
  EXPECT_EQ(e.column_size(), 5);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 5; ++j) {
      EXPECT_EQ(e[i][j], 800);
    }
  }
  matrix::Matrix<int> f = matrix::Matrix<int>(3, 4, 10) * b;
  EXPECT_EQ(f.row_size(), 3);
  EXPECT_EQ(f.column_size(), 5);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 5; ++j) {
      EXPECT_EQ(f[i][j], 800);
    }
  }
}

TEST(ConcurrentSpeedTest, ItWorks) {
  typedef std::chrono::system_clock clock;
  clock::time_point start, stop;
  std::chrono::microseconds microsecs;
  size_t num_row = 1000, num_column = 100000;
  std::vector<size_t> dummy_results(num_row);

  // single thread
  start = clock::now();
  for (size_t i = 0; i < num_row; ++i) {
    for (size_t j = 0; j < num_column; ++j) {
      dummy_results[i] += i + j;
    }
  }
  stop = clock::now();
  microsecs = stop - start;
  SUCCEED() << microsecs.count() << " microseconds"
            << " (single-threaded double-for-loop)";

  // multi-threaded
  start = clock::now();
  std::vector<std::future<size_t>> futures;
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
  SUCCEED() << microsecs.count() << " microseconds"
            << " (multi-threaded double-for-loop)";
}

TEST(ElementaryOperationsTest, ItWorks) {
  matrix::Matrix<int> a(3, 4);
  for (int i = 0; i < 3; ++i) {
    std::fill(a.row_begin(i), a.row_end(i), i + 1);
  }
  a.elementary_row_switch(0, 1);
  for (int j = 0; j < 4; ++j) {
    EXPECT_EQ(a[0][j], 2);
    EXPECT_EQ(a[1][j], 1);
    EXPECT_EQ(a[2][j], 3);
  }
  // a.elementary_row_multiply(1, 0);  // exception thrown
  a.elementary_row_multiply(1, 4);
  for (int j = 0; j < 4; ++j) {
    EXPECT_EQ(a[0][j], 2);
    EXPECT_EQ(a[1][j], 4);
    EXPECT_EQ(a[2][j], 3);
  }
  // a.elementary_row_add(1, 1, 3);  // expection thrown
  a.elementary_row_add(0, 2, 4);
  for (int j = 0; j < 4; ++j) {
    EXPECT_EQ(a[0][j], 14);
    EXPECT_EQ(a[1][j], 4);
    EXPECT_EQ(a[2][j], 3);
  }

  matrix::Matrix<int> b(3, 4);
  for (int j = 0; j < 4; ++j) {
    std::fill(b.column_begin(j), b.column_end(j), j + 1);
  }
  b.elementary_column_switch(1, 2);
  for (int i = 0; i < 3; ++i) {
    EXPECT_EQ(b[i][0], 1);
    EXPECT_EQ(b[i][1], 3);
    EXPECT_EQ(b[i][2], 2);
    EXPECT_EQ(b[i][3], 4);
  }
  // b.elementary_column_multiply(0, 0);  // exception thrown
  b.elementary_column_multiply(0, 5);
  for (int i = 0; i < 3; ++i) {
    EXPECT_EQ(b[i][0], 5);
    EXPECT_EQ(b[i][1], 3);
    EXPECT_EQ(b[i][2], 2);
    EXPECT_EQ(b[i][3], 4);
  }
  // b.elementary_column_add(1, 1, 6);  // exception thrown
  b.elementary_column_add(1, 2, 6);
  for (int i = 0; i < 3; ++i) {
    EXPECT_EQ(b[i][0], 5);
    EXPECT_EQ(b[i][1], 15);
    EXPECT_EQ(b[i][2], 2);
    EXPECT_EQ(b[i][3], 4);
  }
}

TEST(ComparasionsTest, ItWorks) {
  matrix::Matrix<int> a =
                          {
                              {1, 2},
                              {3, 4},
                          },
                      b =
                          {
                              {1, 2},
                              {3, 3},
                          },
                      c(a);
  EXPECT_NE(a, b);
  EXPECT_EQ(a, c);
  EXPECT_TRUE(a.equal_to(
      b, [](const int& lhs, const int& rhs) { return abs(lhs - rhs) <= 1; }));
}

TEST(ArithmetricsTest, Temporaries) {
  matrix::Matrix<int> a =
                          {
                              {1, 2},
                              {3, 4},
                          },
                      b =
                          {
                              {2, 3},
                              {4, 5},
                          },
                      c =
                          {
                              {1, 1},
                              {1, 1},
                          },
                      expected = {
                          {2, 2},
                          {2, 2},
                      };
  int scaler = 3;
  matrix::Matrix<int> d = std::move(c) * scaler + a - b;
  EXPECT_TRUE(d.equal_to(expected));
}

TEST(PrintTest, Print) {
  matrix::Matrix<int> a = {
      {1, 2},
      {3, 4},
      {5, 6},
  };
  std::stringstream ss;
  matrix::Print(a, &ss);
  EXPECT_EQ(ss.str(), R"(1 2 
3 4 
5 6 
)");
}

TEST(PrintTest, PrintAugmented) {
  matrix::Matrix<int> a = {
      {1, 2},
      {3, 4},
      {5, 6},
  };
  matrix::Matrix<int> b = {
      {7, 8, 9},
      {10, 11, 12},
      {13, 14, 15},
  };
  std::stringstream ss;
  matrix::PrintAugmented(a, b, &ss);
  EXPECT_EQ(ss.str(), R"(1 2 | 7 8 9 
3 4 | 10 11 12 
5 6 | 13 14 15 
)");
}
