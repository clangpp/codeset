#include "configure.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::ElementsAreArray;
using ::testing::Pair;

TEST(ConfigureTest, EndToEnd) {
  std::string content = R"(
# this is configuration file of tt_test

tt_server_host = 59.64.138.51  # public tt server host
log_file = tt_test.log  # write log to file
cases = case1 case2 case3 case4  # test cases

[case1]  # a section is a case
server_port = 3456  # tt server port
record_amount = 100  # insert, lookup, remove amount

[case2]
server_port = 3457
record_amount = 10000

[case3]
server_port = 3458
record_amount = 100000

[case4]
server_port = 3459
record_amount = 1000000
)";
  std::string cfg_file = testing::TempDir() + "/test.cfg";
  std::ofstream os(cfg_file);
  os << content;
  os.close();
  configure::read(cfg_file);

  // get configurations
  // =========================================================================
  EXPECT_EQ(configure::default_get<std::string>("log_file", "log.txt"),
            "tt_test.log");

  EXPECT_TRUE(configure::search("cases"));

  auto cases = configure::get<std::vector<std::string>>("cases");
  EXPECT_THAT(cases, ElementsAreArray({"case1", "case2", "case3", "case4"}));

  // get case configurations
  // =========================================================================
  std::vector<std::pair<int, int>> port_and_amounts = {
      {3456, 100},
      {3457, 10000},
      {3458, 100000},
      {3459, 1000000},
  };
  for (size_t i = 0; i < cases.size(); ++i) {
    EXPECT_TRUE(configure::search(cases[i], "server_port"));
    EXPECT_EQ(configure::get<std::string>("tt_server_host"), "59.64.138.51");
    EXPECT_THAT(configure::get<int>(cases[i], "server_port"),
                port_and_amounts[i].first);
    EXPECT_THAT(configure::get<int>(cases[i], "record_amount"),
                port_and_amounts[i].second);
  }
}

TEST(ReadCommandLineOptionsTest, ItWorks) {
  char* options[] = {"cmd",       "sub-cmd",     "--port", "1024", "--server",
                     "127.0.0.1", "192.168.0.1", "--port", "1025", "1027"};
  configure::Configure cfg;
  cfg.read(facility::array_length(options), options);
  EXPECT_THAT(cfg.get<std::vector<int>>("port"),
              ElementsAreArray({1025, 1027}));
}

TEST(IteratorTest, ItWorks) {
  std::vector<std::pair<std::string, std::string>> kvs;
  for (configure::Configure::iterator iter = configure::begin();
       iter != configure::end(); ++iter) {
    kvs.push_back(*iter);
  }
  EXPECT_THAT(kvs, ElementsAreArray({
                       Pair("case1.record_amount", "100"),
                       Pair("case1.server_port", "3456"),
                       Pair("case2.record_amount", "10000"),
                       Pair("case2.server_port", "3457"),
                       Pair("case3.record_amount", "100000"),
                       Pair("case3.server_port", "3458"),
                       Pair("case4.record_amount", "1000000"),
                       Pair("case4.server_port", "3459"),
                       Pair("cases", "case1 case2 case3 case4"),
                       Pair("log_file", "tt_test.log"),
                       Pair("tt_server_host", "59.64.138.51"),
                   }));
}
