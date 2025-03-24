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
  // 时间单位类型定义
  using Seconds = std::chrono::seconds;
  using Minutes = std::chrono::minutes;
  using Hours = std::chrono::hours;
  using Days = std::chrono::duration<int, std::ratio<86400>>;

  // 构造方法
  TimeSpan() = default;
  explicit TimeSpan(Seconds sec) noexcept : duration_(sec) {}

  TimeSpan(int days, int hours, int mins, int secs) {
    duration_ = Seconds(secs) + Minutes(mins) + Hours(hours) + Days(days);
  }

  // 获取时间分量
  int days() const noexcept { return std::chrono::duration_cast<Days>(duration_).count(); }
  int hours() const noexcept { return std::chrono::duration_cast<Hours>(duration_ % Days(1)).count(); }
  int minutes() const noexcept { return std::chrono::duration_cast<Minutes>(duration_ % Hours(1)).count(); }
  int seconds() const noexcept { return std::chrono::duration_cast<Seconds>(duration_ % Minutes(1)).count(); }

  // 获取总时间量
  int64_t totalDays() const noexcept { return std::chrono::duration_cast<Days>(duration_).count(); }
  int64_t totalHours() const noexcept { return std::chrono::duration_cast<Hours>(duration_).count(); }
  int64_t totalMinutes() const noexcept { return std::chrono::duration_cast<Minutes>(duration_).count(); }
  int64_t totalSeconds() const noexcept { return duration_.count(); }

  // 运算符重载
  TimeSpan operator+(const TimeSpan& other) const noexcept { return TimeSpan(duration_ + other.duration_); }
  TimeSpan operator-(const TimeSpan& other) const noexcept { return TimeSpan(duration_ - other.duration_); }
  TimeSpan& operator+=(const TimeSpan& other) noexcept {
    duration_ += other.duration_;
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

  // 格式化
  std::string format(const std::string& fmt = "%D days %H:%M:%S") const {
    std::stringstream ss;
    for (size_t i = 0; i < fmt.size(); ++i) {
      if (fmt[i] == '%' && i + 1 < fmt.size()) {
        switch (fmt[++i]) {
          case 'D':
            ss << days();
            break;
          case 'H':
            ss << std::setw(2) << std::setfill('0') << hours();
            break;
          case 'M':
            ss << std::setw(2) << std::setfill('0') << minutes();
            break;
          case 'S':
            ss << std::setw(2) << std::setfill('0') << seconds();
            break;
          case 't':
            ss << totalSeconds();  // 总秒数
            break;
          case '%':
            ss << '%';
            break;
          default:
            throw std::invalid_argument("Invalid format specifier");
        }
      } else {
        ss << fmt[i];
      }
    }
    return ss.str();
  }

  // 转换为标准库duration
  Seconds toDuration() const noexcept { return duration_; }

 private:
  Seconds duration_{0};
};

}  // namespace time
}  // namespace pickup