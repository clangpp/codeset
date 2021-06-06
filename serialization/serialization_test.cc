#include "serialization.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iterator>
#include <sstream>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace serialization {
namespace {

using ::testing::Pair;

TEST(PairSerializationTest, EndToEnd) {
  std::pair<int, int> val;
  std::stringstream ss, so;
  ss << " ( " << 2 << " " << 5 << " ) ";
  ss >> val;
  EXPECT_THAT(val, Pair(2, 5));

  ss << val;
  std::istream_iterator<std::pair<int, int>> is_iter(ss), is_end;
  std::ostream_iterator<std::pair<int, int>> os_iter(so);
  std::copy(is_iter, is_end, os_iter);
  so >> val;
  EXPECT_THAT(val, Pair(2, 5));
}

TEST(TriadSerializationTest, EndToEnd) {
  Triad<int, int, int> val;
  std::stringstream ss, so;
  ss << " ( " << 2 << " " << 5 << " " << 8 << " ) ";
  ss >> val;
  EXPECT_THAT(val.first, 2);
  EXPECT_THAT(val.second, 5);
  EXPECT_THAT(val.third, 8);

  val.third = 9;
  ss << val;
  std::istream_iterator<Triad<int, int, int>> is_iter(ss), is_end;
  std::ostream_iterator<Triad<int, int, int>> os_iter(so, "\n");
  std::copy(is_iter, is_end, os_iter);
  so >> val;
  EXPECT_THAT(val.first, 2);
  EXPECT_THAT(val.second, 5);
  EXPECT_THAT(val.third, 9);
}

TEST(CellSerializationTest, EndToEnd) {
  namespace sm = sparsematrix;
  sm::Cell<int> val;
  std::stringstream ss, so;
  ss << " ( " << 2 << " " << 5 << " " << 8 << " ) ";
  ss >> val;
  EXPECT_THAT(val.row, 2);
  EXPECT_THAT(val.column, 5);
  EXPECT_THAT(val.value, 8);

  ss << val;
  std::istream_iterator<sm::Cell<int>> is_iter(ss), is_end;
  std::ostream_iterator<sm::Cell<int>> os_iter(so, "\n");
  std::copy(is_iter, is_end, os_iter);
  so >> val;
  EXPECT_THAT(val.row, 2);
  EXPECT_THAT(val.column, 5);
  EXPECT_THAT(val.value, 8);
}

TEST(DimensionSerializationTest, EndToEnd) {
  namespace sm = sparsematrix;
  sm::Dimension val;
  std::stringstream ss, so;
  ss << " [ " << 3 << " " << 6 << " ] ";
  ss >> val;
  EXPECT_THAT(val.row, 3);
  EXPECT_THAT(val.column, 6);

  ss << val;
  std::istream_iterator<sm::Dimension> is_iter(ss), is_end;
  std::ostream_iterator<sm::Dimension> os_iter(so, "\n");
  std::copy(is_iter, is_end, os_iter);
  so >> val;
  EXPECT_THAT(val.row, 3);
  EXPECT_THAT(val.column, 6);
}

TEST(PairCellSerializationTest, EndToEnd) {
  namespace sm = sparsematrix;
  sm::Cell<std::pair<int, int>> val;
  std::stringstream ss, so;
  ss << " ( 2 5 ( 7 8) ) ";
  ss >> val;
  EXPECT_THAT(val.row, 2);
  EXPECT_THAT(val.column, 5);
  EXPECT_THAT(val.value, Pair(7, 8));

  ss << val;
  std::istream_iterator<sm::Cell<std::pair<int, int>>> is_iter(ss), is_end;
  std::ostream_iterator<sm::Cell<std::pair<int, int>>> os_iter(so, "\n");
  std::copy(is_iter, is_end, os_iter);
  so >> val;
  EXPECT_THAT(val.row, 2);
  EXPECT_THAT(val.column, 5);
  EXPECT_THAT(val.value, Pair(7, 8));
}

TEST(CrosslistSerializationTest, EndToEnd) {
  using namespace serialization;
  CrossList<int> c;
  std::stringstream ss, so;
  ss << " ( 0 0 1 )  ( 0 2 2 ) \n";
  ss << " ( 1 1 3 )  ( 1 3 4 ) \n";
  ss << " ( 2 1 5 )  ( 2 2 6 ) \n";
  ss << " [ 3 4 ] ";
  ss >> c;
  EXPECT_THAT(c.row_count(), 3);
  EXPECT_THAT(c.column_count(), 4);
  EXPECT_THAT(c.size(), 6);
  EXPECT_THAT(c(0, 0), 1);
  EXPECT_THAT(c(0, 2), 2);
  EXPECT_THAT(c(1, 1), 3);
  EXPECT_THAT(c(1, 3), 4);
  EXPECT_THAT(c(2, 1), 5);
  EXPECT_THAT(c(2, 2), 6);

  so << c;
  so >> c;

  EXPECT_THAT(c.row_count(), 3);
  EXPECT_THAT(c.column_count(), 4);
  EXPECT_THAT(c.size(), 6);
  EXPECT_THAT(c(0, 0), 1);
  EXPECT_THAT(c(0, 2), 2);
  EXPECT_THAT(c(1, 1), 3);
  EXPECT_THAT(c(1, 3), 4);
  EXPECT_THAT(c(2, 1), 5);
  EXPECT_THAT(c(2, 2), 6);
}

}  // namespace
}  // namespace serialization
