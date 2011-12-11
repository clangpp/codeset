
DECLARATION
================================================================================

This library is written in standard C++ language, without any use of third-party
libraries. 

This library is totally written by Yongtian Zhang (yongtianzhang@gmail.com).

This library borrows concepts from Python's logging module, Google's glog 
library, log4cpp library, and EzLogger library. But no any code borrowed.

I don't guarantee this library's quality, user should be careful to use it.

Any one is free to get, use, modify, and redistribute this library, no need to 
let me know. But if you want to discuss with me, email me anytime. My email 
address is <yongtianzhang@gmail.com>.


INTRODUCTION
================================================================================

This is a C++ logging library. It is designed for simple logging tasks.

It is simple but not so stupid. It does not guarantee thread safety.

It provides a namespace /logging/, containing all the following utilities.

It provides a enumeriation class /Level/, defines several severity levels 
(debug, info, warning, error, critical), in increasing severity order.

It provides a class /Logger/, offers:
1. a C++ stream style API to log message; e.g. logger(INFO_) << "message" << endl;
2. some convenient functions to log message of specific level;
	e.g. logger.info("message");
3. observers to manipulate sinks where message goes to;
	e.g. logger.attach_sink(), logger.detach_sink(), logger.level_of_sink()=DEBUG_.

class /Logger/ maintains a set of std::ostream* as sink set. every log message 
is dispatched to all sinks in the sink set, with current timestamp and severity 
level as prefix. Every sink has a corresponding severity level, and then accepts
only messages that have equal or higher severity levels.

It provides a global standard logger object, initialized with sink stream 
std::clog. One can access it by a free function /standard_logger()/. Thus, 
member function calls on it will cause messages writing to standard error pipe 
(normally the screen).

It provides a convenient class /Trace/, help to log scope enter and leaving
messages. When a Trace instance is constructed, scope enter message will be 
logged. When the instance is destructed, scope leaving message will be logged.

It provides a convenient class /Indent/, help to add and remove indent
information to each message. When an Indent instance is constructed, a new level
indent information is added to the logger object, and this information will take
effect on each message logging after that. When the instance is destructed, the
indent information is then removed from the logger object.

It provides a convenient class /Attach/, help to attach sink to and detach sink
from logger object. When an Attach instance is constructed, a specific sink
will be attached to a specific logger object. When the instance is destructed,
the sink will be detached from the logger object.

It provides several namespace scope free functions, to perform convenient log 
operations on standard logger object. They are implemented just by calling
corresponding member functions of standard logger object, such as debug(), 
info(), error(), etc.


MOTIVATION
================================================================================

I was most motivated by standard /logging/ module in Python.
Python's logging module provides a Logger class, to perform convenient message 
logging and logging dispatching.
Its Handler classes work just like std::ostream in C++, filtering and accepting
messages sent by Logger.
What's more, logging module provides several module level non-member functions, 
with the same name with corresponding member functions of class Logger. They are
used to log message by the module's global logger object /root/.
I pick this frame, let Logger accept user message and dispatch messages to sink 
(std::ostream*) set. 
I borrow its severity level definitions, and the corresponding convenient 
function names, e.g. log(level, message), debug(message), info(message).

After I learned about Python's logging, I started to look for logging library in
C++. I googled and found some, log4cpp, ezlogger, and glog. I scanned their 
documentations, finding out some good concepts:
1. use C++ style stream API. e.g. in glog, LOG(INFO_) << "message";
2. log every message with severity level. e.g. in glog, LOG(INFO_) << "message";
3. model log sink as std::ostream's derive class;
I borrow them into my design, with one thing different. Most of them implement 
this API by macro, but I do this by operator overloading. 

The last thing is message formatting. Python's logging offers several predefined
message formatting. They are flexible for different usages. But I don't want to 
provide that complex function, so I force the message line's format to be:
YYYY-MM-DD HH:MM:SS LEVEL (INDENT)*message\n
The timestamp is local time, LEVEL is severity levels' upper case string form, 
(INDENT)* is for multi-level indents.
I think that is good enough for simple usages.


FILES
================================================================================

src/logging/logging.h
	It is the only file you need to add into your project. All logging utilities
	are defined in this file, within a namespace /logging/.

src/test.cpp
	It is an example using logging. It displays almost every logging API's
	usage. It can be considered as a simple tutorial of logging.

doc/documentation.docx
	It is the documentation of logging. It displays all the APIs of logging.

readme.txt
	It is the file you read now. It introduces the logging library, shows the 
	motivation of designing this library, and shows the file structure of the 
	library.

