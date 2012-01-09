// timing.h
#ifndef TIMING_H_
#define TIMING_H_

// please config here
#define TIMING_USING_CPP11 0  // 0 or 1
#define TIMING_USING_BOOST 0  // 0 or 1
#define TIMING_USING_LINUX 0  // 0 or 1
#define TIMING_USING_CPP98 1  // 0 or 1

// class Clock
// clock facility, to perform time representation and arithmetric
// =============================================================================
#if TIMING_USING_CPP11  // using c++11 <chrono>

#include <chrono>

namespace timing {

class Clock {
public:
	typedef std::chrono::high_resolution_clock impl;
	typedef impl::time_point time_point;
public:
	static void now(time_point& current) {
		current = impl::now();
	}
	static double duration(const time_point& start, const time_point& stop) {
		impl::duration d = stop - start;
		return d.count() * period();
	}
	static double period() {
		return 1.0 * impl::period::num / impl::period::den;
	}
};

}  // namespace timing

#elif TIMING_USING_BOOST  // using boost.chrono (version >= 1.47.0)

#include <boost/chrono.hpp>

namespace timing {

class Clock {
public:
	typedef boost::chrono::high_resolution_clock impl;
	typedef impl::time_point time_point;
public:
	static void now(time_point& current) {
		current = impl::now();
	}
	static double duration(const time_point& start, const time_point& stop) {
		impl::duration d = stop - start;
		return d.count() * period();
	}
	static double period() {
		return 1.0 * impl::period::num / impl::period::den;
	}
};

}  // namespace timing

#elif TIMING_USING_LINUX  // using linux.gettimeofday

#include <utility>
#include <sys/time.h>
#include <unistd.h>

namespace timing {

class Clock {
public:
	typedef std::pair<struct timeval, struct timezone> time_point;
public:
	static void now(time_point& current) {
		gettimeofday(&current.first, &current.second);
	}
	static double duration(const time_point& start, const time_point& stop) {
		return (stop.first.tv_sec + stop.first.tv_usec * 1e-6) -
			(start.first.tv_sec + start.first.tv_usec * 1e-6);
	}
	static double period() { return 1e-6; }
};

}  // namespace timing

#else  // TIMING_USING_CPP98  // using c++98 <ctime>

#include <ctime>

namespace timing {

class Clock {
public:
	typedef std::time_t time_point;
public:
	static void now(time_point& current) {
		std::time(&current);
	}
	static double duration(const time_point& start, const time_point& stop) {
		return difftime(stop, start);
	}
	static double period() { return 1.0; }
};

}  // namespace timing

#endif  // TIMING_USING_*

namespace timing {

// class Timer
// timer facility, perform simple time tasks
// =============================================================================
class Timer {
private:
	typedef Clock clock;
	typedef clock::time_point time_point;
public:
    Timer(): running_(false) { clear(); }
    void start() {
		if (!running_) {  // take effect only when stopped
			clock::now(begin_);
			running_ = true;
        }
    }
    void stop() {
        if (running_) {  // take effect only when running
			clock::now(end_);
            duration_ += clock::duration(begin_, end_);
			running_ = false;
        }
    }
    double duration() const {
		if (!running_) {
			return duration_;
		} else {  // running
			time_point curr;
			clock::now(curr);
			return duration_ + clock::duration(begin_, curr);
		}
	}
    double period() const { return clock::period(); }
    void clear() {
		stop();
		clock::now(begin_);
		clock::now(end_);
		duration_ = 0.0;
	}
private:
    time_point begin_;
    time_point end_;
    double duration_;
    bool running_;
};

// namespace scope free functions
// =============================================================================
inline Timer& standard_timer() {
    static Timer timer;
    return timer;
}
inline void start() { standard_timer().start(); }
inline void stop() { standard_timer().stop(); }
inline void clear() { standard_timer().clear(); }
inline double duration() { return standard_timer().duration(); }
inline double period() { return standard_timer().period(); }

}  // namespace timing

#endif  // TIMING_H_
