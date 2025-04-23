#include "pickup/time/DateTime.h"

#include <cassert>
#include <cmath>

namespace pickup {
namespace time {

DateTime::DateTime() : DateTime(1970, 1, 1) {}

DateTime::DateTime(const Timestamp& timestamp, bool useLocalTime) {
  std::tm tm = useLocalTime ? DateTime::millisToLocal(timestamp.epochMilliseconds())
                            : DateTime::millisToUTC(timestamp.epochMilliseconds());
  int ms = static_cast<int>(timestamp.epochMilliseconds() % 1000);
  int us = static_cast<int>(timestamp.epochMicroseconds() % 1000);
  year_ = tm.tm_year + 1900;
  month_ = tm.tm_mon + 1;
  day_ = tm.tm_mday;
  hour_ = tm.tm_hour;
  minute_ = tm.tm_min;
  second_ = tm.tm_sec;
  millisecond_ = ms;
  microsecond_ = us;
  assert(isValid(year_, month_, day_, hour_, minute_, second_, millisecond_, microsecond_));
}

DateTime::DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
    : year_(year),
      month_(month),
      day_(day),
      hour_(hour),
      minute_(minute),
      second_(second),
      millisecond_(millisecond),
      microsecond_(microsecond) {
  assert(isValid(year, month, day, hour, minute, second, millisecond, microsecond));
}

DateTime::DateTime(const DateTime& dateTime)
    : year_(dateTime.year_),
      month_(dateTime.month_),
      day_(dateTime.day_),
      hour_(dateTime.hour_),
      minute_(dateTime.minute_),
      second_(dateTime.second_),
      millisecond_(dateTime.millisecond_),
      microsecond_(dateTime.microsecond_) {}

DateTime::~DateTime() {}

DateTime& DateTime::operator=(const DateTime& dateTime) {
  if (this != &dateTime) {
    year_ = dateTime.year_;
    month_ = dateTime.month_;
    day_ = dateTime.day_;
    hour_ = dateTime.hour_;
    minute_ = dateTime.minute_;
    second_ = dateTime.second_;
    millisecond_ = dateTime.millisecond_;
    microsecond_ = dateTime.microsecond_;
  }
  return *this;
}

DateTime& DateTime::operator=(const Timestamp& timestamp) { return *this; }

DateTime& DateTime::assign(int year, int month, int day, int hour, int minute, int second, int millisecond,
                           int microsecond) {
  assert(isValid(year, month, day, hour, minute, second, millisecond, microsecond));
  year_ = year;
  month_ = month;
  day_ = day;
  hour_ = hour;
  minute_ = minute;
  second_ = second;
  millisecond_ = millisecond;
  microsecond_ = microsecond;

  return *this;
}

void DateTime::swap(DateTime& datetime) {
  std::swap(year_, datetime.year_);
  std::swap(month_, datetime.month_);
  std::swap(day_, datetime.day_);
  std::swap(hour_, datetime.hour_);
  std::swap(minute_, datetime.minute_);
  std::swap(second_, datetime.second_);
  std::swap(millisecond_, datetime.millisecond_);
  std::swap(microsecond_, datetime.microsecond_);
}

DateTime DateTime::now() {
  Timestamp now;
  std::tm tm = DateTime::millisToLocal(now.epochMilliseconds());
  int ms = static_cast<int>(now.epochMilliseconds() % 1000);
  int us = static_cast<int>(now.epochMicroseconds() % 1000);
  return DateTime(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ms, us);
}

DateTime DateTime::nowUTC() {
  Timestamp now;
  std::tm tm = DateTime::millisToUTC(now.epochMilliseconds());
  int ms = static_cast<int>(now.epochMilliseconds() % 1000);
  int us = static_cast<int>(now.epochMicroseconds() % 1000);
  return DateTime(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ms, us);
}

// TODO: 需要实现时区转换
Timestamp::TimeVal DateTime::timestamp() const {
  // 验证时间有效性
  if (!isValid(year_, month_, day_, hour_, minute_, second_, millisecond_, microsecond_)) {
    throw std::invalid_argument("Invalid DateTime components");
  }

  // 计算从1970年1月1日到当前日期的总天数
  // 总天数 = 到年初的天数 + 当年已过去的天数（其中 dayOfYear() - 1 即为“已过去的天数”）
  int days = daysSinceEpoch(year_) + dayOfYear() - 1;
  // 计算总微秒数
  Timespan span(days, hour_ - 8, minute_, second_, millisecond_ * 1000 + microsecond_);
  return span.totalMicroseconds();  // 返回总微秒数
}

int DateTime::year() const { return year_; }
int DateTime::month() const { return month_; }
int DateTime::day() const { return day_; }
int DateTime::hour() const { return hour_; }
int DateTime::minute() const { return minute_; }
int DateTime::second() const { return second_; }
int DateTime::millisecond() const { return millisecond_; }
int DateTime::microsecond() const { return microsecond_; }

int DateTime::hourAMPM() const {
  // 12小时制的小时数（1-12）
  return (hour_ % 12 == 0) ? 12 : hour_ % 12;
}

bool DateTime::isAM() const {
  // 判断是否为上午
  return hour_ < 12;
}

bool DateTime::isPM() const {
  // 判断是否为下午
  return hour_ >= 12;
}

int DateTime::dayOfYear() const {
  int doy = 0;
  for (int m = 1; m < month_; ++m) {
    doy += daysInMonth(year_, m);
  }
  doy += day_;
  return doy;
}

int DateTime::dayOfWeek() const {
  // 计算星期几（0-6，0表示星期天）
  int a = (14 - month_) / 12;
  int y = year_ - a;
  int m = month_ + 12 * a - 2;
  return (day_ + y + y / 4 - y / 100 + y / 400 + (31 * m) / 12) % 7;
}

int DateTime::week(int firstDayOfWeek) const {
  assert(firstDayOfWeek >= 0 && firstDayOfWeek <= 6);

  // 计算每周的第一天
  int baseDay = 1;
  while (DateTime(year_, 1, baseDay).dayOfWeek() != firstDayOfWeek) ++baseDay;

  int doy = dayOfYear();
  int offs = baseDay <= 4 ? 0 : 1;
  if (doy < baseDay)
    return offs;
  else
    return (doy - baseDay) / 7 + 1 + offs;
}

bool DateTime::operator==(const DateTime& dateTime) const { return this->timestamp() == dateTime.timestamp(); }
bool DateTime::operator!=(const DateTime& dateTime) const { return this->timestamp() != dateTime.timestamp(); }
bool DateTime::operator<(const DateTime& dateTime) const { return this->timestamp() < dateTime.timestamp(); }
bool DateTime::operator<=(const DateTime& dateTime) const { return this->timestamp() <= dateTime.timestamp(); }
bool DateTime::operator>(const DateTime& dateTime) const { return this->timestamp() > dateTime.timestamp(); }
bool DateTime::operator>=(const DateTime& dateTime) const { return this->timestamp() >= dateTime.timestamp(); }

DateTime DateTime::operator+(const Timespan& span) const {
  Timestamp::TimeVal totalMicroseconds = this->timestamp() + span.totalMicroseconds();
  // 使用 UTC 时间戳构造新对象，再转换为本地时间（保持与原始对象一致的时区）
  return DateTime(totalMicroseconds, true);
}

DateTime DateTime::operator-(const Timespan& span) const {
  Timestamp::TimeVal totalMicroseconds = this->timestamp() - span.totalMicroseconds();
  // 使用 UTC 时间戳构造新对象，再转换为本地时间（保持与原始对象一致的时区）
  return DateTime(totalMicroseconds, true);
}

Timespan DateTime::operator-(const DateTime& dateTime) const {
  return Timespan(this->timestamp() - dateTime.timestamp());
}

DateTime& DateTime::operator+=(const Timespan& span) {
  *this = *this + span;  // 使用 operator+ 进行加法运算
  return *this;
}

DateTime& DateTime::operator-=(const Timespan& span) {
  *this = *this - span;  // 使用 operator- 进行减法运算
  return *this;
}

void DateTime::makeUTC(int tzd) { operator-=(Timespan(((Timestamp::TimeDiff)tzd) * Timespan::SECONDS)); }

void DateTime::makeLocal(int tzd) { operator+=(Timespan(((Timestamp::TimeDiff)tzd) * Timespan::SECONDS)); }

bool DateTime::isLeapYear(int year) { return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0); }

int DateTime::daysInMonth(int year, int month) {
  assert(month >= 1 && month <= 12);  // 月份范围检查
  // 每个月的天数（非闰年）
  const int daysInMonthTable[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  // 判断是否为闰年
  if (month == 2 && isLeapYear(year)) {
    return 29;
  } else {
    return daysInMonthTable[month];
  }
}

bool DateTime::isValid(int year, int month, int day, int hour, int minute, int second, int millisecond,
                       int microsecond) {
  // 检查年份范围
  if (year < 0 || year > 9999) {
    return false;
  }
  // 检查月份范围
  if (month < 1 || month > 12) {
    return false;
  }
  // 检查日期范围
  if (day < 1 || day > daysInMonth(year, month)) {
    return false;
  }
  // 检查时间范围
  if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59 || millisecond < 0 ||
      millisecond > 999 || microsecond < 0 || microsecond > 999) {
    return false;
  }

  return true;
}

std::tm DateTime::millisToLocal(Timestamp::TimeVal millis) noexcept {
  std::tm result = {};
#if defined(_WIN32) || defined(_WIN64)
  __time64_t sec = millis / 1000;
  if (_localtime64_s(&result, &sec) != 0) {
    memset(&result, 0, sizeof(result));
  }
#else
  time_t sec = static_cast<time_t>(millis / 1000);
  if (localtime_r(&sec, &result) == nullptr) {
    memset(&result, 0, sizeof(result));
  }
#endif
  return result;
}

std::tm DateTime::millisToUTC(Timestamp::TimeVal millis) noexcept {
  std::tm result = {};
#if defined(_WIN32) || defined(_WIN64)
  __time64_t sec = millis / 1000;
  if (_gmtime64_s(&result, &sec) != 0) {
    memset(&result, 0, sizeof(result));
  }
#else
  time_t sec = static_cast<time_t>(millis / 1000);
  if (gmtime_r(&sec, &result) == nullptr) {
    memset(&result, 0, sizeof(result));
  }
#endif
  return result;
}

int DateTime::computeDaysFrom1AD(int year) const {
  if (year < 1) return 0;  // 无效输入
  int y = year - 1;
  // 总天数 = 平年天数 + 闰年修正。每 4 年加 1 天（闰年），但每 100 年减 1 天，每 400 年加 1 天
  return 365 * y + y / 4 - y / 100 + y / 400;
}

int DateTime::daysSinceEpoch(int year) const {
  // 计算从1970年1月1日到指定年份的天数
  return computeDaysFrom1AD(year) - computeDaysFrom1AD(1970);
}

}  // namespace time
}  // namespace pickup