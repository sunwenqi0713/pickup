#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

#include "pickup/time/TimeSpan.h"

namespace pickup {
namespace time {

class DateTime {
 public:
  // 时间单位定义
  using TimePoint = std::chrono::system_clock::time_point;

  // 构造方法
  DateTime() noexcept : time_(std::chrono::system_clock::now()) {}
  explicit DateTime(const TimePoint& tp) noexcept : time_(tp) {}
  DateTime(int year, int month, int day, int hour = 0, int minute = 0, int second = 0) {
    if (!isValid(year, month, day, hour, minute, second)) {
      throw std::invalid_argument("Invalid datetime components");
    }

    std::tm tm = {};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    tm.tm_isdst = -1;

    std::time_t tt = std::mktime(&tm);
    if (tt == -1) throw std::invalid_argument("Invalid datetime components");
    time_ = std::chrono::system_clock::from_time_t(tt);
  }

  // 获取时间组件
  int year() const { return getLocalTm(&std::tm::tm_year) + 1900; }
  int month() const { return getLocalTm(&std::tm::tm_mon) + 1; }
  int day() const { return getLocalTm(&std::tm::tm_mday); }
  int hour() const { return getLocalTm(&std::tm::tm_hour); }
  int minute() const { return getLocalTm(&std::tm::tm_min); }
  int second() const { return getLocalTm(&std::tm::tm_sec); }

  static bool isValid(int year, int month, int day, int hour, int minute, int second) {
    return (year >= 0 && year <= 9999) && (month >= 1 && month <= 12) &&
           (day >= 1 && day <= daysOfMonth(year, month)) && (hour >= 0 && hour <= 23) &&
           (minute >= 0 && minute <= 59) && (second >= 0 && second <= 60);
  }

  static bool isLeapYear(int year) { return (year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0); }

  static int daysOfMonth(int year, int month) {
    std::array<int, 13> daysOfMonthTable = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && isLeapYear(year))
      return 29;
    else
      return daysOfMonthTable[month];
  }

  static int dayOfYear(int year, int month, int day) {
    int doy = 0;
    for (int i = 1; i < month; ++i) {
      doy += daysOfMonth(year, i);
    }
    doy += day;
    return doy;
  }

  /**
   * 蔡基姆拉尔森计算公式（Zeller's congruence），又称蔡氏公式，是一种计算星期几的方法，公式如下：
   * [w = (d + 2m + 3(m + 1) \ 5 + y + y \4 - y \100 + y \400) \bmod 7]
   * 其中：
   * - (w)表示星期几（(0)代表星期日，(1)代表星期一，以此类推)
   * - (d)表示日期
   * - (m)表示月份（(3)代表三月，(4)代表四月，以此类推，把一月和二月看成上一年的(13)月和(14)月)
   * - (y)表示年份（如果是一月或二月，则是上一年的年份)
   */
  static int dayOfWeek(int year, int month, int day) {
    if (month == 1 || month == 2) {
      month += 12;
      year--;
    }
    int week = (day + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400 + 1) % 7;
    return week;
  }

  // 时间操作
  DateTime operator+(const TimeSpan& ts) { return DateTime(time_ + ts.toDuration()); }
  DateTime operator-(const TimeSpan& ts) { return DateTime(time_ - ts.toDuration()); }
  TimeSpan operator-(const DateTime& dt) {
    return TimeSpan(std::chrono::duration_cast<TimeSpan::Seconds>(time_ - dt.time_));
  }
  DateTime& operator+=(const TimeSpan& ts) noexcept {
    time_ += ts.toDuration();
    return *this;
  }

  DateTime& operator-=(const TimeSpan& ts) noexcept {
    time_ -= ts.toDuration();
    return *this;
  }

  // 比较运算符
  bool operator==(const DateTime& other) const noexcept { return time_ == other.time_; }
  bool operator!=(const DateTime& other) const noexcept { return time_ != other.time_; }
  bool operator<(const DateTime& other) const noexcept { return time_ < other.time_; }
  bool operator>(const DateTime& other) const noexcept { return time_ > other.time_; }
  bool operator<=(const DateTime& other) const noexcept { return time_ <= other.time_; }
  bool operator>=(const DateTime& other) const noexcept { return time_ >= other.time_; }

  // 格式化输出
  std::string format(const std::string& fmt) const {
    std::time_t tt = std::chrono::system_clock::to_time_t(time_);
    std::tm tm = *std::localtime(&tt);

    std::stringstream ss;
    ss << std::put_time(&tm, fmt.c_str());
    return ss.str();
  }

  // 静态方法
  static DateTime now() noexcept { return DateTime(std::chrono::system_clock::now()); }

 private:
  int getLocalTm(int std::tm::* member) const {
    std::time_t tt = std::chrono::system_clock::to_time_t(time_);
    std::tm tm = *std::localtime(&tt);
    return tm.*member;
  }

 private:
  TimePoint time_;
};

}  // namespace time
}  // namespace pickup