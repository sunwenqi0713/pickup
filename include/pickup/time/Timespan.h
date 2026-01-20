#pragma once

#include <cstdint>
#include <string>

namespace pickup {
namespace time {

/**
 * 相对时间度量
 *
 * 时间以秒为单位存储，采用双精度浮点精度，可以是正数或负数
 *
 * 如果需要绝对时间（即日期+时间），请参阅 Time 类
 *
 * @tags{Core}
 */
class Timespan {
 public:
  /**
   * 创建 Timespan 对象
   *
   * @param seconds  秒数，可以是正数或负数
   * @see milliseconds, minutes, hours, days, weeks
   */
  explicit Timespan(double seconds = 0.0) noexcept;

  /**
   * 复制另一个相对时间对象
   */
  Timespan(const Timespan& other) noexcept;

  /**
   * 复制另一个相对时间对象
   */
  Timespan& operator=(const Timespan& other) noexcept;

  /**
   * 析构函数
   */
  ~Timespan() noexcept;

  /**
   * 创建一个表示若干毫秒的新 Timespan 对象
   * @see seconds, minutes, hours, days, weeks
   */
  static Timespan milliseconds(int milliseconds) noexcept;

  /**
   * 创建一个表示若干毫秒的新 Timespan 对象
   * @see seconds, minutes, hours, days, weeks
   */
  static Timespan milliseconds(int64_t milliseconds) noexcept;

  /**
   * 创建一个表示若干秒的新 Timespan 对象
   * @see milliseconds, minutes, hours, days, weeks
   */
  static Timespan seconds(double seconds) noexcept;

  /**
   * 创建一个表示若干分钟的新 Timespan 对象
   * @see milliseconds, hours, days, weeks
   */
  static Timespan minutes(double numberOfMinutes) noexcept;

  /**
   * 创建一个表示若干小时的新 Timespan 对象
   * @see milliseconds, minutes, days, weeks
   */
  static Timespan hours(double numberOfHours) noexcept;

  /**
   * 创建一个表示若干天的新 Timespan 对象
   * @see milliseconds, minutes, hours, weeks
   */
  static Timespan days(double numberOfDays) noexcept;

  /**
   * 创建一个表示若干周的新 Timespan 对象
   * @see milliseconds, minutes, hours, days
   */
  static Timespan weeks(double numberOfWeeks) noexcept;

  /**
   * 返回此时间表示的毫秒数
   * @see milliseconds, inSeconds, inMinutes, inHours, inDays, inWeeks
   */
  int64_t inMilliseconds() const noexcept;

  /**
   * 返回此时间表示的秒数
   * @see inMilliseconds, inMinutes, inHours, inDays, inWeeks
   */
  double inSeconds() const noexcept { return seconds_; }

  /**
   * 返回此时间表示的分钟数
   * @see inMilliseconds, inSeconds, inHours, inDays, inWeeks
   */
  double inMinutes() const noexcept;

  /**
   * 返回此时间表示的小时数
   * @see inMilliseconds, inSeconds, inMinutes, inDays, inWeeks
   */
  double inHours() const noexcept;

  /**
   * 返回此时间表示的天数。
   * @see inMilliseconds, inSeconds, inMinutes, inHours, inWeeks
   */
  double inDays() const noexcept;

  /**
   * 返回此时间表示的周数。
   * @see inMilliseconds, inSeconds, inMinutes, inHours, inDays
   */
  double inWeeks() const noexcept;

  /** 将另一个 Timespan 添加到此时间 */
  Timespan operator+=(Timespan timeToAdd) noexcept;
  /** 从此时间中减去另一个 RelativeTime */
  Timespan operator-=(Timespan timeToSubtract) noexcept;

  /** 将若干秒添加到此时间 */
  Timespan operator+=(double secondsToAdd) noexcept;
  /** 从此时间中减去若干秒 */
  Timespan operator-=(double secondsToSubtract) noexcept;

 private:
  double seconds_;  ///< 存储时间的秒数
};

/** 比较两个 Timespan 对象 */
bool operator==(Timespan t1, Timespan t2) noexcept;
/** 比较两个 Timespan 对象 */
bool operator!=(Timespan t1, Timespan t2) noexcept;
/** 比较两个 Timespan 对象 */
bool operator>(Timespan t1, Timespan t2) noexcept;
/** 比较两个 Timespan 对象 */
bool operator<(Timespan t1, Timespan t2) noexcept;
/** 比较两个 Timespan 对象 */
bool operator>=(Timespan t1, Timespan t2) noexcept;
/** 比较两个 Timespan 对象 */
bool operator<=(Timespan t1, Timespan t2) noexcept;

/** 将两个 Timespan 相加 */
Timespan operator+(Timespan t1, Timespan t2) noexcept;
/** 将两个 Timespan 相减 */
Timespan operator-(Timespan t1, Timespan t2) noexcept;

}  // namespace time
}  // namespace pickup