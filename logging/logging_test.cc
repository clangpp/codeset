// TODO(clangpp): Make this compile.

#include "logging.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "gtest/gtest.h"

TEST(StandardLoggerTest, ItWorks) {
  logging::Trace trace(logging::INFO_, "test_standard_logger()");

  // perform logging on standard logger through stream inserter
  {
    using namespace logging;
    Trace trace(INFO_, "test stream inserter");

    std::string message_suffix(" message through stream inserter");
    log(DEBUG_) << "debug" << message_suffix << std::endl;
    log(INFO_) << "info" << message_suffix << std::endl;
    log(WARNING_) << "warning" << message_suffix << std::endl;
    log(ERROR_) << "error" << message_suffix << std::endl;
    log(CRITICAL_) << "critical" << message_suffix << std::endl;
    log(INFO_) << "testing log() and level() ..." << std::endl;
    logging::log(INFO_) << ": ";
    for (int i = 0; i < 5; ++i) log() << i << " ";
    logging::Level old_level = logging::level();
    logging::level() = logging::ERROR_;
    for (int i = 5; i < 10; ++i) log() << i << " ";
    logging::level() = old_level;
    log() << std::endl;
    log(INFO_) << "test log() and level() done." << std::endl;
  }

  // perform logging on standard logger through
  //		namespace scope convenient functions
  {
    logging::Trace trace(logging::INFO_, "test namespace functions");

    std::string message_suffix(" message through namespace functions");
    logging::debug("debug" + message_suffix);
    logging::info("info" + message_suffix);
    logging::warning("warning" + message_suffix);
    logging::error("error" + message_suffix);
    logging::critical("critical" + message_suffix);
    logging::log(logging::DEBUG_,
                 "debug message through logging::log(level, message)");
    logging::log(logging::CRITICAL_,
                 "critical message through logging::log(level, message)");
  }
}

TEST(LoggerTest, ItWorks) {
  using namespace logging;
  Trace trace(INFO_, "test_logger()");

  log(INFO_) << "initialize Logger object with file 'logger.log'" << std::endl;
  std::ofstream flog("logger.log");
  Logger logger(flog);
  standard_logger().attach_sink(flog);

  // test logging APIs
  log(INFO_) << "logger's severity level is " << logger.level_of_sink(flog)
             << " now." << std::endl;

  test_logger_inserter(logger);
  test_logger_functions(logger);

  Level level = ERROR_;
  log(INFO_) << "changing logger's level to " << level << std::endl;
  logger.level_of_sink(flog) = level;

  test_logger_inserter(logger);
  test_logger_functions(logger);

  standard_logger().detach_sink(flog);

  // test sink observers
  log(INFO_) << "clog in logger's sink set? " << std::boolalpha
             << logger.verify_sink(clog) << std::endl;
  log(INFO_) << "adding clog into logger's sink set..." << std::endl;
  logger.attach_sink(clog);
  log(INFO_) << "clog in logger's sink set now? " << std::boolalpha
             << logger.verify_sink(clog) << std::endl;

  log(INFO_) << "clog's severity level is " << logger.level_of_sink(clog)
             << std::endl;
  level = CRITICAL_;
  log(INFO_) << "changing clog's severity level to " << level << std::endl;
  logger.level_of_sink(clog) = level;
  log(INFO_) << "clog's severity level is " << logger.level_of_sink(clog)
             << " now" << std::endl;

  log(INFO_) << "clog in logger's sink set? " << std::boolalpha
             << logger.verify_sink(clog) << std::endl;
  log(INFO_) << "removing clog from logger's sink set..." << std::endl;
  logger.detach_sink(clog);
  log(INFO_) << "clog in logger's sink set now? " << std::boolalpha
             << logger.verify_sink(clog) << std::endl;
}

TEST(SeverityLevelsTest, ItWorks) {
  using namespace logging;
  Trace trace(INFO_, "test_severity_levels()");
  log(INFO_) << "logging data goes to '<level>.log'" << std::endl;

  Logger logger;  // construct logger with no sinks

  // attach sinks of different severity level
  std::ofstream fdebug("debug.log");
  logger.attach_sink(fdebug, DEBUG_);
  std::ofstream finfo("info.log");
  logger.attach_sink(finfo);  // INFO_ is the default level
  std::ofstream fwarning("warning.log");
  logger.attach_sink(fwarning, WARNING_);
  std::ofstream ferror("error.log");
  logger.attach_sink(ferror, ERROR_);
  std::ofstream fcritical("critical.log");
  logger.attach_sink(fcritical, CRITICAL_);

  test_logger_inserter(logger);
  test_logger_functions(logger);
}

TEST(LoggerInserterTest, ItWorks) {
  using namespace logging;
  Trace trace(INFO_, "test_logger_inserter()");
  log(INFO_) << "log all level messages by stream inserter" << std::endl;

  std::string message_suffix(" message through stream inserter");
  logger(DEBUG_) << "debug" << message_suffix << std::endl;
  logger(INFO_) << "info" << message_suffix << std::endl;
  logger(WARNING_) << "warning" << message_suffix << std::endl;
  logger(ERROR_) << "error" << message_suffix << std::endl;
  logger(CRITICAL_) << "critical" << message_suffix << std::endl;
}

TEST(LoggerFunctionsTest, ItWorks) {
  using namespace logging;
  Trace trace(INFO_, "test_logger_functions()");
  log(INFO_) << "log all level messages by logger's functions" << std::endl;

  std::string message_suffix(" message through member function");
  logger.debug("debug" + message_suffix);
  logger.info("info" + message_suffix);
  logger.warning("warning" + message_suffix);
  logger.error("error" + message_suffix);
  logger.critical("critical" + message_suffix);

  logger.log(DEBUG_,
             "debug message through logging::Logger::log(level, message)");
  logger.log(CRITICAL_,
             "critical message through logging::Logger::log(level, message)");
}

TEST(IndentFunc3Test, ItWorks) {
  using namespace logging;
  Trace trace(INFO_, "test_indent_func3()");
  {
    Indent indent("^^^^");
    log(INFO_) << "hello there, I am an indent test" << std::endl;
  }
  {
    Indent indent("----");
    log(INFO_) << "this is another indent scope" << std::endl;
  }
  {
    Indent indent("^_^ ");
    log(INFO_) << "byebye!" << std::endl;
  }
}

TEST(IndentFunc2Test, ItWorks) {
  using namespace logging;
  Trace trace(INFO_, "test_indent_func2()");
  Indent indent("____");
  log(INFO_) << "about to call test_indent_func3()" << std::endl;
  test_indent_func3();
  log(INFO_) << "finish called test_indent_func3()" << std::endl;
}

TEST(IndentFunc1Test, ItWorks) {
  using namespace logging;
  Trace trace(INFO_, "test_indent_func1()");
  Indent indent;
  log(INFO_) << "about to call test_indent_func2()" << std::endl;
  test_indent_func2();
  log(INFO_) << "finish called test_indent_func2()" << std::endl;
}

TEST(IndentTest, ItWorks) {
  using namespace logging;
  Trace trace(INFO_, "test_indent()");
  Indent indent;
  test_indent_func1();
}

TEST(AttachTest, ItWorks) {
  using namespace logging;
  Trace trace(INFO_, "test_attach()");
  std::string log_file("test_attach.log");
  std::ofstream flog;
  flog.open(log_file.c_str(), ios::app);
  {
    Attach attach(INFO_, flog);
    log(INFO_) << "You'll see this line in '" << log_file << "'" << std::endl;
  }
  log(INFO_) << "You won't see this line in '" << log_file << "'" << std::endl;
  {
    Attach attach(INFO_, flog);
    log(INFO_) << "You'll see this line in '" << log_file << "' again"
               << std::endl;
  }
  log(INFO_) << "You won't see this line in '" << log_file << "' either"
             << std::endl;
}

TEST(LogIteratorTest, ItWorks) {
  using namespace logging;
  Trace trace(INFO_, "test_log_iterator()");
  int values[] = {1, 2, 3, 4, 5};
  log(INFO_) << "values are ";
  copy(values, values + sizeof(values) / sizeof(int), log(" "));
  log() << std::endl;

  Logger logger(log());
  int values1[] = {1, 2, 3, 4, 5};
  logger(INFO_) << "on logger, values are ";
  copy(values1, values1 + sizeof(values1) / sizeof(int), logger(" "));
  logger << std::endl;
}

TEST(NamedLoggerTest, ItWorks) {
  using namespace logging;
  Trace trace(INFO_, "test_named_logger()");
  NamedLogger nlog("module1");
  nlog(INFO_) << "message line" << std::endl;

  nlog = logging::named_logger("module2");
  nlog(INFO_) << "message line 2" << std::endl;

  int values[] = {1, 2, 3, 4, 5};
  nlog(INFO_) << "values are ";
  copy(values, values + sizeof(values) / sizeof(int), nlog(" "));
  nlog << std::endl;

  nlog = NamedLogger();
  copy(values, values + sizeof(values) / sizeof(int), nlog(ERROR_)(" "));
  nlog << std::endl;
}
