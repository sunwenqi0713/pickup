#pragma once

#include <array>
#include <chrono>
#include <cstring>  // For memset
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

#include "pickup/time/Timespan.h"
#include "pickup/time/Timestamp.h"

namespace pickup {
namespace time {

/**
 * @brief 表示公历日期时间的类（精度至微秒级）
 *
 * 该类用年、月、日、时、分、秒、毫秒和微秒表示时间点，支持公历日期/UTC时间的相互转换。
 * 支持时间的加减运算、比较操作符和格式化输出。
 * @see http://en.wikipedia.org/wiki/Gregorian_Calendar  公历
 * @see http://en.wikipedia.org/wiki/UTC                 UTC
 * @see http://en.wikipedia.org/wiki/ISO_8601            ISO 8601
 */
class DateTime {
 public:
  /**
   * @brief 月份枚举（1-12）
   */
  enum Months {
    JANUARY = 1,  ///< 一月
    FEBRUARY,     ///< 二月
    MARCH,        ///< 三月
    APRIL,        ///< 四月
    MAY,          ///< 五月
    JUNE,         ///< 六月
    JULY,         ///< 七月
    AUGUST,       ///< 八月
    SEPTEMBER,    ///< 九月
    OCTOBER,      ///< 十月
    NOVEMBER,     ///< 十一月
    DECEMBER      ///< 十二月
  };

  /**
   * @brief 星期枚举（0-6）
   */
  enum DaysOfWeek {
    SUNDAY = 0,  ///< 星期日
    MONDAY,      ///< 星期一
    TUESDAY,     ///< 星期二
    WEDNESDAY,   ///< 星期三
    THURSDAY,    ///< 星期四
    FRIDAY,      ///< 星期五
    SATURDAY     ///< 星期六
  };

  // 创建当前 UTC 时间的 DateTime 对象
  DateTime();

  /**
   * @brief 通过 Timestamp 创建 DateTime 对象
   * @param timestamp 时间戳（UTC 时间）
   */
  DateTime(const Timestamp& timestamp, bool useLocalTime = true);

  /**
   * @brief 通过公历日期创建 DateTime 对象
   * @param year  年份（0-9999）
   * @param month 月份（1-12）
   * @param day   日期（1-31）
   * @param hour  小时（0-23，默认0）
   * @param minute 分钟（0-59，默认0）
   * @param second 秒（0-59，默认0）
   * @param millisecond 毫秒（0-999，默认0）
   * @param microsecond 微秒（0-999，默认0）
   */
  DateTime(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0,
           int microsecond = 0);

  // 拷贝构造函数
  DateTime(const DateTime& dateTime);

  // 析构函数
  ~DateTime();

  // 拷贝赋值操作符
  DateTime& operator=(const DateTime& dateTime);

  // 通过 Timestamp 赋值
  DateTime& operator=(const Timestamp& timestamp);

  /**
   * @brief 设置公历日期时间
   * @param year  年份（0-9999）
   * @param month 月份（1-12）
   * @param day   日期（1-31）
   * @param hour  小时（0-23，默认0）
   * @param minute 分钟（0-59，默认0）
   * @param second 秒（0-59，默认0）
   * @param millisecond 毫秒（0-999，默认0）
   * @param microsecond 微秒（0-999，默认0）
   */
  DateTime& assign(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0,
                   int microseconds = 0);

  // 交换两个 DateTime 对象的值
  void swap(DateTime& dateTime);

  // 获取当前时间的 DateTime 对象
  static DateTime now();

  // 获取当前 UTC 时间的 DateTime 对象
  static DateTime nowUTC();

  /**
   * @brief 转换为 Timestamp 对象
   */
  Timestamp::TimeVal timestamp() const;

  /**
   *  @brief 获取年份 （0-9999）
   */
  int year() const;

  /**
   * @brief 获取月份（1-12）
   */
  int month() const;

  /**
   * @brief 获取当月中的日期（1-31）
   */
  int day() const;

  /**
   * @brief 获取24小时制的小时数（0-23）
   */
  int hour() const;

  /**
   * @brief 获取分钟数（0-59）
   */
  int minute() const;

  /**
   * @brief 获取秒数（0-59）
   */
  int second() const;

  /**
   * @brief 获取毫秒数（0-999）
   */
  int millisecond() const;

  /**
   * @brief 获取微秒数（0-999）
   */
  int microsecond() const;

  /**
   * @brief 获取12小时制的小时数（0-11）
   */
  int hourAMPM() const;

  /**
   * @brief 判断是否为上午（hour < 12）
   */
  bool isAM() const;

  /**
   * @brief 判断是否为下午（hour >= 12）
   */
  bool isPM() const;

  /**
   * @brief 获取一年中的第几天（1-366）
   */
  int dayOfYear() const;

  /**
   * @brief 获取星期几（0=周日，1=周一，...，6=周六）
   */
  int dayOfWeek() const;

  /**
   * @brief 计算一年中的周数（ISO 8601 标准）
   * @param firstDayOfWeek 周起始日（SUNDAY 或 MONDAY）
   * @return 周数（0-53）
   *
   * @note 示例：
   * - 2005 年从周六开始，周1从1月3日（周一）开始，1月1-2日属于周0
   * - 2007 年从周一开始，周1从1月1日开始，没有周0
   */
  int week(int firstDayOfWeek = MONDAY) const;

  // 比较操作符
  bool operator==(const DateTime& dateTime) const;
  bool operator!=(const DateTime& dateTime) const;
  bool operator<(const DateTime& dateTime) const;
  bool operator<=(const DateTime& dateTime) const;
  bool operator>(const DateTime& dateTime) const;
  bool operator>=(const DateTime& dateTime) const;

  // 时间运算操作符
  DateTime operator+(const Timespan& span) const;
  DateTime operator-(const Timespan& span) const;
  Timespan operator-(const DateTime& dateTime) const;
  DateTime& operator+=(const Timespan& span);
  DateTime& operator-=(const Timespan& span);

  // Converts a local time into UTC, by applying the given time zone differential.
  void makeUTC(int tzd);

  // Converts a UTC time into a local time, by applying the given time zone differential.
  void makeLocal(int tzd);

  /**
   * @brief 判断是否为闰年
   * @param year 年份（0-9999）
   */
  static bool isLeapYear(int year);

  /**
   * @brief 获取指定年月中的天数
   * @param year  年份（0-9999）
   * @param month 月份（1-12）
   */
  static int daysOfMonth(int year, int month);

  /**
   * @brief 验证日期时间是否有效
   * @return 所有参数在合法范围内时返回 true
   */
  static bool isValid(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0,
                      int microsecond = 0);

 private:
  /**
   * @brief 计算从公元1年到指定年份的天数
   *
   * @param year
   * @return int
   */
  int computeDaysFrom1AD(int year) const;

  /**
   * @brief 计算从公元1970年到指定年份的天数
   *
   * @param year
   * @return int
   */
  int daysSinceEpoch(int year) const;

  /**
   * @brief 将时间戳转换为本地时间
   *
   * @param millis
   * @return std::tm
   */
  static std::tm millisToLocal(Timestamp::TimeVal millis) noexcept;

  /**
   * @brief 将时间戳转换为 UTC 时间
   *
   * @param millis
   * @return std::tm
   */
  static std::tm millisToUTC(Timestamp::TimeVal millis) noexcept;

 private:
  short year_;         ///< 年份（0-9999）
  short month_;        ///< 月份（1-12）
  short day_;          ///< 日期（1-31）
  short hour_;         ///< 小时（0-23）
  short minute_;       ///< 分钟（0-59）
  short second_;       ///< 秒（0-59）
  short millisecond_;  ///< 毫秒（0-999）
  short microsecond_;  ///< 微秒（0-999）
};

}  // namespace time
}  // namespace pickup