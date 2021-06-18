#ifndef TIMING_H_
#define TIMING_H_

#include <chrono>

namespace timing {

// class Clock
// clock facility, to perform time representation and arithmetric
class Clock {
 public:
  typedef std::chrono::high_resolution_clock impl;
  typedef impl::time_point time_point;

 public:
  static void now(time_point& current) { current = impl::now(); }
  static double duration(const time_point& start, const time_point& stop) {
    impl::duration d = stop - start;
    return d.count() * period();
  }
  static double period() { return 1.0 * impl::period::num / impl::period::den; }
};

// class Timer
// timer facility, perform simple time tasks
class Timer {
 private:
  typedef Clock clock;
  typedef clock::time_point time_point;

 public:
  Timer() : running_(false) { clear(); }
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
  void restart() {
    stop();
    clear();
    start();
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
inline void restart() { standard_timer().restart(); }

}  // namespace timing

#endif  // TIMING_H_
