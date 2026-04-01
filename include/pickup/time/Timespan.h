#pragma once

#include <cstdint>
#include <string>

namespace pickup {
namespace time {

/**
 * 相对时间度量
 *
 * 内部以微秒精度的 int64_t 存储，避免 double 浮点累积误差。
 * 可以是正数或负数。
 *
 * 如果需要绝对时间（即日期+时间），请参阅 Time 类
 */
class Timespan {
 public:
  /**
   * 创建 Timespan 对象
   * @param seconds 秒数，可以是正数或负数
   */
  explicit Timespan(double seconds = 0.0) noexcept;

  Timespan(const Timespan&) noexcept = default;
  Timespan& operator=(const Timespan&) noexcept = default;
  ~Timespan() noexcept = default;

  /** 创建表示若干毫秒的 Timespan（精确，无浮点误差） */
  static Timespan milliseconds(int milliseconds) noexcept;
  /** 创建表示若干毫秒的 Timespan（精确，无浮点误差） */
  static Timespan milliseconds(int64_t milliseconds) noexcept;
  /** 创建表示若干秒的 Timespan */
  static Timespan seconds(double seconds) noexcept;
  /** 创建表示若干分钟的 Timespan */
  static Timespan minutes(double numberOfMinutes) noexcept;
  /** 创建表示若干小时的 Timespan */
  static Timespan hours(double numberOfHours) noexcept;
  /** 创建表示若干天的 Timespan */
  static Timespan days(double numberOfDays) noexcept;
  /** 创建表示若干周的 Timespan */
  static Timespan weeks(double numberOfWeeks) noexcept;

  /** 返回微秒数（精确整数，无精度损失） */
  int64_t inMicroseconds() const noexcept { return microseconds_; }

  /** 返回毫秒数 */
  int64_t inMilliseconds() const noexcept;

  /** 返回秒数（double） */
  double inSeconds() const noexcept { return static_cast<double>(microseconds_) / 1e6; }

  /** 返回分钟数 */
  double inMinutes() const noexcept;

  /** 返回小时数 */
  double inHours() const noexcept;

  /** 返回天数 */
  double inDays() const noexcept;

  /** 返回周数 */
  double inWeeks() const noexcept;

  Timespan operator+=(Timespan timeToAdd) noexcept;
  Timespan operator-=(Timespan timeToSubtract) noexcept;
  Timespan operator+=(double secondsToAdd) noexcept;
  Timespan operator-=(double secondsToSubtract) noexcept;

 private:
  /** @brief 工厂方法专用：直接以微秒构造，无浮点转换 */
  explicit Timespan(int64_t microseconds) noexcept : microseconds_(microseconds) {}

  int64_t microseconds_{0};  ///< 以微秒存储，精确无浮点误差
};

bool operator==(Timespan t1, Timespan t2) noexcept;
bool operator!=(Timespan t1, Timespan t2) noexcept;
bool operator>(Timespan t1, Timespan t2) noexcept;
bool operator<(Timespan t1, Timespan t2) noexcept;
bool operator>=(Timespan t1, Timespan t2) noexcept;
bool operator<=(Timespan t1, Timespan t2) noexcept;

Timespan operator+(Timespan t1, Timespan t2) noexcept;
Timespan operator-(Timespan t1, Timespan t2) noexcept;

}  // namespace time
}  // namespace pickup
