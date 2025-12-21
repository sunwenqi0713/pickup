#pragma once

#include <atomic>  // **添加这个头文件**
#include <cstdint>
#include <string>

namespace pickup {
namespace time {

/**
 * 表示一个绝对的日期和时间
 *
 * 内部以毫秒精度存储时间，自 Unix 'epoch'（UTC 1970年1月1日午夜）以来
 *
 * @see Timespan
 *
 * @tags{Core}
 */
class Timespan;  // 前向声明
class Time {
 public:
  /**
   * 创建一个 Time 对象
   * 这个默认构造函数创建一个 UTC 时间 1970年1月1日午夜的时间（内部表示为0毫秒）
   * 要创建一个表示当前时间的 Time 对象，请使用 getCurrentTime() 方法
   * @see getCurrentTime
   */
  Time() = default;

  /**
   * 基于毫秒数创建一个时间
   * 要创建一个设置为当前时间的 Time 对象，请使用 getCurrentTime() 方法
   *
   * @param millisecondsSinceEpoch   自 Unix 'epoch'（UTC 1970年1月1日午夜）以来的毫秒数
   * @see getCurrentTime, currentTimeMillis
   */
  explicit Time(int64_t millisecondsSinceEpoch) noexcept;

  /**
   * 从一组日期组件创建时间
   *
   * @param year             年份，4位格式，例如 2004
   * @param month            月份，范围 0 到 11
   * @param day              日期，范围 1 到 31
   * @param hours            24小时制的小时数，0 到 23
   * @param minutes          分钟数 0 到 59
   * @param seconds          秒数 0 到 59
   * @param milliseconds     毫秒数 0 到 999
   * @param useLocalTime     如果为 true，假设输入是此机器的本地时区时间
   *                         如果为 false，假设输入是 UTC 时间。
   */
  Time(int year, int month, int day, int hours, int minutes, int seconds = 0, int milliseconds = 0,
       bool useLocalTime = true) noexcept;

  Time(const Time&) = default;
  ~Time() = default;

  Time& operator=(const Time&) = default;

  /**
   * 返回一个设置为当前系统时间的 Time 对象
   *
   * 这可能不是单调的，因为系统时间可以随时更改
   * 因此不应使用此方法测量时间间隔
   *
   * @see currentTimeMillis
   */
  static Time getCurrentTime() noexcept;

  /**
   * 以毫秒数返回时间
   *
   * @returns 此 Time 对象表示的毫秒数，自 UTC 1970年1月1日午夜以来
   *
   * @see getMilliseconds
   */
  int64_t toMilliseconds() const noexcept { return millisSinceEpoch_; }

  /**
   * 返回年份（在此机器的本地时区）
   * 使用4位格式，例如 2004
   */
  int getYear() const noexcept;

  /**
   * 返回月份数字（在此机器的本地时区）
   * 返回的值范围是 0 到 11
   * @see getMonthName
   */
  int getMonth() const noexcept;

  /**
   * 返回月份名称（在此机器的本地时区）
   * @param threeLetterVersion   如果为 true，返回3字母缩写，例如 "Jan"；如果为 false 返回完整形式，例如 "January"
   * @see getMonth
   */
  std::string getMonthName(bool threeLetterVersion) const;

  /**
   * 返回日期中的天数（在此机器的本地时区）
   * 返回的值范围是 1 到 31
   */
  int getDayOfMonth() const noexcept;

  /**
   * 返回星期几的数字（在此机器的本地时区）
   * 返回的值范围是 0 到 6（0 = 星期日，1 = 星期一，等等）
   */
  int getDayOfWeek() const noexcept;

  /**
   * 返回一年中的第几天（在此机器的本地时区）
   * 返回的值范围是 0 到 365
   */
  int getDayOfYear() const noexcept;

  /**
   * 返回星期几的名称（在此机器的本地时区）
   * @param threeLetterVersion   如果为 true，返回3字母缩写，例如 "Tue"；如果为 false
   *                              返回完整版本，例如 "Tuesday"
   */
  std::string getWeekdayName(bool threeLetterVersion) const;

  /**
   * 返回自午夜以来的小时数（在此机器的本地时区）
   * 这是24小时制格式，范围 0 到 23
   * @see getHoursInAmPmFormat, isAfternoon
   */
  int getHours() const noexcept;

  /**
   * 返回时间是否为下午（在此机器的本地时区）
   * @returns true 表示 "PM"，false 表示 "AM"
   * @see getHoursInAmPmFormat, getHours
   */
  bool isAfternoon() const noexcept;

  /**
   * 以12小时制格式返回小时数（在此机器的本地时区）
   * 这将返回值 1 到 12 - 使用 isAfternoon() 来确定这是下午还是上午
   * @see getHours, isAfternoon
   */
  int getHoursInAmPmFormat() const noexcept;

  /**
   * 返回分钟数，0 到 59（在此机器的本地时区）
   */
  int getMinutes() const noexcept;

  /**
   * 返回秒数，0 到 59
   */
  int getSeconds() const noexcept;

  /**
   * 返回毫秒数，0 到 999
   *
   * 与 toMilliseconds() 不同，此方法只返回当前秒内的毫秒数，而不是自纪元以来的总毫秒数
   *
   * @see toMilliseconds
   */
  int getMilliseconds() const noexcept;

  /**
   * 如果本地时区使用夏令时调整，则返回 true
   */
  bool isDaylightSavingTime() const noexcept;

  /**
   * 返回表示本地时区的3字符字符串
   */
  std::string getTimeZone() const;

  /**
   * 返回本地时区与 UTC 的偏移秒数
   */
  int getUTCOffsetSeconds() const noexcept;

  /**
   * 返回表示本地时区与 UTC 偏移的字符串
   * @returns "+XX:XX", "-XX:XX" 或 "Z"
   * @param includeDividerCharacters  是否在字符串中包含 ":" 分隔符
   */
  std::string getUTCOffsetString(bool includeDividerCharacters) const;

  /**
   * 返回此日期和时间的字符串版本，使用此机器的本地时区
   *
   * 有关格式化日期和时间的更强大方法，请参见 formatted() 方法
   *
   * @param includeDate      是否在字符串中包含日期
   * @param includeTime      是否在字符串中包含时间
   * @param includeSeconds   如果包含时间，此选项决定是否包含秒数
   * @param use24HourClock   如果包含时间，设置是使用 am/pm 还是24小时制表示法
   * @see formatted
   */
  std::string toString(bool includeDate, bool includeTime, bool includeSeconds = true,
                       bool use24HourClock = false) const;

  /** 使用用户定义的格式将此日期/时间转换为字符串
   *
   * 此方法使用 C 的 strftime() 函数将此时间格式化为字符串。为了节省您查找的时间，
   * 以下是 strftime 使用的转义码（其他代码可能在部分平台上有效，但以下是最常见的）：
   *
   * - %a  替换为本地化的缩写星期名称。
   * - %A  替换为本地化的完整星期名称。
   * - %b  替换为本地化的缩写月份名称。
   * - %B  替换为本地化的完整月份名称。
   * - %c  替换为本地化的适当日期和时间表示。
   * - %d  替换为十进制表示的月份中的天数 [01,31]。
   * - %H  替换为24小时制的小时数 [00,23]。
   * - %I  替换为12小时制的小时数 [01,12]。
   * - %j  替换为一年中的天数 [001,366]。
   * - %m  替换为十进制表示的月份 [01,12]。
   * - %M  替换为分钟数 [00,59]。
   * - %p  替换为本地化的等效上午或下午表示。
   * - %S  替换为秒数 [00,60]。
   * - %U  替换为一年中的周数（星期日为一周的第一天）[00,53]。
   * - %w  替换为十进制表示的星期几 [0,6]，0 表示星期日。
   * - %W  替换为一年中的周数（星期一为一周的第一天）[00,53]。新年中第一个星期一之前的所有天被视为在第0周。
   * - %x  替换为本地化的适当日期表示。
   * - %X  替换为本地化的适当时间表示。
   * - %y  替换为不带世纪的年份 [00,99]。
   * - %Y  替换为带世纪的年份。
   * - %Z  替换为时区名称或缩写，如果不存在时区信息则不替换。
   * - %%  替换为 %.
   *
   * @see toString
   */
  std::string formatted(const std::string& format) const;

  /**
   * 返回此日期和时间在 ISO-8601 格式中的完整描述字符串（使用本地时区）
   * @param includeDividerCharacters  是否在字符串中包含 "-" 和 ":" 分隔符
   */
  std::string toISO8601(bool includeDividerCharacters) const;

  /**
   * 解析 ISO-8601 字符串并返回为 Time
   *
   */
  static Time fromISO8601(const std::string& iso8601);

  /** 将 Timespan 添加到此时间 */
  Time& operator+=(Timespan delta) noexcept;
  /** 从此时间中减去 Timespan */
  Time& operator-=(Timespan delta) noexcept;

  /**
   * 尝试设置计算机的时钟
   * @returns 如果成功则返回 true，但根据系统不同，应用程序可能没有足够的权限执行此操作
   */
  bool setSystemTimeToThisTime() const;

  /**
   * 返回星期几的名称
   * @param dayNumber            星期几，0 到 6（0 = 星期日，1 = 星期一，等等）
   * @param threeLetterVersion   如果为 true，返回3字母缩写，例如 "Tue"；如果为 false
   *                             返回完整版本，例如 "Tuesday"
   */
  static std::string getWeekdayName(int dayNumber, bool threeLetterVersion);

  /**
   * 返回月份的名称
   * @param monthNumber      月份，0 到 11
   * @param threeLetterVersion   如果为 true，返回3字母缩写，例如 "Jan"；如果为 false
   *                             返回完整形式，例如 "January"
   */
  static std::string getMonthName(int monthNumber, bool threeLetterVersion);

  /**
   * 返回当前系统时间
   * 返回自 UTC 1970年1月1日午夜以来的毫秒数
   * 根据平台、硬件等的不同，精度应在几毫秒以内
   */
  static int64_t currentTimeMillis() noexcept;

  /**
   * 返回自固定事件（通常是系统启动）以来的毫秒数
   * 此方法返回一个单调递增的值，不受系统时钟更改的影响
   * 根据平台、硬件等的不同，精度应在几毫秒以内
   * 由于是32位返回值，在运行 2^32 秒后会回绕到0，因此请注意这一点
   * 如果需要64位时间，可以使用 currentTimeMillis()
   * @see getApproximateMillisecondCounter
   */
  static uint32_t getMillisecondCounter() noexcept;

  /**
   * 返回自固定事件（通常是系统启动）以来的毫秒数
   * 此方法与 getMillisecondCounter() 功能相同，但返回更精确的值，如果可用则使用更高分辨率的计时器
   *  @see getMillisecondCounter
   */
  static double getMillisecondCounterHiRes() noexcept;

  /**
   * 等待直到 getMillisecondCounter() 达到给定值
   * 此方法将使线程在等待时尽可能高效地休眠
   */
  static void waitForMillisecondCounter(uint32_t targetTime) noexcept;

  /**
   * getMillisecondCounter() 的精度较低但速度更快的版本
   * 此方法将返回 getMillisecondCounter() 最后返回的值，因此不需要进行系统调用，
   * 但精度较低 - 尽管不应偏离正确时间超过100毫秒，对于许多用途仍然足够精确。
   * @see getMillisecondCounter
   */
  static uint32_t getApproximateMillisecondCounter() noexcept;

  /**
   * 返回当前高分辨率计数器的刻度数
   * 这是一个类似于 getMillisecondCounter() 的概念，但具有更高的分辨率
   * @see getHighResolutionTicksPerSecond, highResolutionTicksToSeconds, secondsToHighResolutionTicks
   */
  static int64_t getHighResolutionTicks() noexcept;

  /**
   * 返回高分辨率计数器的分辨率，以每秒刻度数为单位
   * @see getHighResolutionTicks, highResolutionTicksToSeconds, secondsToHighResolutionTicks
   */
  static int64_t getHighResolutionTicksPerSecond() noexcept;

  /**
   * 将高分辨率刻度数转换为秒
   * @see getHighResolutionTicks, getHighResolutionTicksPerSecond, highResolutionTicksToSeconds
   */
  static double highResolutionTicksToSeconds(int64_t ticks) noexcept;

  /**
   * 将秒数转换为高分辨率刻度数
   * @see getHighResolutionTicks, getHighResolutionTicksPerSecond, highResolutionTicksToSeconds
   */
  static int64_t secondsToHighResolutionTicks(double seconds) noexcept;

  /**
   * 返回基于编译此模块时 __DATE__ 宏值的时间
   */
  static Time getCompilationDate();

 private:
  // Helper functions
  static std::tm millisToLocal(int64_t millis) noexcept;
  static std::tm millisToUTC(int64_t millis) noexcept;
  static int getUTCOffsetSecondsFromMillis(int64_t millis) noexcept;
  static int extendedModulo(int64_t value, int modulo) noexcept;
  static std::string formatString(const std::string& format, const std::tm* tm);
  static bool isLeapYear(int year) noexcept;
  static int daysFromJan1(int year, int month) noexcept;
  static int64_t daysFromYear0(int year) noexcept;
  static int64_t daysFrom1970(int year) noexcept;
  static int64_t daysFrom1970(int year, int month) noexcept;
  static int64_t mktime_utc(const std::tm& t) noexcept;
  static std::string getUTCOffsetString(int utcOffsetSeconds, bool includeSemiColon);

 private:
  int64_t millisSinceEpoch_ = 0;
  static std::atomic<uint32_t> lastMSCounterValue_;
};

/** 将 Timespan 添加到 Time */
Time operator+(Time time, Timespan delta) noexcept;
/** 将 Timespan 添加到 Time */
Time operator+(Timespan delta, Time time) noexcept;

/** 从 Time 中减去 Timespan */
Time operator-(Time time, Timespan delta) noexcept;
/** 返回两个时间之间的相对时间差 */
Timespan operator-(Time time1, Time time2) noexcept;

/** 比较两个 Time 对象 */
bool operator==(Time time1, Time time2) noexcept;
/** 比较两个 Time 对象 */
bool operator!=(Time time1, Time time2) noexcept;
/** 比较两个 Time 对象 */
bool operator<(Time time1, Time time2) noexcept;
/** 比较两个 Time 对象 */
bool operator<=(Time time1, Time time2) noexcept;
/** 比较两个 Time 对象 */
bool operator>(Time time1, Time time2) noexcept;
/** 比较两个 Time 对象 */
bool operator>=(Time time1, Time time2) noexcept;

}  // namespace time
}  // namespace pickup