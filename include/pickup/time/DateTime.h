#pragma once

#include <array>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

#include "pickup/time/TimeSpan.h"
#include "pickup/utils/platform.h"

namespace pickup {
namespace time {

class DateTime {
 public:
  // 构造方法
  DateTime() noexcept : millisecondsSinceEpoch‌_(0) {}
  explicit DateTime(int64_t millisecondsSinceEpoch) noexcept : millisecondsSinceEpoch‌_(millisecondsSinceEpoch) {}
  DateTime(int year, int month, int day, int hours = 0, int minutes = 0, int seconds = 0, int milliseconds = 0) {
    std::tm tm = {};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hours;
    tm.tm_min = minutes;
    tm.tm_sec = seconds;
    tm.tm_isdst = -1;

    // 将时间转换为时间戳
    std::time_t time = std::mktime(&tm);
    if (time == -1) {
      throw std::invalid_argument("Invalid date and time");
    }
    // 将时间戳转换为毫秒数
    milliseconds = milliseconds < 0 ? 0 : milliseconds;
    // 计算自 epoch 以来的毫秒数，std::mktime 返回的是秒数，所以乘以 1000 转换为毫秒
    millisecondsSinceEpoch‌_ = 1000 * (int64_t)time + milliseconds;
  }

  DateTime(const DateTime&) noexcept = default;
  DateTime(DateTime&&) noexcept = default;
  ~DateTime() noexcept = default;

  DateTime& operator=(int64_t millisecondsSinceEpoch) noexcept {
    millisecondsSinceEpoch‌_ = millisecondsSinceEpoch;
    return *this;
  }
  DateTime& operator=(const DateTime&) noexcept = default;
  DateTime& operator=(DateTime&&) noexcept = default;

  DateTime operator+(const TimeSpan& other) const noexcept {
    return DateTime(millisecondsSinceEpoch‌_ + other.total());
  }
  DateTime operator-(const TimeSpan& other) const noexcept {
    return DateTime(millisecondsSinceEpoch‌_ - other.total());
  }
  DateTime operator+(const DateTime& other) const noexcept {
    return DateTime(millisecondsSinceEpoch‌_ + other.millisecondsSinceEpoch‌_);
  }
  DateTime operator-(const DateTime& other) const noexcept {
    return DateTime(millisecondsSinceEpoch‌_ - other.millisecondsSinceEpoch‌_);
  }
  DateTime& operator+=(DateTime& other) noexcept {
    millisecondsSinceEpoch‌_ += other.millisecondsSinceEpoch‌_;
    return *this;
  }
  DateTime& operator-=(DateTime& other) noexcept {
    millisecondsSinceEpoch‌_ -= other.millisecondsSinceEpoch‌_;
    return *this;
  }
  DateTime& operator+=(TimeSpan& other) noexcept {
    millisecondsSinceEpoch‌_ += other.total();
    return *this;
  }
  DateTime& operator-=(TimeSpan& other) noexcept {
    millisecondsSinceEpoch‌_ -= other.total();
    return *this;
  }

  // 用于获取当前时间的时间戳
  static int64_t currentTimeMillis() {
    auto now = std::chrono::system_clock::now();
    // 转换为自 epoch 以来的毫秒数
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
  }

  static DateTime currentTime() noexcept { return DateTime(currentTimeMillis()); }

  // 获取时间组件
  int year() const { return getLocalTm(&std::tm::tm_year) + 1900; }
  int month() const { return getLocalTm(&std::tm::tm_mon) + 1; }
  int day() const { return getLocalTm(&std::tm::tm_mday); }
  int hours() const { return getLocalTm(&std::tm::tm_hour); }
  int minutes() const { return getLocalTm(&std::tm::tm_min); }
  int seconds() const { return getLocalTm(&std::tm::tm_sec); }
  int milliseconds() const { return millisecondsSinceEpoch‌_ % 1000; }
  int dayOfWeek() const { return getLocalTm(&std::tm::tm_wday); }
  int dayOfYear() const { return getLocalTm(&std::tm::tm_yday); }
  bool isLeapYear() { return isLeapYear(year()); }

  // 比较运算符
  bool operator==(const DateTime& other) const noexcept {
    return millisecondsSinceEpoch‌_ == other.millisecondsSinceEpoch‌_;
  }
  bool operator!=(const DateTime& other) const noexcept {
    return millisecondsSinceEpoch‌_ != other.millisecondsSinceEpoch‌_;
  }
  bool operator<(const DateTime& other) const noexcept {
    return millisecondsSinceEpoch‌_ < other.millisecondsSinceEpoch‌_;
  }
  bool operator>(const DateTime& other) const noexcept {
    return millisecondsSinceEpoch‌_ > other.millisecondsSinceEpoch‌_;
  }
  bool operator<=(const DateTime& other) const noexcept {
    return millisecondsSinceEpoch‌_ <= other.millisecondsSinceEpoch‌_;
  }
  bool operator>=(const DateTime& other) const noexcept {
    return millisecondsSinceEpoch‌_ >= other.millisecondsSinceEpoch‌_;
  }

  // 格式化输出
  std::string format(const std::string& format = "%Y-%m-%d %H:%M:%S") const {
    std::tm tm = millisToLocal(millisecondsSinceEpoch‌_);

    char buffer[256];
    std::strftime(buffer, sizeof(buffer), format.c_str(), &tm);
    std::string result(buffer);

    // 处理毫秒 %f（3位）
    size_t pos;
    while ((pos = result.find("%f")) != std::string::npos) {
      std::stringstream ss;
      ss << std::setw(3) << std::setfill('0') << milliseconds();
      result.replace(pos, 3, ss.str());
    }

    return result;
  }

  static bool isLeapYear(int year) { return (year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0); }

  static bool isValid(int year, int month, int day, int hours, int minutes, int seconds, int milliseconds) {
    return (year >= 0 && year <= 9999) && (month >= 1 && month <= 12) &&
           (day >= 1 && day <= daysOfMonth(year, month)) && (hours >= 0 && hours <= 23) &&
           (minutes >= 0 && minutes <= 59) && (seconds >= 0 && seconds <= 60) &&
           (milliseconds >= 0 && milliseconds <= 999);
  }

 private:
  static void zerostruct(std::tm& tm) {
    tm.tm_sec = 0;
    tm.tm_min = 0;
    tm.tm_hour = 0;
    tm.tm_mday = 1;
    tm.tm_mon = 0;
    tm.tm_year = 0;
    tm.tm_wday = 0;
    tm.tm_yday = 0;
    tm.tm_isdst = -1;
  }

  static std::tm millisToLocal(int64_t millis) noexcept {
#ifdef PICKUP_WINDOWS
    std::tm result;
    millis /= 1000;

    if (_localtime64_s(&result, &millis) != 0) zerostruct(result);

    return result;
#else
    std::tm result;
    auto now = (time_t)(millis / 1000);

    if (localtime_r(&now, &result) == nullptr) zerostruct(result);

    return result;
#endif
  }

  int getLocalTm(int std::tm::* member) const {
    std::tm tm = millisToLocal(millisecondsSinceEpoch‌_);
    return tm.*member;
  }

  static int daysOfMonth(int year, int month) {
    std::array<int, 13> daysOfMonthTable = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && isLeapYear(year))
      return 29;
    else
      return daysOfMonthTable[month];
  }

 private:
  int64_t millisecondsSinceEpoch‌_;
};

}  // namespace time
}  // namespace pickup