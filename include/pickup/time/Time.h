#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>

#include "pickup/time/Timespan.h"
#include "pickup/time/Timestamp.h"

namespace pickup {
namespace time {

/** @brief 星期（0 = 周日，与 std::tm::tm_wday 一致） */
enum class Weekday {
  Sunday = 0,
  Monday,
  Tuesday,
  Wednesday,
  Thursday,
  Friday,
  Saturday,
};

class UtcTime;
class LocalTime;

/**
 * @brief 日历日期与时间（分解为年/月/日/时/分/秒及亚秒字段 + 星期）
 *
 * 支持与 Timestamp 互转、ISO-8601 解析/序列化、strftime 及可读字符串格式化。
 *
 * 本基类的字段是不带时区语义的“挂钟读数”，格式化时按 UTC 处理；需要明确的
 * UTC 或本地时间语义时应使用派生类 UtcTime / LocalTime。比较运算逐级比较日历
 * 字段，比较的是挂钟表示而非绝对时刻——不同时区、指向同一时刻的两个 Time
 * 并不相等。
 *
 * 可表示范围：year ∈ [1678, 2262]（受 int64 纳秒 Timestamp 限制）。
 *
 * @note 非线程安全。
 */
class Time {
 public:
  /** @brief 初始化为 epoch（1970-01-01T00:00:00） */
  Time() noexcept;

  /**
   * @brief 从日历字段构造
   * @param year        年（1678–2262）
   * @param month       月（1–12）
   * @param day         日（1–31）
   * @param hour        时（0–23）
   * @param minute      分（0–59）
   * @param second      秒（0–59）
   * @param millisecond 毫秒（0–999）
   * @param microsecond 微秒（0–999）
   * @param nanosecond  纳秒（0–999）
   * @throws std::invalid_argument 任一字段越界
   */
  explicit Time(int year, int month, int day, int hour = 0, int minute = 0,
                int second = 0, int millisecond = 0, int microsecond = 0, int nanosecond = 0);

  Time(const Time&) noexcept = default;
  Time(Time&&) noexcept = default;
  virtual ~Time() noexcept = default;

  Time& operator=(const Timestamp& timestamp) { return operator=(Time(timestamp)); }
  Time& operator=(const Time&) noexcept = default;
  Time& operator=(Time&&) noexcept = default;

  // 偏移运算经由 UTC 时刻进行，结果为按 UTC 分解的 Time
  Time& operator+=(const Timespan& offset) { return operator=(Time(utcstamp() + offset)); }
  Time& operator-=(const Timespan& offset) { return operator=(Time(utcstamp() - offset)); }

  friend Time operator+(const Time& time, const Timespan& offset) { return Time(time.utcstamp() + offset); }
  friend Time operator+(const Timespan& offset, const Time& time) { return Time(time.utcstamp() + offset); }
  friend Time operator-(const Time& time, const Timespan& offset) { return Time(time.utcstamp() - offset); }
  friend Timespan operator-(const Time& time1, const Time& time2) { return time1.utcstamp() - time2.utcstamp(); }

  friend bool operator==(const Time& t1, const Time& t2) noexcept;
  friend bool operator!=(const Time& t1, const Time& t2) noexcept;
  friend bool operator<(const Time& t1, const Time& t2) noexcept;
  friend bool operator>(const Time& t1, const Time& t2) noexcept;
  friend bool operator<=(const Time& t1, const Time& t2) noexcept;
  friend bool operator>=(const Time& t1, const Time& t2) noexcept;

  /** @brief 转换为 std::chrono::system_clock 时间点 */
  std::chrono::system_clock::time_point chrono() const { return utcstamp().chrono(); }

  int     year()        const noexcept { return year_; }         ///< 年
  int     month()       const noexcept { return month_; }        ///< 月（1–12）
  Weekday weekday()     const noexcept { return static_cast<Weekday>(weekday_); }  ///< 星期
  int     day()         const noexcept { return day_; }          ///< 日（1–31）
  int     hour()        const noexcept { return hour_; }         ///< 时（0–23）
  int     minute()      const noexcept { return minute_; }       ///< 分（0–59）
  int     second()      const noexcept { return second_; }       ///< 秒（0–59）
  int     millisecond() const noexcept { return millisecond_; }  ///< 毫秒（0–999）
  int     microsecond() const noexcept { return microsecond_; }  ///< 微秒（0–999）
  int     nanosecond()  const noexcept { return nanosecond_; }   ///< 纳秒（0–999）

  /** @brief 将字段按 UTC 解释得到的时刻 */
  UtcTimestamp utcstamp() const;
  /** @brief 将字段按本地时区解释得到的时刻 */
  LocalTimestamp localstamp() const;

  /**
   * @brief 使用 strftime 格式串格式化（依据本对象字段，不做时区换算）
   *
   * 在标准 strftime 格式符之外，扩展支持三个亚秒 token（取小数秒的前 N 位）：
   *   - %3f → 毫秒（3 位，如 "007"）
   *   - %6f → 微秒（6 位）
   *   - %9f → 纳秒（9 位）
   * 这些 token 在调用 strftime 前被替换；%% 仍表示字面量 '%'，其后的 3f/6f/9f
   * 按普通文本处理，不会被识别为 token。
   *
   * @param format 格式字符串，如 "%Y-%m-%d %H:%M:%S.%3f"
   */
  std::string formatted(const std::string& format) const;

  /**
   * @brief 序列化为 ISO-8601 字符串（含毫秒与时区偏移）
   *
   * @param includeDividers true 输出扩展格式 "2024-01-15T10:30:00.123+08:00"；
   *                        false 输出紧凑格式 "20240115T103000.123+0800"。
   * 时区偏移：UtcTime 输出 "Z"，其余按本地时区偏移。
   * @note 小数秒仅输出到毫秒（三位），微秒/纳秒分量不输出。
   */
  std::string toISO8601(bool includeDividers) const;

  /**
   * @brief 格式化为可读字符串
   * @param includeDate    含日期部分（"15 Jan 2024"）
   * @param includeTime    含时间部分
   * @param includeSeconds 时间部分是否含秒
   * @param use24HourClock true 为 24 小时制，false 为 12 小时制（带 am/pm）
   */
  std::string toString(bool includeDate, bool includeTime,
                       bool includeSeconds = true, bool use24HourClock = true) const;

  /** @brief 月份名。month：1–12；abbreviated=true 返回 "Jan"，否则 "January" */
  static std::string getMonthName(int month, bool abbreviated);
  /** @brief 星期名。day：0–6（0=周日）；abbreviated=true 返回 "Mon"，否则 "Monday" */
  static std::string getWeekdayName(int day, bool abbreviated);

  /**
   * @brief 解析 ISO-8601 字符串为绝对时刻（UtcTime）
   *
   * 支持扩展格式 "2024-01-15T10:30:00[.fff][Z|±HH:MM]" 与紧凑格式
   * "20240115T103000[.fff][Z|±HHMM]"。缺省时区视为 UTC。
   * @note 小数秒按三位毫秒解析（fff）；解析失败或字段越界时返回 epoch。
   */
  static UtcTime fromISO8601(const std::string& iso8601);

  /** @brief epoch 日期时间（1970-01-01T00:00:00） */
  static Time epoch() { return Time(1970, 1, 1); }

  /** @brief 交换两个实例 */
  void swap(Time& other) noexcept;

 protected:
  int year_;
  int month_;
  int weekday_;
  int day_;
  int hour_;
  int minute_;
  int second_;
  int millisecond_;
  int microsecond_;
  int nanosecond_;

  /** @brief 由 Timestamp 分解构造（按 UTC 分解）；派生类可覆盖具体时区分解 */
  explicit Time(const Timestamp& timestamp);

  /**
   * @brief 本对象相对 UTC 的偏移秒数（供 ISO-8601 输出）
   * @details 基类与 UtcTime 的字段视为 UTC（偏移 0，输出 "Z"）；LocalTime 覆盖
   *          为本地时区偏移。因基类由 Timestamp 分解时按 UTC 解释，故算术运算
   *          得到的 Time 也以 UTC 标注，避免时刻被错误偏移。
   */
  virtual int utcOffsetSeconds() const;
};

/** @brief UTC 日期时间 */
class UtcTime : public Time {
 public:
  using Time::Time;
  using Time::chrono;

  /** @brief 以当前 UTC 时刻初始化 */
  UtcTime() : UtcTime(UtcTimestamp()) {}
  /** @brief 由时刻分解为 UTC 字段 */
  explicit UtcTime(const Timestamp& timestamp);
  /** @brief 由任意 std::chrono 时间点分解为 UTC 字段 */
  template <class Clock, class Duration>
  explicit UtcTime(const std::chrono::time_point<Clock, Duration>& time_point) : UtcTime(Timestamp(time_point)) {}
  /** @brief 由另一个 Time 复制字段 */
  UtcTime(const Time& time) noexcept : Time(time) {}
  /** @brief 由本地时间转换为等价的 UTC 时间 */
  UtcTime(const LocalTime& time);
};

/** @brief 本地时区日期时间 */
class LocalTime : public Time {
 public:
  using Time::Time;
  using Time::chrono;

  /** @brief 以当前本地时刻初始化 */
  LocalTime() : LocalTime(UtcTimestamp()) {}
  /** @brief 由时刻分解为本地字段 */
  explicit LocalTime(const Timestamp& timestamp);
  /** @brief 由任意 std::chrono 时间点分解为本地字段 */
  template <class Clock, class Duration>
  explicit LocalTime(const std::chrono::time_point<Clock, Duration>& time_point) : LocalTime(Timestamp(time_point)) {}
  /** @brief 由另一个 Time 复制字段 */
  LocalTime(const Time& time) noexcept : Time(time) {}
  /** @brief 由 UTC 时间转换为等价的本地时间 */
  LocalTime(const UtcTime& time);

 protected:
  int utcOffsetSeconds() const override;
};

inline void swap(Time& t1, Time& t2) noexcept { t1.swap(t2); }

}  // namespace time
}  // namespace pickup