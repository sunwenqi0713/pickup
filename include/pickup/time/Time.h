#pragma once

#include <atomic>
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
 * @see Timespan
 */
class Time {
 public:
  /**
   * 一次性分解的本地时间字段（避免多次调用 getXxx() 产生多次 localtime_r）
   */
  struct BrokenDownTime {
    int year;         ///< 4 位年份，如 2024
    int month;        ///< 0–11
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
   * @param month        0–11
   * @param day          1–31
   * @param hours        0–23
   * @param minutes      0–59
   * @param seconds      0–59
   * @param milliseconds 0–999
   * @param useLocalTime true = 本地时区，false = UTC
   */
  Time(int year, int month, int day, int hours, int minutes,
       int seconds = 0, int milliseconds = 0, bool useLocalTime = true) noexcept;

  Time(const Time&) = default;
  Time& operator=(const Time&) = default;
  ~Time() = default;

  /** @brief 当前时间 */

  /** 返回当前系统时间（可能非单调，不适用于计时） */
  static Time getCurrentTime() noexcept;

  /** 返回自 epoch 以来的毫秒数（系统时钟） */
  static int64_t currentTimeMillis() noexcept;

  /** @brief 从其他类型构造 */

  /** 从 std::chrono::system_clock::time_point 构造 */
  static Time fromTimePoint(std::chrono::system_clock::time_point tp) noexcept;

  /** 解析 ISO-8601 字符串（含时区偏移） */
  static Time fromISO8601(const std::string& iso8601);

  /** 返回基于编译时 __DATE__/__TIME__ 宏的时间 */
  static Time getCompilationDate();

  /** @brief 转换 */

  /** 返回自 epoch 以来的毫秒数 */
  int64_t toMilliseconds() const noexcept { return millisSinceEpoch_; }

  /** 转换为 std::chrono::system_clock::time_point */
  std::chrono::system_clock::time_point toTimePoint() const noexcept;

  /** @brief 字段分解 */

  /**
   * 一次 localtime_r 调用分解全部字段（本地时区）
   * 当需要多个字段时，比逐个调用 getXxx() 高效 N 倍
   */
  BrokenDownTime toBrokenDown() const noexcept;

  int getYear()         const noexcept;  ///< 4 位年份（本地时区）
  int getMonth()        const noexcept;  ///< 0–11（本地时区）
  int getDayOfMonth()   const noexcept;  ///< 1–31（本地时区）
  int getDayOfWeek()    const noexcept;  ///< 0–6，0=周日（本地时区）
  int getDayOfYear()    const noexcept;  ///< 0–365（本地时区）
  int getHours()        const noexcept;  ///< 0–23（本地时区）
  int getMinutes()      const noexcept;  ///< 0–59（本地时区）
  int getSeconds()      const noexcept;  ///< 0–59
  int getMilliseconds() const noexcept;  ///< 0–999（当前秒内的毫秒）

  bool isAfternoon()           const noexcept;  ///< true = PM
  int  getHoursInAmPmFormat()  const noexcept;  ///< 1–12
  bool isDaylightSavingTime()  const noexcept;
  int  getUTCOffsetSeconds()   const noexcept;  ///< 本地时区与 UTC 的偏移秒数

  std::string getTimeZone()                              const;
  std::string getMonthName(bool threeLetterVersion)      const;
  std::string getWeekdayName(bool threeLetterVersion)    const;
  std::string getUTCOffsetString(bool includeDivider)    const;

  /** @brief 格式化 */

  /**
   * 格式化为字符串（使用单次 localtime_r，比逐字段访问高效）
   */
  std::string toString(bool includeDate, bool includeTime,
                       bool includeSeconds = true, bool use24HourClock = false) const;

  /** 使用 strftime 格式字符串格式化 */
  std::string formatted(const std::string& format) const;

  /** ISO-8601 格式（含本地时区偏移） */
  std::string toISO8601(bool includeDividerCharacters) const;

  /** @brief 静态工具 */

  static std::string getWeekdayName(int dayNumber, bool threeLetterVersion);
  static std::string getMonthName(int monthNumber, bool threeLetterVersion);

  /** @brief 单调时钟 */

  /**
   * 自系统启动以来的毫秒数（32 位，约 49.7 天后回绕）
   * 单调递增，不受系统时钟调整影响，适用于计时
   */
  static uint32_t getMillisecondCounter() noexcept;

  /** getMillisecondCounter() 的快速近似版本（无系统调用，误差 <100ms） */
  static uint32_t getApproximateMillisecondCounter() noexcept;

  /** 等待直到 getMillisecondCounter() 达到目标值 */
  static void waitForMillisecondCounter(uint32_t targetTime) noexcept;

  /** 高精度计时器（纳秒级分辨率） */
  static double getMillisecondCounterHiRes() noexcept;

  /** 返回高分辨率计数器当前值 */
  static int64_t getHighResolutionTicks() noexcept;

  /**
   * 高分辨率计数器的每秒 tick 数（编译期常量）
   */
  static constexpr int64_t getHighResolutionTicksPerSecond() noexcept {
    return std::chrono::high_resolution_clock::period::den /
           std::chrono::high_resolution_clock::period::num;
  }

  static double  highResolutionTicksToSeconds(int64_t ticks) noexcept;
  static int64_t secondsToHighResolutionTicks(double seconds) noexcept;

  /** @brief 系统时钟设置 */

  /** 尝试将系统时钟设置为当前对象的时间（通常需要管理员权限） */
  bool setSystemTimeToThisTime() const;

  /** @brief 运算符 */

  Time& operator+=(Timespan delta) noexcept;
  Time& operator-=(Timespan delta) noexcept;

 private:
  int64_t millisSinceEpoch_{0};
  static std::atomic<uint32_t> lastMSCounterValue_;
};

Time operator+(Time time, Timespan delta) noexcept;
Time operator+(Timespan delta, Time time) noexcept;
Time operator-(Time time, Timespan delta) noexcept;
Timespan operator-(Time time1, Time time2) noexcept;

bool operator==(Time time1, Time time2) noexcept;
bool operator!=(Time time1, Time time2) noexcept;
bool operator< (Time time1, Time time2) noexcept;
bool operator<=(Time time1, Time time2) noexcept;
bool operator> (Time time1, Time time2) noexcept;
bool operator>=(Time time1, Time time2) noexcept;

}  // namespace time
}  // namespace pickup
