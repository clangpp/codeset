#include "facility.h"

#include <cstdlib>
#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace facility {
namespace {

using ::testing::ElementsAreArray;

template <typename OStream, typename CharT>
OStream& write(OStream& os, const std::basic_string<CharT>& str) {
  const std::size_t W = sizeof(CharT);
  for (size_t i = 0; i < str.size(); ++i) {
    for (size_t j = 0; j < W; ++j) {
      os << (unsigned)(unsigned char)(str[i] >> (W - 1 - j) * 8)
         << (W - 1 == j ? "" : ",");
    }
    os << (str.size() - 1 == i ? "" : " ");
  }
  return os;
}

TEST(ToTest, ItWorks) { EXPECT_EQ(to<int>("35"), 35); }

TEST(TrimTest, ItWorks) {
  std::string str("\tabc def\tghi\r\n \v");
  std::string str_test;

  str_test = str;
  EXPECT_EQ(trim_left(str_test), "abc def\tghi\r\n \v");

  str_test = str;
  EXPECT_EQ(trim_right(str_test), "\tabc def\tghi");

  str_test = str;
  EXPECT_EQ(trim(str_test), "abc def\tghi");
}

TEST(ArrayLengthTest, ItWorks) {
  int a[3], b[1], c[] = {1, 2};
  EXPECT_EQ(array_length(a), 3);
  EXPECT_EQ(array_length(b), 1);
  EXPECT_EQ(array_length(c), 2);
}

// TODO(clangpp): Get this work.
TEST(CopyStringTest, DISABLED_ItWorks) {
  std::string str0("SOME_THING_WRONG_DEF"), str1, strb(str0);
  std::swap(strb[0], strb[1]);
  std::swap(strb[5], strb[6]);
  std::wstring wstr0, wstr1;
  wstr0.push_back((unsigned char)str0[0] | (str0[1] << 8));
  wstr0.append(str0.data() + 2, str0.data() + 5);
  wstr0.push_back((unsigned char)str0[5] | (str0[6] << 8));
  wstr0.append(str0.data() + 7, str0.data() + 10);

  EXPECT_EQ(copy_little_endian(wstr0, 2, str1), str0);
  EXPECT_EQ(copy_little_endian(str0, 2, wstr1), wstr0);
  EXPECT_EQ(copy_big_endian(wstr0, 2, str1), strb);

  copy_big_endian(strb, 2, wstr1);
  EXPECT_EQ(wstr1[4] & 0xff, (wstr0[4] >> 8) & 0xff);
  EXPECT_EQ((wstr1[5] >> 8) & 0xff, wstr0[4] & 0xff);
  EXPECT_EQ((wstr1[5] & 0xff), wstr0[5] & 0xff);
  wstr1[4] = wstr0[4];
  wstr1[5] = wstr0[5];
  EXPECT_EQ(wstr1, wstr0);
}

TEST(SplitTest, ItWorks) {
  EXPECT_THAT(split("abc def  ghi "),
              ElementsAreArray({"abc", "def", "", "ghi", ""}));
}

}  // namespace
}  // namespace facility
