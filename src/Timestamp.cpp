#include "pickup/time/Timestamp.h"

#include <stdexcept>  // For std::runtime_error
#include <utility>    // For std::swap

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>  // For GetSystemTimeAsFileTime
#else
#include <sys/time.h>  // For gettimeofday
#endif

namespace pickup {
namespace time {

Timestamp::Timestamp() { update(); }

Timestamp::Timestamp(TimeVal tv) : tv_(tv) {}

Timestamp::Timestamp(const Timestamp& other) : tv_(other.tv_) {}

Timestamp::~Timestamp() {}

Timestamp& Timestamp::operator=(const Timestamp& other) {
  if (this != &other) {
    tv_ = other.tv_;
  }
  return *this;
}

Timestamp& Timestamp::operator=(TimeVal tv) {
  tv_ = tv;
  return *this;
}

void Timestamp::swap(Timestamp& timestamp) { std::swap(tv_, timestamp.tv_); }

void Timestamp::update() {
#if defined(_WIN32) || defined(_WIN64)
  FILETIME ft;
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
  GetSystemTimePreciseAsFileTime(&ft);
#else
  GetSystemTimeAsFileTime(&ft);
#endif
  tv_ = ((int64_t)ft.dwHighDateTime << 32 | ft.dwLowDateTime) / 10;  // 转换为微秒
#else
  timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) throw std::runtime_error("Cannot get current time!");
  tv_ = static_cast<TimeVal>(ts.tv_sec) * 1000000 + static_cast<TimeVal>(ts.tv_nsec) / 1000;
#endif
}

bool Timestamp::operator==(const Timestamp& ts) const { return tv_ == ts.tv_; }
bool Timestamp::operator!=(const Timestamp& ts) const { return tv_ != ts.tv_; }
bool Timestamp::operator>(const Timestamp& ts) const { return tv_ > ts.tv_; }
bool Timestamp::operator>=(const Timestamp& ts) const { return tv_ >= ts.tv_; }
bool Timestamp::operator<(const Timestamp& ts) const { return tv_ < ts.tv_; }
bool Timestamp::operator<=(const Timestamp& ts) const { return tv_ <= ts.tv_; }

Timestamp Timestamp::operator+(Timestamp::TimeDiff d) const { return Timestamp(tv_ + d); }
Timestamp Timestamp::operator-(Timestamp::TimeDiff d) const { return Timestamp(tv_ - d); }
Timestamp::TimeDiff Timestamp::operator-(const Timestamp& ts) const { return tv_ - ts.tv_; }
Timestamp& Timestamp::operator+=(Timestamp::TimeDiff d) {
  tv_ += d;
  return *this;
}

Timestamp& Timestamp::operator-=(Timestamp::TimeDiff d) {
  tv_ -= d;
  return *this;
}

Timestamp::TimeVal Timestamp::epochMilliseconds() const { return tv_ / 1000; }

Timestamp::TimeVal Timestamp::epochMicroseconds() const { return tv_; }

Timestamp::TimeDiff Timestamp::elapsed() const {
  Timestamp now;
  return now - *static_cast<const Timestamp*>(this);
}

bool Timestamp::isElapsed(const Timestamp::TimeDiff interval) const {
  Timestamp now;
  return (now - *static_cast<const Timestamp*>(this)) >= interval;
}

}  // namespace time
}  // namespace pickup