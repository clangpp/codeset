#include "ac.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::ElementsAreArray;
using ::testing::Pair;

TEST(AutomationTest, ItWorks) {
  std::string str("abcdefghijklmnopqrstuvwxyz");
  std::vector<std::string> substrs = {"abc", "abcdf", "mnopq", "bac"};

  ac::Automation<char, int> automation;
  EXPECT_TRUE(automation.empty());
  EXPECT_EQ(automation.size(), 0);

  ac::Automation<char, int>::node_pointer pnode;
  for (const auto& substr : substrs) {
    automation.insert(substr.begin(), substr.end(), true, pnode);
  }

  EXPECT_FALSE(automation.empty());
  EXPECT_EQ(automation.size(), 4);

  auto match = ac::find_first_of(str.begin(), str.end(), automation);
  EXPECT_EQ(match.first - str.begin(), 0);
  EXPECT_EQ(match.second - str.begin(), 3);

  EXPECT_TRUE(ac::search(str.begin(), str.end(), automation));
  EXPECT_EQ(ac::count(str.begin(), str.end(), automation), 2);

  automation.clear();
  EXPECT_TRUE(automation.empty());
  EXPECT_EQ(automation.size(), 0);
}

TEST(TrieMapTest, ItWorks) {
  std::string str("abcdefghijklmnopqrstuvwxyz");
  std::string s1("abc"), s2("abcdf"), s3("mnopq"), s4("ghi"), s5("xyz"),
      s6("opq"), s7("rst");

  ac::TrieMap<char, int> mp;
  ac::TrieMap<char, int>::const_iterator citer(mp.end());
  EXPECT_EQ(citer == mp.end(), true);
  EXPECT_EQ(citer != mp.end(), false);
  EXPECT_EQ(mp.end() == citer, true);
  EXPECT_EQ(mp.end() != citer, false);

  auto ret = mp.insert(s1.begin(), s1.end(), 1);
  std::string iter_key(ret.first.key().begin(), ret.first.key().end());
  EXPECT_EQ(iter_key, s1);
  EXPECT_EQ(ret.first.value(), 1);

  const ac::TrieMap<char, int>::const_iterator cciter = ret.first;
  iter_key.assign(cciter.key().begin(), cciter.key().end());
  EXPECT_EQ(iter_key, s1);
  EXPECT_EQ(ret.first.value(), 1);

  mp.insert(make_pair(s2.begin(), s2.end()), 2);
  mp.insert(s3, 3);
  mp[s4] = 4;

  ac::TrieMap<char, int>::node_pointer np;
  mp.insert(s5.begin(), s5.end(), 5, np);
  mp.insert(s6, 6);
  mp.insert(s7, 7);

  const ac::TrieMap<char, int>& cmp(mp);
  EXPECT_NE(mp.find(s5), mp.end());
  EXPECT_GT(cmp.count(s5), 0);

  mp.erase(s5.begin(), s5.end());
  mp.erase(make_pair(s6.begin(), s6.end()));
  mp.erase(s7);
  EXPECT_EQ(mp.find(s5), mp.end());
  EXPECT_EQ(cmp.count(s5), 0);
  EXPECT_EQ(mp.size(), 4);

  auto match = ac::find_first_of(str.begin(), str.end(), mp);
  EXPECT_EQ(match.first - str.begin(), 0);
  EXPECT_EQ(match.second - str.begin(), 3);
  EXPECT_TRUE(ac::search(str.begin(), str.end(), mp));
  EXPECT_EQ(ac::count(str.begin(), str.end(), mp), 3);
}

TEST(IteratorTest, ItWorks) {
  std::string str("abcdefghijklmnopqrstuvwxyz");
  std::string s1("abc"), s2("abcdf"), s3("mnopq"), s4("ghi");

  ac::SubsequenceSet<char> subset;
  subset.insert(s1.begin(), s1.end());
  subset.insert(make_pair(s2.begin(), s2.end()));
  subset.insert(s3);
  subset[s4] = true;
  subset.erase(s1);

  auto match = ac::find_first_of(str.begin(), str.end(), subset);
  EXPECT_EQ(match.first - str.begin(), 6);
  EXPECT_EQ(match.second - str.begin(), 9);
  EXPECT_TRUE(ac::search(str.begin(), str.end(), subset));
  EXPECT_EQ(ac::count(str.begin(), str.end(), subset), 2);

  ac::SubsequenceIterator<std::string::const_iterator> ac_iter(
      str.begin(), str.end(), subset),
      ac_end;
  std::vector<std::pair<std::string, bool>> actual;
  for (; ac_iter != ac_end; ++ac_iter) {
    actual.push_back(
        {std::string(ac_iter->first, ac_iter->second), ac_iter->mapped()});
  }
  EXPECT_THAT(actual, ElementsAreArray({
                          Pair("ghi", true),
                          Pair("mnopq", false),
                      }));
}

TEST(AcTest, Chinese) {
  std::string str = "早就上班了你睡觉的时间不短哈你今天不上班啊";
  std::vector<std::string> dict = {"今天", "上班", "时间", "睡觉", "早就"};
  ac::SubsequenceSet<char> subset;
  for (const auto& s : dict) {
    subset.insert(s.rbegin(), s.rend());
  }
  for (std::size_t i = 0; i < dict.size(); ++i) {
    EXPECT_NE(subset.find(dict[i].rbegin(), dict[i].rend()), subset.end());
  }
  ac::SubsequenceIterator<std::string::reverse_iterator> ac_iter(
      str.rbegin(), str.rend(), subset),
      ac_end;
  std::vector<std::string> actual;
  for (; ac_iter != ac_end; ++ac_iter) {
    actual.push_back(
        std::string(ac_iter->second.base(), ac_iter->first.base()));
  }
  EXPECT_THAT(actual, ElementsAreArray(
                          {"上班", "今天", "时间", "睡觉", "上班", "早就"}));
}
