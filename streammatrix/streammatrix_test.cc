#include "streammatrix.h"

#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::ElementsAreArray;

const int M = 10, N = 8;
int MAT[M][N] = {
    {10, 0, 0, 20, 30, 40, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {50, 0, 60, 70, 0, 0, 80, 0},
    {0, 90, 100, 0, 110, 0, 120, 130},
    {150, 160, 170, 180, 190, 200, 210, 220},
    {0, 0, 230, 0, 240, 250, 260, 0},
    {0, 270, 280, 290, 0, 0, 0, 0},
    {300, 0, 310, 0, 320, 0, 330, 0},
    {340, 0, 350, 0, 360, 0, 370, 0},
    {0, 0, 0, 380, 390, 0, 0, 400},
};

class StreamMatrixTest : public testing::Test {
 protected:
  StreamMatrixTest() {
    mat_file_ = testing::TempDir() + "/matrix.mat";
    CrossList<int> mat(M, N);
    for (int i = 0; i < M; ++i) {
      for (int j = 0; j < N; ++j) {
        if (MAT[i][j] != 0) {
          mat.insert(i, j, MAT[i][j]);
        }
      }
    }
    std::ofstream fout(mat_file_.c_str());
    fout << mat;
    fout.close();
  }

  std::string mat_file_;
};

TEST_F(StreamMatrixTest, InputStream) {
  std::ifstream fin(mat_file_.c_str());
  StreamMatrix<int> smat(fin);
  EXPECT_EQ(smat.row_count(), M);
  EXPECT_EQ(smat.column_count(), N);
  EXPECT_EQ(smat.size(), 39);
  EXPECT_FALSE(smat.empty());
}

TEST_F(StreamMatrixTest, Iterator) {
  std::ifstream fin(mat_file_.c_str());
  StreamMatrix<int> smat(fin);
  std::vector<int> values;
  for (auto iter = smat.begin(), end = smat.end(); iter != end; ++iter) {
    values.push_back(*iter);
  }
  std::vector<int> values_check;
  for (int i = 0; i < M; ++i) {
    for (int j = 0; j < N; ++j) {
      if (MAT[i][j] != 0) {
        values_check.push_back(MAT[i][j]);
      }
    }
  }
  EXPECT_THAT(values, ElementsAreArray(values_check));
}

TEST_F(StreamMatrixTest, RowIterator) {
  std::ifstream fin(mat_file_.c_str());
  StreamMatrix<int> smat(fin);
  for (int i = 0; i < M; ++i) {
    std::vector<int> values;
    for (auto iter = smat.row_begin(i), end = smat.row_end(i); iter != end;
         ++iter) {
      values.push_back(*iter);
      EXPECT_EQ(iter.row(), i);
    }

    std::vector<int> values_check;
    for (int j = 0; j < N; ++j) {
      if (MAT[i][j] != 0) {
        values_check.push_back(MAT[i][j]);
      }
    }
    EXPECT_THAT(values, ElementsAreArray(values_check)) << "row " << i;
  }
}
