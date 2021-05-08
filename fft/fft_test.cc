#include "fft/fft.h"

#include <complex>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::ElementsAreArray;

TEST(FilterTest, ItWorks) {
  fft::Filter<long double> f;
  std::deque<std::complex<long double> > seq;
  seq.push_back(std::complex<long double>(1));
  seq.push_back(std::complex<long double>(0));
  seq.push_back(std::complex<long double>(-1));
  seq.push_back(std::complex<long double>(0));
  seq.push_back(std::complex<long double>(1));
  seq.push_back(std::complex<long double>(0));
  seq.push_back(std::complex<long double>(-1));
  seq.push_back(std::complex<long double>(0));
  std::deque<std::complex<long double> > seqb(seq);

  f.establish(seq.size());
  EXPECT_EQ(f.point_count(), 8);

  f.filter(seq);
  EXPECT_THAT(seq, ElementsAreArray({0, 0, 4, 0, 0, 0, 4, 0}));

  EXPECT_THAT(f(seqb), ElementsAreArray({0, 0, 4, 0, 0, 0, 4, 0}));
}
