#include "waf_facility.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

TEST(TermSetTest, EndToEnd) {
  waf::TermSet termset;

  std::stringstream ss;
  ss << " ( 0 aa ) \n ( 1 ab ) \n ( 2 c ) \n ( 4 e ) \n";
  std::string str = ss.str();
  ss >> termset;

  std::stringstream so;
  so << termset;
  EXPECT_EQ(so.str(), str);

  EXPECT_EQ(termset.size(), 4);

  EXPECT_EQ("aa", termset[0]);
  EXPECT_EQ("ab", termset[1]);
  EXPECT_EQ("c", termset[2]);
  EXPECT_EQ("e", termset[4]);

  EXPECT_FALSE(termset.search(3));
  EXPECT_TRUE(termset.search(4));
  EXPECT_FALSE(termset.search(5));
  EXPECT_EQ(termset.count(3), 0);
  EXPECT_EQ(termset.count(4), 1);
  EXPECT_EQ(termset.count(5), 0);

  EXPECT_EQ(0, termset["aa"]);
  EXPECT_EQ(1, termset["ab"]);
  EXPECT_EQ(2, termset["c"]);
  EXPECT_EQ(4, termset["e"]);

  EXPECT_TRUE(termset.search("ab"));
  EXPECT_FALSE(termset.search("ac"));
  EXPECT_EQ(termset.count("ab"), 1);
  EXPECT_EQ(termset.count("ac"), 0);

  EXPECT_EQ(termset.max_termid(), 4);

  EXPECT_FALSE(termset.insert(4, "ad"));
  EXPECT_FALSE(termset.search("ad"));

  EXPECT_FALSE(termset.insert("ab", 3));
  EXPECT_EQ(1, termset.search("ab"));

  EXPECT_TRUE(termset.search(2));
  EXPECT_TRUE(termset.search("c"));
  EXPECT_FALSE(termset.erase(3));
  EXPECT_TRUE(termset.erase(2));
  EXPECT_FALSE(termset.search(2));
  EXPECT_FALSE(termset.search("c"));

  EXPECT_TRUE(termset.search("e"));
  EXPECT_TRUE(termset.search(4));
  EXPECT_FALSE(termset.erase("f"));
  EXPECT_TRUE(termset.erase("e"));
  EXPECT_FALSE(termset.search("e"));
  EXPECT_FALSE(termset.search(4));

  EXPECT_EQ(termset.max_termid(), 1);

  EXPECT_EQ(termset.size(), 2);
  termset.clear();
  EXPECT_EQ(termset.size(), 0);
}

TEST(FreqVectorTest, EndToEnd) {
  waf::FreqVector freqvec;

  std::stringstream ss;
  ss << " ( 0 500 ) \n ( 1 100 ) \n ( 2 200 ) \n ( 4 400 ) \n";
  std::string str = ss.str();
  ss >> freqvec;

  std::stringstream so;
  so << freqvec;
  EXPECT_EQ(so.str(), str);

  EXPECT_EQ(500, freqvec[0]);
  EXPECT_EQ(100, freqvec[1]);
  EXPECT_EQ(200, freqvec[2]);
  EXPECT_EQ(0, freqvec[3]);
  EXPECT_EQ(400, freqvec[4]);

  waf::FreqVector::const_iterator citer = freqvec.begin();
  EXPECT_EQ(200, *(citer + 2));
  EXPECT_EQ(200, citer[2]);
  EXPECT_EQ(0, *(citer + 3));
  EXPECT_EQ(0, *(citer + 5));

  waf::FreqVector::iterator iter = freqvec.begin();
  EXPECT_EQ(200, *(iter + 2));
  EXPECT_EQ(200, iter[2]);
  EXPECT_EQ(0, *(iter + 3));
  EXPECT_EQ(0, *(iter + 5));
}

TEST(CellCompTest, EndToEnd) {
  serialization::sparsematrix::Cell<int> c1(10, 20, 1000), c2(10, 20, 1000);

  EXPECT_FALSE(waf::cell_value_greater(c1, c2));
  EXPECT_FALSE(waf::cell_value_greater(c2, c1));

  c1.column = 19;
  EXPECT_TRUE(waf::cell_value_greater(c1, c2));
  EXPECT_FALSE(waf::cell_value_greater(c2, c1));

  c1.row = 9;
  EXPECT_TRUE(waf::cell_value_greater(c1, c2));
  EXPECT_FALSE(waf::cell_value_greater(c2, c1));

  c1.value = 1025;
  EXPECT_TRUE(waf::cell_value_greater(c1, c2));
  EXPECT_FALSE(waf::cell_value_greater(c2, c1));
}

TEST(CareAllTest, ItWorks) {
  waf::Care care = waf::care_all();

  EXPECT_TRUE(care(1000));
  EXPECT_TRUE(care(1));
  EXPECT_TRUE(care(0));
}

TEST(CareInTest, ItWorks) {
  waf::TermSet termset;

  termset.insert(0, "0");
  termset.insert(1, "1");
  termset.insert(3, "3");

  waf::Care care = waf::care_in(termset);
  EXPECT_TRUE(care(0));
  EXPECT_TRUE(care(1));
  EXPECT_FALSE(care(2));
  EXPECT_TRUE(care(3));
  EXPECT_FALSE(care(4));

  int a[5] = {0, 1, 2, 3, 4};
  std::remove_if(a, a + 5, waf::care_in(termset));
  EXPECT_EQ(2, a[0]);
  EXPECT_EQ(4, a[1]);
}

TEST(FreqDictTest, ItWorks) {
  waf::FreqVector freqvec;

  freqvec[0] = 1;
  freqvec[1] = 5;
  freqvec[3] = 11;

  EXPECT_EQ(1, waf::freq_dict(freqvec)(0));
  EXPECT_EQ(5, waf::freq_dict(freqvec)(1));
  EXPECT_EQ(11, waf::freq_dict(freqvec)(3));

  EXPECT_EQ(0, waf::freq_dict(freqvec)(2));
  EXPECT_EQ(0, waf::freq_dict(freqvec)(4));
}
