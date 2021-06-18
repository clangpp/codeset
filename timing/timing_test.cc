#include "timing.h"

#include <iomanip>
#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

TEST(TimingTest, EndToEnd) {
  const int N = 2000;
  timing::start();
  std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(9);
  for (int i = 0; i < N; ++i) {
    std::cout << i << " " << timing::duration() << std::endl;
  }
  std::cout << std::endl;
  timing::stop();
  std::cout << "total duration: " << timing::duration() << std::endl;
  std::cout << "period: " << timing::period() << std::endl;
  timing::clear();
  std::cout << "duration after clear: " << timing::duration() << std::endl;

  timing::start();
  for (int i = 0; i < N; ++i) {
    std::cout << i << " " << timing::duration() << std::endl;
  }
  std::cout << "restart..." << std::endl;
  timing::restart();
  for (int i = 0; i < N; ++i) {
    std::cout << i << " " << timing::duration() << std::endl;
  }
  timing::stop();
  std::cout << "total duration: " << timing::duration() << std::endl;
  timing::clear();
  std::cout << "duration after clear: " << timing::duration() << std::endl;
}
