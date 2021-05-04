#include "bm/bm.h"

#include <iostream>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::ElementsAreArray;
using ::testing::Pair;

TEST(BMIteratorTest, ItWorks) {
  std::string s1("aa"), str("aabcdefghiaac");
  bm::Subsequence<char> subseq(s1.begin(), s1.end());
  std::vector<std::pair<std::size_t, std::string>> actual;
  for (bm::SubsequenceIterator<std::string::const_iterator>
           bm_iter(str.begin(), str.end(), subseq),
       bm_end;
       bm_iter != bm_end; ++bm_iter) {
    actual.push_back({bm_iter->first - str.begin(),
                      std::string(bm_iter->first, bm_iter->second)});
  }
  EXPECT_THAT(actual, ElementsAreArray({
                          Pair(0, "aa"),
                          Pair(10, "aa"),
                      }));
}

TEST(BMTest, Bug1) {
  // when s1 has only two same character, and str contains s1,
  // when run bm::count(str.begin(), str.end(), Subsequence<T>(s1.begin(),
  // s1.end())), bug occures: program die here. reason: in bm::next(first,
  // subseq), next position set to:
  //     first + subseq.good_suffix(0) - subseq.size()
  // but this is wrong. first + subseq.good_suffix(0) donates new tail
  // element to be checked firstly in [first, last), it should minus
  // subseq.size()-1. so the correct expression is:
  //     first + subseq.good_suffix(0) + 1 - subseq.size()
  // which make first+subseq.good_suffix(0)+1 donates the element after
  // new tail element that to be checked in [first, last), then the
  // whole expression donates the position matches subseq[0].
  // bug fixed.

  std::string s1("aa"), str("aabcdefghiaac");
  bm::Subsequence<char> subseq(s1.begin(), s1.end());
  EXPECT_EQ(bm::count(str.begin(), str.end(), subseq), 2);
}
