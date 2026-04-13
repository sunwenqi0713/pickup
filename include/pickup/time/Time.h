#pragma once

#include <chrono>
#include <cstdint>
#include <string>

namespace pickup {
namespace time {

class Timespan;

/**
 * 表示一个绝对的日期和时间
 *
 * 内部以毫秒精度存储，自 Unix epoch（UTC 1970-01-01 00:00:00）起算。
 *
 * 时区说明：字段分解（getYear/getMonth/... 及 toBrokenDown）均使用本地时区。
 * month 字段遵循 std::tm 惯例，范围为 0–11（0=一月）。
 *
 * @see Timespan
 */
class Time {
 public:
  /**
   * 一次性分解的本地时间字段
   *
   * 当需要多个字段时，通过 toBrokenDown() 获取，比逐个调用 getXxx() 高效。
   * month 范围 0–11（与 std::tm 一致）。
   */
  struct BrokenDownTime {
    int year;         ///< 4 位年份，如 2024
    int month;        ///< 0–11（0 = 一月）
    int day;          ///< 1–31
    int hours;        ///< 0–23
    int minutes;      ///< 0–59
    int seconds;      ///< 0–59
    int milliseconds; ///< 0–999
  };

  /**
   * 默认构造：UTC 1970-01-01 00:00:00
   * @see getCurrentTime
   */
  Time() = default;

  /**
   * 从自 epoch 以来的毫秒数构造
   */
  explicit Time(int64_t millisecondsSinceEpoch) noexcept;

  /**
   * 从日期/时间组件构造
   *
   * @param year         4 位年份
   * @param month        0–11（0 = 一月）
   * @param day          1–31
   * @param hours        0–23
   * @param minutes      0–59
   * @param seconds      0–59
   * @param milliseconds 0–999
   * @param useLocalTime true = 按本地时区解释，false = 按 UTC 解释
   */
  Time(int year, int month, int day, int hours, int minutes,
       int seconds = 0, int milliseconds = 0, bool useLocalTime = true) noexcept;

  Time(const Time&) = default;
  Time& operator=(const Time&) = default;
  ~Time() = default;

  /** 返回当前系统时间（system_clock，非单调，不适用于计时） */
  static Time getCurrentTime() noexcept;

  /** 返回自 epoch 以来的毫秒数（system_clock） */
  static int64_t currentTimeMillis() noexcept;

  /** 从 std::chrono::system_clock::time_point 构造 */
  static Time fromTimePoint(std::chrono::system_clock::time_point tp) noexcept;

  /**
   * 解析 ISO-8601 字符串
   *
   * 支持格式：
   *   - 扩展格式：2024-01-15T10:30:00[.123][Z|+HH:MM|-HH:MM]
   *   - 紧凑格式：20240115T103000[.123][Z|+HHMM|-HHMM]
   *
   * 解析失败时返回 epoch（1970-01-01T00:00:00Z）。
   */
  static Time fromISO8601(const std::string& iso8601);

  /** 返回自 epoch 以来的毫秒数 */
  int64_t toMilliseconds() const noexcept { return millisSinceEpoch_; }

  /** 转换为 std::chrono::system_clock::time_point */
  std::chrono::system_clock::time_point toTimePoint() const noexcept;

  /**
   * 单次 localtime_r 调用分解全部字段
   * 需要多个字段时应优先使用此方法，避免多次系统调用
   */
  BrokenDownTime toBrokenDown() const noexcept;

  int getYear()         const noexcept;  ///< 4 位年份
  int getMonth()        const noexcept;  ///< 0–11（0 = 一月）
  int getDayOfMonth()   const noexcept;  ///< 1–31
  int getDayOfWeek()    const noexcept;  ///< 0–6（0 = 周日）
  int getDayOfYear()    const noexcept;  ///< 0–365
  int getHours()        const noexcept;  ///< 0–23
  int getMinutes()      const noexcept;  ///< 0–59
  int getSeconds()      const noexcept;  ///< 0–59
  int getMilliseconds() const noexcept;  ///< 0–999（当前秒内的毫秒）

  int         getUTCOffsetSeconds()                   const noexcept;  ///< 本地时区与 UTC 的偏移秒数
  std::string getUTCOffsetString(bool includeDivider) const;            ///< 如 "+08:00" 或 "+0800"，UTC 返回 "Z"
  std::string getTimeZone()                           const;            ///< 时区名，如 "CST"

  /**
   * 格式化为可读字符串（单次 localtime_r）
   *
   * @param includeDate     包含日期部分（如 "15 Jan 2024"）
   * @param includeTime     包含时间部分
   * @param includeSeconds  时间部分是否包含秒
   * @param use24HourClock  true = 24 小时制，false = 12 小时制（带 am/pm）
   */
  std::string toString(bool includeDate, bool includeTime,
                       bool includeSeconds = true, bool use24HourClock = false) const;

  /** 使用 strftime 格式字符串格式化（本地时区） */
  std::string formatted(const std::string& format) const;

  /** 序列化为 ISO-8601 字符串（含本地时区偏移） */
  std::string toISO8601(bool includeDividerCharacters) const;

  static std::string getMonthName(int monthNumber, bool abbreviated);    ///< monthNumber: 0–11；abbreviated=true 返回 "Jan"，false 返回 "January"
  static std::string getWeekdayName(int dayNumber, bool abbreviated);    ///< dayNumber: 0–6；abbreviated=true 返回 "Mon"，false 返回 "Monday"

  /**
   * 自系统启动以来的毫秒数（int64_t，无回绕问题）
   *
   * 基于 steady_clock，单调递增，不受系统时钟调整影响。
   * 适用于计时和性能测量，不适用于表示绝对时刻。
   */
  static int64_t getMillisecondCounter() noexcept;

  /**
   * 高精度计时器，返回毫秒（纳秒级分辨率）
   *
   * 适用于微基准测试；一般计时优先使用 getMillisecondCounter()。
   */
  static double getMillisecondCounterHiRes() noexcept;

  Time& operator+=(Timespan delta) noexcept;
  Time& operator-=(Timespan delta) noexcept;

 private:
  int64_t millisSinceEpoch_{0};
};

Time     operator+(Time time, Timespan delta) noexcept;
Time     operator+(Timespan delta, Time time) noexcept;
Time     operator-(Time time, Timespan delta) noexcept;
Timespan operator-(Time time1, Time time2)   noexcept;

bool operator==(Time time1, Time time2) noexcept;
bool operator!=(Time time1, Time time2) noexcept;
bool operator< (Time time1, Time time2) noexcept;
bool operator<=(Time time1, Time time2) noexcept;
bool operator> (Time time1, Time time2) noexcept;
bool operator>=(Time time1, Time time2) noexcept;

}  // namespace time
}  // namespace pickup
