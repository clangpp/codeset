// test.cpp

#include "logging/logging.h"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

void test_standard_logger();
void test_logger();
void test_severity_levels();
void test_logger_inserter(logging::Logger& logger);
void test_logger_functions(logging::Logger& logger);
void test_indent();
void test_attach();
void test_log_iterator();
void test_named_logger();

int main(int argc, char* argv[]) {

	test_standard_logger();
	test_logger();
	test_severity_levels();
    test_indent();
    test_attach();
    test_log_iterator();
    test_named_logger();
	system("pause");
	return EXIT_SUCCESS;
}

void test_standard_logger() {
	logging::Trace trace(logging::INFO_, "test_standard_logger()");

	// perform logging on standard logger through stream inserter
	{
		using namespace logging;
		Trace trace(INFO_, "test stream inserter");

		string message_suffix(" message through stream inserter");
		log(DEBUG_) << "debug" << message_suffix << endl;
		log(INFO_) << "info" << message_suffix << endl;
		log(WARNING_) << "warning" << message_suffix << endl;
		log(ERROR_) << "error" << message_suffix << endl;
		log(CRITICAL_) << "critical" << message_suffix << endl;
        log(INFO_) << "testing log() and level() ..." << endl;
        logging::log(INFO_) << ": ";
        for (int i=0; i<5; ++i) log() << i << " ";
        logging::Level old_level = logging::level();
        logging::level() = logging::ERROR_;
        for (int i=5; i<10; ++i) log() << i << " ";
        logging::level() = old_level;
        log() << endl;
        log(INFO_) << "test log() and level() done." << endl;
	}

	// perform logging on standard logger through 
	//		namespace scope convenient functions
	{
		logging::Trace trace(logging::INFO_, "test namespace functions");

		string message_suffix(" message through namespace functions");
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


void test_logger() {
	using namespace logging;
	Trace trace(INFO_, "test_logger()");

	log(INFO_) << "initialize Logger object with file 'logger.log'" << endl;
	ofstream flog("logger.log");
	Logger logger(flog);
	standard_logger().attach_sink(flog);

	// test logging APIs
	log(INFO_) << "logger's severity level is "
		<< logger.level_of_sink(flog) << " now." << endl;

	test_logger_inserter(logger);
	test_logger_functions(logger);

	Level level=ERROR_;
	log(INFO_) << "changing logger's level to " << level << endl;
	logger.level_of_sink(flog) = level;

	test_logger_inserter(logger);
	test_logger_functions(logger);

	standard_logger().detach_sink(flog);

	// test sink observers
	log(INFO_) << "clog in logger's sink set? "
		<< boolalpha << logger.verify_sink(clog) << endl;
	log(INFO_) << "adding clog into logger's sink set..." << endl;
	logger.attach_sink(clog);
	log(INFO_) << "clog in logger's sink set now? "
		<< boolalpha << logger.verify_sink(clog) << endl;

	log(INFO_) << "clog's severity level is "
		<< logger.level_of_sink(clog) << endl;
	level=CRITICAL_;
	log(INFO_) << "changing clog's severity level to " << level << endl;
	logger.level_of_sink(clog) = level;
	log(INFO_) << "clog's severity level is "
		<< logger.level_of_sink(clog) << " now" << endl;

	log(INFO_) << "clog in logger's sink set? "
		<< boolalpha << logger.verify_sink(clog) << endl;
	log(INFO_) << "removing clog from logger's sink set..." << endl;
	logger.detach_sink(clog);
	log(INFO_) << "clog in logger's sink set now? "
		<< boolalpha << logger.verify_sink(clog) << endl;
}


void test_severity_levels() {
	using namespace logging;
	Trace trace(INFO_, "test_severity_levels()");
	log(INFO_) << "logging data goes to '<level>.log'" << endl;

	Logger logger;  // construct logger with no sinks

	// attach sinks of different severity level
	ofstream fdebug("debug.log");
	logger.attach_sink(fdebug, DEBUG_);
	ofstream finfo("info.log");
	logger.attach_sink(finfo);  // INFO_ is the default level
	ofstream fwarning("warning.log");
	logger.attach_sink(fwarning, WARNING_);
	ofstream ferror("error.log");
	logger.attach_sink(ferror, ERROR_);
	ofstream fcritical("critical.log");
	logger.attach_sink(fcritical, CRITICAL_);

	test_logger_inserter(logger);
	test_logger_functions(logger);
}


void test_logger_inserter(logging::Logger& logger) {
	using namespace logging;
	Trace trace(INFO_,  "test_logger_inserter()");
	log(INFO_) << "log all level messages by stream inserter" << endl;

	string message_suffix(" message through stream inserter");
	logger(DEBUG_) << "debug" << message_suffix << endl;
	logger(INFO_) << "info" << message_suffix << endl;
	logger(WARNING_) << "warning" << message_suffix << endl;
	logger(ERROR_) << "error" << message_suffix << endl;
	logger(CRITICAL_) << "critical" << message_suffix << endl;
}


void test_logger_functions(logging::Logger& logger) {
	using namespace logging;
	Trace trace(INFO_,  "test_logger_functions()");
	log(INFO_) << "log all level messages by logger's functions" << endl;

	string message_suffix(" message through member function");
	logger.debug("debug" + message_suffix);
	logger.info("info" + message_suffix);
	logger.warning("warning" + message_suffix);
	logger.error("error" + message_suffix);
	logger.critical("critical" + message_suffix);

	logger.log(DEBUG_, "debug message through logging::Logger::log(level, message)");
	logger.log(CRITICAL_,
			"critical message through logging::Logger::log(level, message)");
}

void test_indent_func3() {
    using namespace logging;
    Trace trace(INFO_, "test_indent_func3()");
    {
        Indent indent("^^^^");
        log(INFO_) << "hello there, I am an indent test" << endl;
    }
    {
        Indent indent("----");
        log(INFO_) << "this is another indent scope" << endl;
    }
    {
        Indent indent("^_^ ");
        log(INFO_) << "byebye!" << endl;
    }
}

void test_indent_func2() {
    using namespace logging;
    Trace trace(INFO_, "test_indent_func2()");
    Indent indent("____");
    log(INFO_) << "about to call test_indent_func3()" << endl;
    test_indent_func3();
    log(INFO_) << "finish called test_indent_func3()" << endl;
}

void test_indent_func1() {
    using namespace logging;
    Trace trace(INFO_, "test_indent_func1()");
    Indent indent;
    log(INFO_) << "about to call test_indent_func2()" << endl;
    test_indent_func2();
    log(INFO_) << "finish called test_indent_func2()" << endl;
}

void test_indent() {
    using namespace logging;
    Trace trace(INFO_, "test_indent()");
    Indent indent;
    test_indent_func1();
}

void test_attach() {
    using namespace logging;
    Trace trace(INFO_, "test_attach()");
    string log_file("test_attach.log");
    ofstream flog;
    flog.open(log_file.c_str(), ios::app);
    {
        Attach attach(INFO_, flog);
        log(INFO_) << "You'll see this line in '" << log_file << "'" << endl;
    }
    log(INFO_) << "You won't see this line in '" << log_file << "'" << endl;
    {
        Attach attach(INFO_, flog);
        log(INFO_) << "You'll see this line in '" << log_file << "' again" << endl;
    }
    log(INFO_) << "You won't see this line in '" << log_file << "' either" << endl;
}

void test_log_iterator() {
    using namespace logging;
    Trace trace(INFO_, "test_log_iterator()");
    int values[] = {1, 2, 3, 4, 5};
    log(INFO_) << "values are ";
    copy(values, values+sizeof(values)/sizeof(int), log(" "));
    log() << endl;

    Logger logger(log());
    int values1[] = {1, 2, 3, 4, 5};
    logger(INFO_) << "on logger, values are ";
    copy(values1, values1+sizeof(values1)/sizeof(int), logger(" "));
    logger << endl;
}

void test_named_logger() {
    using namespace logging;
    Trace trace(INFO_, "test_named_logger()");
    NamedLogger nlog("module1");
    nlog(INFO_) << "message line" << endl;
    
	nlog = logging::named_logger("module2");
    nlog(INFO_) << "message line 2" << endl;

    int values[] = {1, 2, 3, 4, 5};
    nlog(INFO_) << "values are ";
    copy(values, values+sizeof(values)/sizeof(int), nlog(" "));
    nlog << endl;

	nlog = NamedLogger();
    copy(values, values+sizeof(values)/sizeof(int), nlog(ERROR_)(" "));
    nlog << endl;
}
