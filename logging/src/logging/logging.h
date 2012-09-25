// logging.h
#ifndef LOGGING_H_
#define LOGGING_H_

/* convenient and simple log library,
 * provide simple interface for simple log task
 */

// Author: Yongtian Zhang
// email: yongtianzhang@gmail.com
// Date: 2011.08.25
// Last Modified At: 2011.10.13

#include <ctime>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace logging {

// enumeration of logging levels
enum Level {
    DEBUG_,  // diagnosing information
    INFO_,  // normal executing status information
    WARNING_,  // warning information
    ERROR_, // error information
    CRITICAL_  // critical error information
};

// convert level enumeration to name string
// note: strong coupled with enum Level
inline std::string to_string(Level level, size_t width=0)
{
    static const char* LEVEL_STRINGS[] = {
        "DEBUG",
        "INFO",
        "WARNING",
        "ERROR",
        "CRITICAL"
    };
    std::string name(LEVEL_STRINGS[level]);
    if (width>name.size())
        name.append(width-name.size(), ' ');
    return name;
}

// write Level name string to stream
inline std::ostream& operator << (std::ostream& os, Level level) {
    return os << to_string(level);
}

// get current timestamp string in format YYYY-MM-DD HH:MM:SS
inline std::string current_timestamp() {
    std::time_t t = std::time(0);
    char str[50]={0};  // string buffer, size should be big enough
    strftime(str, 50, "%Y-%m-%d %H:%M:%S", localtime(&t));
    return str;
}

// a logger class, provide convenient API for simple logging usage
class Logger {
private:
    typedef std::map<std::ostream*, Level> SinkContainer;
    typedef std::vector<std::string> IndentStack;

public:
    Logger(): message_level_(INFO_) {}
    Logger(std::ostream& sink, Level level=INFO_): message_level_(INFO_) {
        attach_sink(sink, level);
    }

    // write log information to sinks
    template <typename T>
    Logger& operator << (const T& value) {
        dispatch_message(value); return *this;
    }
    Logger& operator << (std::ostream& (*pf) (std::ostream&)) {
        return operator << <std::ostream& (*) (std::ostream&)>(pf);
    }
    Logger& operator << (std::ios& (*pf) (std::ios&)) {
        return operator << <std::ios& (*) (std::ios&)>(pf);
    }
    Logger& operator << (std::ios_base& (*pf) (std::ios_base&)) {
        return operator << <std::ios_base& (*) (std::ios_base&)>(pf);
    }

    // set current log level, write log item prefix, and return the logger
    Logger& operator () (Level level) {
        set_message_level(level);
        write_timestamp();
        write_message_level();
        write_indents();
        return *this;
    }

    // write log with level, logger's internal level won't change
    void log(Level level, const std::string& message) {
        (*this)(level) << message << std::endl;
    }

    // convenient functions, just call log(level, message)
    void debug(const std::string& message) { log(DEBUG_, message); }
    void info(const std::string& message) { log(INFO_, message); }
    void warning(const std::string& message) { log(WARNING_, message); }
    void error(const std::string& message) { log(ERROR_, message); }
    void critical(const std::string& message) { log(CRITICAL_, message); }

    // get level reference of specified sink,
    // exception: if sink is not in logger's sink set, throw std::runtime_error
    Level& level_of_sink(std::ostream& sink) {
        SinkContainer::iterator sink_iter = sinks_.find(&sink);
        if (sink_iter == sinks_.end())
            throw std::runtime_error("sink not in logger's sink set!");
        return sink_iter->second;
    }

    // check whether a sink is attached to logger
    bool verify_sink(std::ostream& sink) { return sinks_.count(&sink)>0; }

    // attach sink to logger
    void attach_sink(std::ostream& sink, Level level=INFO_) {
        sinks_[&sink] = level;
    }

    // detach sink from logger
    void detach_sink(std::ostream& sink) { sinks_.erase(&sink); }

    // add one more level indent to each message
    void indent(const std::string& info) { indents_.push_back(info); }

    // remove newest level indent to each message
    void unindent() { indents_.pop_back(); }

public:  // output iterator APIs
    class LogIterator:
        public std::iterator<std::output_iterator_tag, void, void, void, void>{
    public:
        LogIterator(Logger& logger, const std::string& delim):
            logger_(logger), delim_(delim) {}
        LogIterator& operator ++ () { return *this; }
        LogIterator& operator ++ (int) { return *this; }
        LogIterator& operator * () { return *this; }
        template <typename T>
        LogIterator& operator = (const T& value) {
            logger_ << value << delim_; return *this;
        }
    private:
        Logger& logger_;
        std::string delim_;
    };
    LogIterator operator () (const std::string& delim) {  // LogIterator observer
        return LogIterator(*this, delim);
    }
    LogIterator operator () (const char* delim) {
        return (*this)(delim ? std::string(delim) : std::string());
    }

public:  // key functions
    template <typename T>
    void dispatch_message(const T& value) {
        for (SinkContainer::const_iterator sink=sinks_.begin();
                sink!=sinks_.end(); ++sink) {
            // write only when message_level_ >= sink's level
            if (message_level_>=sink->second) (*sink->first) << value;
        }
    }
    void set_message_level(Level level) { message_level_ = level; }
    void write_timestamp() { *this << current_timestamp() << " "; }
    void write_message_level() { *this << to_string(message_level_, 8) << " "; }
    void write_indents() {
        for (IndentStack::const_iterator ii=
                indents_.begin(); ii!=indents_.end(); ++ii) {
            (*this) << (*ii);
        }
    }

private:
    SinkContainer sinks_;  // log sink set
    Level message_level_;  // write message to sinks with level <= message_level_
    IndentStack indents_;  // indent string list
};

// get global standard logger, with sink std::clog
inline Logger& standard_logger() {
    static Logger logger(std::clog, INFO_);
    return logger;
}
inline Logger& log() { return standard_logger(); }  // short name

// free function mapping standard logger's member function /level_of_sink(sink)/
inline Level& level(std::ostream& sink=std::clog) {
    return standard_logger().level_of_sink(sink);
}

// free function mapping standard logger's member function /operator () (level)/
inline Logger& log(Level level) { return standard_logger()(level); }

// free functions mapping standard logger's corresponding member functions
inline void log(Level level, const std::string& message) {
    standard_logger().log(level, message);
}
inline void debug(const std::string& message) {
    standard_logger().debug(message);
}
inline void info(const std::string& message) {
    standard_logger().info(message);
}
inline void warning(const std::string& message) {
    standard_logger().warning(message);
}
inline void error(const std::string& message) {
    standard_logger().error(message);
}
inline void critical(const std::string& message) {
    standard_logger().critical(message);
}

// free function mapping standard logger's member function /operator () (delim)/
inline Logger::LogIterator log(const std::string& delim) {
    return standard_logger()(delim);
}
inline Logger::LogIterator log(const char* delim) {
    return log(delim ? std::string(delim) : std::string());
}

// trace helper, log 'enter' and 'leave' message of scope
class Trace {
public:
    Trace(Level level, const std::string& scope,
            Logger& logger=standard_logger()):
            logger_(logger), message_level_(level), scope_(scope) {
        logger_(message_level_) << "enter " << scope_ << std::endl;
    }
    ~Trace() { logger_(message_level_) << "leave " << scope_ << std::endl; }

private:
    Logger& logger_;
    Level message_level_;
    std::string scope_;
};

// indent helper, add and remove indents for each message
class Indent {
public:
    Indent(const std::string& info="    ",
            Logger& logger=standard_logger()): logger_(logger) {
        logger_.indent(info);
    }
    ~Indent() { logger_.unindent(); }
private:
    Logger& logger_;
};

// attach helper, attach and remove sink for logger
class Attach {
public:
    Attach(Level level, std::ostream& sink,
            Logger& logger=standard_logger()):
            logger_(logger), sink_(sink) {
        logger_.attach_sink(sink, level);
    }
    ~Attach() { logger_.detach_sink(sink_); }
private:
    Logger& logger_;
    std::ostream& sink_;
};

// class NamedLogger, provide log facility with module name
class NamedLogger {
public:
    explicit NamedLogger(const std::string& name="",
            Logger& logger=standard_logger()): name_(name), logger_(&logger) {}

    // write log information to sinks
    template <typename T>
    Logger& operator << (const T& value) {
        logger_->dispatch_message(value); return *this;
    }
    Logger& operator << (std::ostream& (*pf) (std::ostream&)) {
        return operator << <std::ostream& (*) (std::ostream&)>(pf);
    }
    Logger& operator << (std::ios& (*pf) (std::ios&)) {
        return operator << <std::ios& (*) (std::ios&)>(pf);
    }
    Logger& operator << (std::ios_base& (*pf) (std::ios_base&)) {
        return operator << <std::ios_base& (*) (std::ios_base&)>(pf);
    }

    NamedLogger& operator () (Level level) {
        logger_->set_message_level(level);
        logger_->write_timestamp();
        logger_->write_message_level();
        write_name();
        logger_->write_indents();
        return *this;
    }

public:  // LogIterator observer
    Logger::LogIterator operator () (const std::string& delim) {
        return Logger::LogIterator(*logger_, delim);
    }
    Logger::LogIterator operator () (const char* delim) {
        return (*this)(delim ? std::string(delim) : std::string());
    }

public:  // key functions
    void write_name() { *logger_ << "[" << name_ << "] "; }
    operator Logger& () { return *logger_; }

private:
    std::string name_;
    Logger* logger_;
};

// generate NamedLogger object
inline NamedLogger named_logger(
        const std::string& name, Logger& logger=standard_logger()) {
    return NamedLogger(name, logger);
}

}  // namespace logging

#endif  // LOGGING_H_
