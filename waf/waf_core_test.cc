#include "waf_core.h"

#include <cassert>
#include <functional>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "waf_facility.h"

TEST(TermFrequencyTest, ItWorks) {
  std::stringstream ss;
  ss << "1 2 3 5 3 -1 5 2 5 3 5 4 5 -1";
  std::istream_iterator<int> is_iter(ss), is_end;

  waf::TermSet termset;
  termset.insert(1, "1");
  termset.insert(2, "2");
  termset.insert(3, "3");
  termset.insert(5, "5");

  waf::FreqVector freqvec;
  waf::term_frequency(is_iter, is_end, waf::care_in(termset), freqvec.begin());

  EXPECT_EQ(freqvec[1], 1);
  EXPECT_EQ(freqvec[2], 2);
  EXPECT_EQ(freqvec[3], 3);
  EXPECT_EQ(freqvec[5], 5);

  EXPECT_EQ(freqvec[0], 0);
  EXPECT_EQ(freqvec[4], 0);
}

TEST(CoOccurrenceTest, ItWorks) {
  SparseMatrix<waf::cooccur_type> co_mat;

  std::stringstream ss;
  ss << "0 1 2 2 3 5 -1 0 3 1";
  std::istream_iterator<int> is_iter(ss), is_end;
  waf::co_occurrence(is_iter, is_end, waf::care_all(), waf::care_all(), 5,
                     co_mat);
  waf::mean_distance(co_mat.begin(), co_mat.end());

  EXPECT_NEAR(co_mat(0, 2).first, 2, 0.01);
  EXPECT_NEAR(co_mat(0, 1).first, 1.5, 0.01);
  EXPECT_NEAR(co_mat.iget(5, 0).first, 0, 0.01);
}

TEST(MeanDistanceTest, ItWorks) {
  std::vector<std::pair<double, int>> vec = {{8, 4}, {12, -24}, {0, 1}};

  waf::mean_distance(vec.begin(), vec.end());

  EXPECT_NEAR(vec[0].first, 2, 0.01);
  EXPECT_NEAR(vec[1].first, -0.5, 0.01);
  EXPECT_NEAR(vec[2].first, 0, 0.01);
}

TEST(TotalDistanceTest, ItWorks) {
  std::vector<std::pair<double, int>> vec = {{2, 4}, {-0.5, -24}, {0, 1}};

  waf::total_distance(vec.begin(), vec.end());

  EXPECT_NEAR(vec[0].first, 8, 0.01);
  EXPECT_NEAR(vec[1].first, 12, 0.01);
  EXPECT_NEAR(vec[2].first, 0, 0.01);
}

void test_word_activation_force() {
  waf::cooccur_type co(5, 2);
  waf::force_type f = waf::word_activation_force(co, 3, 5);
  EXPECT_NEAR(f, 0.010667, 0.00001);
}

TEST(AffinityOrMeanTest, ItWorks) {
  waf::TermSet termset;
  termset.insert(0, "t");
  termset.insert(1, "u");
  termset.insert(2, "v");
  termset.insert(3, "w");
  termset.insert(4, "x");
  termset.insert(5, "y");
  termset.insert(6, "z");

  CrossList<waf::force_type> waf_mat(termset.max_termid() + 1,
                                     termset.max_termid() + 1);
  waf_mat(termset["t"], termset["u"]) = 0.04;
  waf_mat(termset["w"], termset["u"]) = 0.05;
  waf_mat(termset["z"], termset["u"]) = 0.05;

  waf_mat(termset["u"], termset["x"]) = 0.03;
  waf_mat(termset["u"], termset["y"]) = 0.03;

  waf_mat(termset["t"], termset["v"]) = 0.05;
  waf_mat(termset["w"], termset["v"]) = 0.02;

  waf_mat(termset["v"], termset["x"]) = 0.06;
  waf_mat(termset["v"], termset["y"]) = 0.03;
  waf_mat(termset["v"], termset["z"]) = 0.04;

  waf::termid_type u_id = termset["u"], v_id = termset["v"];

  // in-link of "u" and "v"
  waf::force_type or_mean = waf::affinity_or_mean(
      waf_mat.column_begin(u_id), waf_mat.column_end(u_id), waf::care_all(),
      waf_mat.column_begin(v_id), waf_mat.column_end(v_id), waf::care_all(),
      std::mem_fn(&CrossList<waf::force_type>::column_iterator::row));
  EXPECT_NEAR(or_mean, 0.4, 0.01);

  // out-link of "u" and "v"
  or_mean = waf::affinity_or_mean(
      waf_mat.row_begin(u_id), waf_mat.row_end(u_id), waf::care_all(),
      waf_mat.row_begin(v_id), waf_mat.row_end(v_id), waf::care_all(),
      std::mem_fn(&CrossList<waf::force_type>::row_iterator::column));
  EXPECT_NEAR(or_mean, 0.5, 0.01);
}

TEST(AffinityMeasureTest, ItWorks) {
  waf::TermSet termset;
  termset.insert(0, "t");
  termset.insert(1, "u");
  termset.insert(2, "v");
  termset.insert(3, "w");
  termset.insert(4, "x");
  termset.insert(5, "y");
  termset.insert(6, "z");

  CrossList<waf::force_type> waf_mat(termset.max_termid() + 1,
                                     termset.max_termid() + 1);
  waf_mat(termset["t"], termset["u"]) = 0.04;
  waf_mat(termset["w"], termset["u"]) = 0.05;
  waf_mat(termset["z"], termset["u"]) = 0.05;

  waf_mat(termset["u"], termset["x"]) = 0.03;
  waf_mat(termset["u"], termset["y"]) = 0.03;

  waf_mat(termset["t"], termset["v"]) = 0.05;
  waf_mat(termset["w"], termset["v"]) = 0.02;

  waf_mat(termset["v"], termset["x"]) = 0.06;
  waf_mat(termset["v"], termset["y"]) = 0.03;
  waf_mat(termset["v"], termset["z"]) = 0.04;

  waf::termid_type u_id = termset["u"], v_id = termset["v"];

  waf::affinity_type a = waf::affinity_measure(waf_mat, u_id, waf::care_all(),
                                               waf_mat, v_id, waf::care_all());
  EXPECT_NEAR(a, 0.45, 0.01);
}
