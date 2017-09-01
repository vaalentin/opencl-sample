#include "Timer.hpp"

Timer::Timer() {
  m_begin = highResClock::now();
}

Timer::~Timer() {
}

void Timer::reset() {
  m_begin = highResClock::now();
}

double Timer::elpased() {
  return std::chrono::duration_cast<toSecs>(highResClock::now() - m_begin).count();
}
