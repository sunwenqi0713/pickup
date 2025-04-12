#pragma once

#include "pickup/time/Timestamp.h"

namespace pickup {
namespace time {

/**
 * @brief 表示最大精度为微秒的时间跨度类
 * @note 该类可表示从微秒到天级别的时间跨度，支持比较、算术运算和单位转换。
 */
class Timespan {
 public:
  using TimeDiff = Timestamp::TimeDiff;  ///< 时间差值类型（微秒单位）

  // 创建零时间跨度
  Timespan();

  /**
   * @brief 通过微秒数创建时间跨度
   * @param microseconds 总微秒数
   */
  Timespan(TimeDiff microseconds);

  /**
   * @brief 通过秒和微秒创建时间跨度（兼容 struct timeval）
   * @param seconds 秒数
   * @param microseconds 微秒数（0-999999）
   * @note 该构造函数将秒数转换为微秒数，并与微秒数相加，形成总的微秒数。
   */
  Timespan(long seconds, long microseconds);

  /**
   * @brief 通过多级时间单位创建时间跨度
   * @param days 天数
   * @param hours 小时数（0-23）
   * @param minutes 分钟数（0-59）
   * @param seconds 秒数（0-59）
   * @param microseconds 微秒数（0-999999）
   * @note 该构造函数将所有时间单位转换为微秒数，并相加形成总的微秒数。
   */
  Timespan(int days, int hours, int minutes, int seconds, int microseconds);

  // 拷贝构造函数
  Timespan(const Timespan& timespan);

  // 析构函数
  ~Timespan();

  // 拷贝赋值操作符
  Timespan& operator=(const Timespan& timespan);

  // 通过微秒数赋值
  Timespan& operator=(TimeDiff microseconds);

  /**
   * @brief 分配多级时间单位的时间跨度
   * @param days 天数
   * @param hours 小时数（0-23）
   * @param minutes 分钟数（0-59）
   * @param seconds 秒数（0-59）
   * @param microseconds 微秒数（0-999999）
   * @note 该方法将所有时间单位转换为微秒数，并相加形成总的微秒数。
   */
  Timespan& assign(int days, int hours, int minutes, int seconds, int microseconds);

  /**
   * @brief 分配秒和微秒的时间跨度（兼容 struct timeval）
   * @param seconds 秒数
   * @param microseconds 微秒数（0-999999）
   * @note 该方法将秒数转换为微秒数，并与微秒数相加，形成总的微秒数。
   */
  Timespan& assign(long seconds, long microseconds);

  // 与另一个 Timespan 交换值
  void swap(Timespan& timespan);

  // 比较操作符（Timespan vs Timespan）
  bool operator==(const Timespan& ts) const;
  bool operator!=(const Timespan& ts) const;
  bool operator>(const Timespan& ts) const;
  bool operator>=(const Timespan& ts) const;
  bool operator<(const Timespan& ts) const;
  bool operator<=(const Timespan& ts) const;

  // 比较操作符（Timespan vs 微秒数）
  bool operator==(TimeDiff microseconds) const;
  bool operator!=(TimeDiff microseconds) const;
  bool operator>(TimeDiff microseconds) const;
  bool operator>=(TimeDiff microseconds) const;
  bool operator<(TimeDiff microseconds) const;
  bool operator<=(TimeDiff microseconds) const;

  // 算术操作（Timespan vs Timespan）
  Timespan operator+(const Timespan& d) const;
  Timespan operator-(const Timespan& d) const;
  Timespan& operator+=(const Timespan& d);
  Timespan& operator-=(const Timespan& d);

  // 算术操作（Timespan vs 微秒数）
  Timespan operator+(TimeDiff microseconds) const;
  Timespan operator-(TimeDiff microseconds) const;
  Timespan& operator+=(TimeDiff microseconds);
  Timespan& operator-=(TimeDiff microseconds);

  // 获取天数部分（总天数中的整数值）
  int days() const;

  // 获取小时部分（0-23）
  int hours() const;

  // 获取总小时数（包含天数转换）
  int totalHours() const;

  // 获取分钟部分（0-59）
  int minutes() const;

  // 获取总分钟数（包含小时/天数转换）
  int totalMinutes() const;

  // 获取秒部分（0-59）
  int seconds() const;

  // 获取总秒数（包含分钟/小时/天数转换）
  int totalSeconds() const;

  // 获取毫秒部分（0-999）
  int milliseconds() const;

  // 获取总毫秒数
  TimeDiff totalMilliseconds() const;

  /**
   * @brief 获取微秒部分（0-999）
   * @note 此处的微秒是秒后的余数
   */
  int microseconds() const;

  /**
   * @brief 获取微秒部分（0-999999）
   * @note 此处的微秒是整个秒后的余数
   */
  int useconds() const;

  // 获取总微秒数
  TimeDiff totalMicroseconds() const;

  // 时间单位常量（单位：微秒）
  static const TimeDiff MILLISECONDS;  ///< 1 毫秒 = 1,000 微秒
  static const TimeDiff SECONDS;       ///< 1 秒 = 1,000,000 微秒
  static const TimeDiff MINUTES;       ///< 1 分钟 = 60,000,000 微秒
  static const TimeDiff HOURS;         ///< 1 小时 = 3,600,000,000 微秒
  static const TimeDiff DAYS;          ///< 1 天 = 86,400,000,000 微秒

 private:
  TimeDiff span_;  ///< 内部存储的总微秒数
};
}  // namespace time
}  // namespace pickup
