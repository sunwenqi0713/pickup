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
  printf("tm: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  int ms = static_cast<int>(now.epochMilliseconds() % 1000);
  int us = static_cast<int>(now.epochMicroseconds() % 1000);
  return DateTime(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ms, us);
}

DateTime DateTime::nowUTC() {
  Timestamp now;
  std::tm tm = DateTime::millisToUTC(now.epochMilliseconds());
  printf("tm: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  int ms = static_cast<int>(now.epochMilliseconds() % 1000);
  int us = static_cast<int>(now.epochMicroseconds() % 1000);
  return DateTime(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ms, us);
}

Timestamp::TimeVal DateTime::timestamp() const {
  // 将 DateTime 转换为时间戳（微秒级）
  Timestamp::TimeVal timestamp = static_cast<Timestamp::TimeVal>(year_) * Timespan::DAYS * 365 +
                                 static_cast<Timestamp::TimeVal>(month_) * Timespan::DAYS * daysOfMonth(year_, month_) +
                                 static_cast<Timestamp::TimeVal>(day_) * Timespan::DAYS +
                                 static_cast<Timestamp::TimeVal>(hour_) * Timespan::HOURS +
                                 static_cast<Timestamp::TimeVal>(minute_) * Timespan::MINUTES +
                                 static_cast<Timestamp::TimeVal>(second_) * Timespan::SECONDS +
                                 static_cast<Timestamp::TimeVal>(millisecond_) * Timespan::MILLISECONDS + microsecond_;
  return timestamp;
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
    doy += daysOfMonth(year_, m);
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

bool DateTime::isLeapYear(int year) { return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0); }

int DateTime::daysOfMonth(int year, int month) {
  assert(year >= 0 && year <= 9999);  // 年份范围检查
  assert(month >= 1 && month <= 12);  // 月份范围检查
  // 每个月的天数（非闰年）
  const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  // 判断是否为闰年
  if (month == 2 && isLeapYear(year)) {
    return 29;
  } else {
    return daysInMonth[month - 1];
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
  if (day < 1 || day > daysOfMonth(year, month)) {
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

}  // namespace time
}  // namespace pickup