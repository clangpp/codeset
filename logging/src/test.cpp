// test.cpp

#include "logging/logging.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

// test logging namespace scope API
void test_standard_logger();

// test logging::Logger
void test_logger();

// test severity levels
void test_severity_levels();

// write test messages to logger through stream inserter
void test_logger_inserter(logging::Logger& logger);

// write test messages to logger through member functions
void test_logger_functions(logging::Logger& logger);

// test indent facility
void test_indent();

// test attach facility
void test_attach();

int main(int argc, char* argv[]) {

    // test trace facility

	// test standard logger
	test_standard_logger();

	// test logging::Logger
	test_logger();

	// test severity levels
	test_severity_levels();

    // test indent facility
    test_indent();

    // test attach facility
    test_attach();

	// system("pause");
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
