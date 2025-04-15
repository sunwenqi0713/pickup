#include "pickup/time/Timestamp.h"

#include <stdexcept>  // For std::runtime_error
#include <utility>    // For std::swap

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <sys/time.h>
#include <time.h>
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

// Windows和Unix系统使用不同的时间起点（纪元），Windows使用1601年1月1日作为起点，而Unix使用1970年1月1日作为起点。
// 因此在Windows上需要减去116444736000000000个100ns单位的时间差，这个差值是从1601年到1970年的总时间（以100ns为单位）。
// 1601年到1970年之间有 369年 的差距，其中有89个闰年（1601-1970），所以总天数为 369 * 365 + 89 = 134774 天。
// 134774天 * 24小时/天 * 60分钟/小时 * 60秒/分钟 * 10000000微秒/秒 = 116444736000000000微秒。
// 这个差值是以100ns为单位的，所以需要除以10来得到微秒级别的时间差。
void Timestamp::update() {
#if defined(_WIN32) || defined(_WIN64)
  FILETIME ft;
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
  GetSystemTimePreciseAsFileTime(&ft);
#else
  GetSystemTimeAsFileTime(&ft);
#endif
  ULARGE_INTEGER uli;
  uli.LowPart = ft.dwLowDateTime;
  uli.HighPart = ft.dwHighDateTime;
  // 转换为Unix纪元时间（从1601-01-01到1970-01-01的100ns间隔数）
  const uint64_t UNIX_EPOCH = 116444736000000000ULL;
  // FILETIME是以100纳秒为单位的，所以需要除以10来得到微秒
  tv_ = (uli.QuadPart - UNIX_EPOCH) / 10;
#else
  timespec ts;
  if (clock_gettime(CLOCK_REALTIME, &ts) != 0) throw std::runtime_error("Cannot get current time!");
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

std::time_t Timestamp::epochTime() const { return static_cast<std::time_t>(tv_ / 1000000); }

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