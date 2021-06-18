#include "statvar.h"

#include <cstdlib>
#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

template <typename... Ts>
std::string ToString(const StatVar<Ts...>& var) {
  std::stringstream ss;
  ss << var;
  return ss.str();
}

TEST(StatVarTest, EndToEnd) {
  StatVar<double> var1("var1"), var2;

  EXPECT_EQ(
      ToString(var1),
      "var1 sum=0 count=0 mean=nan min=0 max=0 variance=nan deviation=nan");

  var1.record(2.0);
  var1 += 1.0;

  var2.name = "var2";
  var2 += 1.3;
  var1 += var2;

  EXPECT_EQ(
      ToString(var1),
      R"(var1 sum=4.3 count=3 mean=1.43333 min=1 max=2 variance=0.175556 deviation=0.418994)");
  EXPECT_EQ(
      ToString(var2),
      R"(var2 sum=1.3 count=1 mean=1.3 min=1.3 max=1.3 variance=5.33427e-17 deviation=7.30361e-09)");
}
