#ifndef TIMER_HPP
#define TIMER_HPP

#include <iostream>

class Timer {
  public:
    Timer();
    ~Timer();

    void reset();
    double elpased();

  private:
    typedef std::chrono::high_resolution_clock highResClock;
    typedef std::chrono::time_point<highResClock> time;
    typedef std::chrono::duration<double, std::ratio<1>> toSecs;

    time m_begin;
};

#endif
