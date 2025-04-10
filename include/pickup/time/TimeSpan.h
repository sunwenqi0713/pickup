#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

namespace pickup {
namespace time {

class TimeSpan {
 public:
  // Initialize timespan with a zero time duration
  TimeSpan() noexcept : duration_(0) {}
  // Initialize timespan with a specific time duration in nanoseconds
  explicit TimeSpan(int64_t duration) noexcept : duration_(duration) {};
  // Initialize timespan with a given std::chrono duration
  template <class Rep, class Period>
  explicit TimeSpan(const std::chrono::duration<Rep, Period>& duration) noexcept
      : duration_(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count()) {}
  // Initialize timespan with time components
  TimeSpan(int64_t days, int64_t hours, int64_t minutes, int64_t seconds, int64_t milliseconds = 0,
           int64_t microseconds = 0, int64_t nanoseconds = 0) noexcept
      : duration_((days * 24 * 60 * 60 + hours * 60 * 60 + minutes * 60 + seconds) * 1000000000ll +
                  milliseconds * 1000000ll + microseconds * 1000ll + nanoseconds) {}
  TimeSpan(const TimeSpan&) noexcept = default;
  TimeSpan(TimeSpan&&) noexcept = default;
  ~TimeSpan() noexcept = default;

  TimeSpan& operator=(int64_t duration) noexcept {
    duration_ = duration;
    return *this;
  }
  TimeSpan& operator=(const TimeSpan&) noexcept = default;
  TimeSpan& operator=(TimeSpan&&) noexcept = default;

  // 运算符重载
  TimeSpan operator+(const TimeSpan& other) const noexcept { return TimeSpan(duration_ + other.duration_); }
  TimeSpan operator-(const TimeSpan& other) const noexcept { return TimeSpan(duration_ - other.duration_); }
  TimeSpan& operator+=(int64_t offset) noexcept {
    duration_ += offset;
    return *this;
  }
  TimeSpan& operator+=(const TimeSpan& other) noexcept {
    duration_ += other.duration_;
    return *this;
  }
  TimeSpan& operator-=(int64_t offset) noexcept {
    duration_ -= offset;
    return *this;
  }
  TimeSpan& operator-=(const TimeSpan& other) noexcept {
    duration_ -= other.duration_;
    return *this;
  }

  // 比较运算符
  bool operator==(const TimeSpan& other) const noexcept { return duration_ == other.duration_; }
  bool operator!=(const TimeSpan& other) const noexcept { return duration_ != other.duration_; }
  bool operator<(const TimeSpan& other) const noexcept { return duration_ < other.duration_; }
  bool operator>(const TimeSpan& other) const noexcept { return duration_ > other.duration_; }
  bool operator<=(const TimeSpan& other) const noexcept { return duration_ <= other.duration_; }
  bool operator>=(const TimeSpan& other) const noexcept { return duration_ >= other.duration_; }

  // Convert timespan to the std::chrono nanoseconds duration
  std::chrono::system_clock::duration chrono() const noexcept {
    return std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::nanoseconds(duration_));
  }

  // 获取时间分量
  int64_t days() const noexcept { return duration_ / (24 * 60 * 60 * 1000000000ll); }
  int64_t hours() const noexcept { return duration_ / (60 * 60 * 1000000000ll); }
  int64_t minutes() const noexcept { return duration_ / (60 * 1000000000ll); }
  int64_t seconds() const noexcept { return duration_ / 1000000000; }
  int64_t milliseconds() const noexcept { return duration_ / 1000000; }
  int64_t microseconds() const noexcept { return duration_ / 1000; }
  int64_t nanoseconds() const noexcept { return duration_; }

  // 获取总时间量
  int64_t total() const noexcept { return duration_; }

  // get zero timespan
  static TimeSpan zero() noexcept { return TimeSpan(0); }

 private:
  int64_t duration_;
};

}  // namespace time
}  // namespace pickup