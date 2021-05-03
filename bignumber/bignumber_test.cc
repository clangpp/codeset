#include "bignumber/bignumber.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "gtest/gtest.h"

template <typename ForwardIterator>
void to_digits(ForwardIterator first, ForwardIterator last) {
  for (; first != last; ++first) {
    if ('0' <= *first && *first <= '9') {
      *first -= '0';
    } else if ('a' <= *first && *first <= 'z') {
      *first = *first - 'a' + 10;
    } else if ('A' <= *first && *first <= 'Z') {
      *first = *first - 'A' + 10;
    }
  }
}

template <typename ForwardIterator>
void to_characters(ForwardIterator first, ForwardIterator last) {
  for (; first != last; ++first) {
    if (0 <= *first && *first < 10) {
      *first += '0';
    } else if (10 <= *first && *first < 10 + 26) {
      *first = *first - 10 + 'A';
    }
  }
}

void TestMultiplies(int num1, int num2) {
  std::string str_num1 = std::to_string(num1);
  std::string str_num2 = std::to_string(num2);
  int result = num1 * num2;
  std::string str_result_check = std::to_string(result);
  std::string str_result(str_num1.size() + str_num2.size(), '0');
  to_digits(str_num1.begin(), str_num1.end());
  to_digits(str_num2.begin(), str_num2.end());
  to_digits(str_result.begin(), str_result.end());
  std::string::reverse_iterator iter = bignumber::multiplies(
      str_num1.rbegin(), str_num1.rend(), str_num2.rbegin(), str_num2.rend(),
      10, str_result.rbegin());
  str_result.erase(str_result.begin(), iter.base());
  to_characters(str_result.begin(), str_result.end());
  EXPECT_EQ(str_result, str_result_check)
      << "num1=" << num1 << " num2=" << num2;
}

TEST(BigNumberTest, Multiplies) {
  TestMultiplies(0, 0);
  TestMultiplies(0, 1);
  TestMultiplies(1, 0);
  TestMultiplies(1234, 42);
  TestMultiplies(10054, 38722);
}
