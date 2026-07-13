#include "pickup/time/Time.h"

#include <cstdint>
#include <cstdio>
#include <ctime>
#include <stdexcept>
#include <string>
#include <tuple>

namespace pickup {
namespace time {

namespace {

// 允许的年份范围（受 int64 纳秒 Timestamp 表示范围限制）
constexpr int kMinYear = 1678;
constexpr int kMaxYear = 2262;

constexpr const char* kMonthShort[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                       "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
constexpr const char* kMonthLong[] = {"January", "February", "March",     "April",
                                      "May",     "June",     "July",      "August",
                                      "September", "October", "November", "December"};
constexpr const char* kWeekdayShort[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
constexpr const char* kWeekdayLong[] = {"Sunday",   "Monday", "Tuesday", "Wednesday",
                                        "Thursday", "Friday", "Saturday"};

bool isLeapYear(int year) noexcept {
  return (year % 400 == 0) || ((year % 100 != 0) && (year % 4 == 0));
}

// Sakamoto 算法，返回 0–6（0 = 周日）
int weekdayOf(int year, int month, int day) noexcept {
  static const int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
  int y = year;
  if (month < 3) --y;
  return (y + y / 4 - y / 100 + y / 400 + t[month - 1] + day) % 7;
}

int dayOfYear(int year, int month, int day) noexcept {
  static const int cum[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
  int doy = cum[month - 1] + day - 1;
  if (month > 2 && isLeapYear(year)) ++doy;
  return doy;
}

// 向下取整除法（用于处理负时间戳的秒/亚秒分解）
int64_t floorDiv(int64_t a, int64_t b) noexcept {
  int64_t q = a / b;
  if ((a % b != 0) && ((a < 0) != (b < 0))) --q;
  return q;
}

std::tm toTm(int year, int month, int day, int hour, int minute, int second) noexcept {
  std::tm t{};
  t.tm_year = year - 1900;
  t.tm_mon = month - 1;
  t.tm_mday = day;
  t.tm_hour = hour;
  t.tm_min = minute;
  t.tm_sec = second;
  t.tm_isdst = -1;
  return t;
}

std::tm gmtimeOf(time_t seconds) noexcept {
  std::tm r{};
#if defined(_WIN32) || defined(_WIN64)
  gmtime_s(&r, &seconds);
#else
  gmtime_r(&seconds, &r);
#endif
  return r;
}

std::tm localtimeOf(time_t seconds) noexcept {
  std::tm r{};
#if defined(_WIN32) || defined(_WIN64)
  localtime_s(&r, &seconds);
#else
  localtime_r(&seconds, &r);
#endif
  return r;
}

// 格式化 UTC 偏移，如 "+08:00" / "-0500" / "Z"
std::string formatOffset(int offsetSeconds, bool includeDivider) {
  if (offsetSeconds == 0) return "Z";
  const char sign = (offsetSeconds < 0) ? '-' : '+';
  int absMinutes = (offsetSeconds < 0 ? -offsetSeconds : offsetSeconds) / 60;
  char buffer[8];
  if (includeDivider)
    std::snprintf(buffer, sizeof(buffer), "%c%02d:%02d", sign, absMinutes / 60, absMinutes % 60);
  else
    std::snprintf(buffer, sizeof(buffer), "%c%02d%02d", sign, absMinutes / 60, absMinutes % 60);
  return buffer;
}

// 预替换亚秒扩展 token（%3f/%6f/%9f）为小数秒的前 N 位数字，其余格式符原样
// 留给 strftime。%% 视为 strftime 的字面量转义，其后的字符不参与 token 识别。
std::string expandSubsecond(const std::string& format, int millisecond,
                            int microsecond, int nanosecond) {
  const long long subNs =
      millisecond * 1000000LL + microsecond * 1000LL + nanosecond;

  std::string out;
  out.reserve(format.size() + 16);
  for (size_t i = 0; i < format.size(); ++i) {
    if (format[i] != '%') {
      out += format[i];
      continue;
    }
    const char next = (i + 1 < format.size()) ? format[i + 1] : '\0';
    if (next == '%') {  // %% 转义整体保留，跳过其后字符的 token 识别
      out += "%%";
      ++i;
      continue;
    }
    if ((next == '3' || next == '6' || next == '9') && (i + 2 < format.size()) &&
        format[i + 2] == 'f') {
      const int digits = next - '0';
      const long long value = subNs / (digits == 3 ? 1000000 : digits == 6 ? 1000 : 1);
      char buf[16];
      std::snprintf(buf, sizeof(buf), "%0*lld", digits, value);
      out += buf;
      i += 2;
      continue;
    }
    out += '%';  // 其它格式符：保留 '%'，后续字符照常交给 strftime
  }
  return out;
}

void validateComponent(bool ok, const char* message) {
  if (!ok) throw std::invalid_argument(message);
}

}  // namespace

Time::Time() noexcept
    : year_(1970), month_(1), weekday_(weekdayOf(1970, 1, 1)), day_(1),
      hour_(0), minute_(0), second_(0), millisecond_(0), microsecond_(0), nanosecond_(0) {}

Time::Time(int year, int month, int day, int hour, int minute, int second,
           int millisecond, int microsecond, int nanosecond) {
  validateComponent(year >= kMinYear && year <= kMaxYear, "Year is out of representable range [1678, 2262]!");
  validateComponent(month >= 1 && month <= 12, "Month value is limited in range from 1 to 12!");
  validateComponent(day >= 1 && day <= 31, "Day value is limited in range from 1 to 31!");
  validateComponent(hour >= 0 && hour <= 23, "Hour value is limited in range from 0 to 23!");
  validateComponent(minute >= 0 && minute <= 59, "Minute value is limited in range from 0 to 59!");
  validateComponent(second >= 0 && second <= 59, "Second value is limited in range from 0 to 59!");
  validateComponent(millisecond >= 0 && millisecond <= 999, "Millisecond value is limited in range from 0 to 999!");
  validateComponent(microsecond >= 0 && microsecond <= 999, "Microsecond value is limited in range from 0 to 999!");
  validateComponent(nanosecond >= 0 && nanosecond <= 999, "Nanosecond value is limited in range from 0 to 999!");

  year_ = year;
  month_ = month;
  weekday_ = weekdayOf(year, month, day);
  day_ = day;
  hour_ = hour;
  minute_ = minute;
  second_ = second;
  millisecond_ = millisecond;
  microsecond_ = microsecond;
  nanosecond_ = nanosecond;
}

Time::Time(const Timestamp& timestamp) {
  const int64_t total = timestamp.total();
  const int64_t seconds = floorDiv(total, 1000000000ll);
  const int64_t sub = total - seconds * 1000000000ll;

  const std::tm r = gmtimeOf(static_cast<time_t>(seconds));
  year_ = r.tm_year + 1900;
  month_ = r.tm_mon + 1;
  weekday_ = r.tm_wday;
  day_ = r.tm_mday;
  hour_ = r.tm_hour;
  minute_ = r.tm_min;
  second_ = r.tm_sec;
  millisecond_ = static_cast<int>(sub / 1000000);
  microsecond_ = static_cast<int>((sub / 1000) % 1000);
  nanosecond_ = static_cast<int>(sub % 1000);
}

UtcTimestamp Time::utcstamp() const {
  std::tm t = toTm(year_, month_, day_, hour_, minute_, second_);
#if defined(_WIN32) || defined(_WIN64)
  const time_t seconds = _mkgmtime(&t);
#else
  const time_t seconds = timegm(&t);
#endif
  const int64_t ns = static_cast<int64_t>(seconds) * 1000000000ll +
                     static_cast<int64_t>(millisecond_) * 1000000ll +
                     static_cast<int64_t>(microsecond_) * 1000ll + nanosecond_;
  return UtcTimestamp(Timestamp(ns));
}

LocalTimestamp Time::localstamp() const {
  std::tm t = toTm(year_, month_, day_, hour_, minute_, second_);
  const time_t seconds = std::mktime(&t);
  const int64_t ns = static_cast<int64_t>(seconds) * 1000000000ll +
                     static_cast<int64_t>(millisecond_) * 1000000ll +
                     static_cast<int64_t>(microsecond_) * 1000ll + nanosecond_;
  return LocalTimestamp(Timestamp(ns));
}

int Time::utcOffsetSeconds() const {
  // 基类字段视为 UTC（由 Timestamp 分解、算术运算结果均为 UTC）
  return 0;
}

std::string Time::formatted(const std::string& format) const {
  const std::string expanded = expandSubsecond(format, millisecond_, microsecond_, nanosecond_);
  std::tm t = toTm(year_, month_, day_, hour_, minute_, second_);
  t.tm_wday = weekday_;
  t.tm_yday = dayOfYear(year_, month_, day_);
  char buffer[256];
  const size_t n = std::strftime(buffer, sizeof(buffer), expanded.c_str(), &t);
  return (n > 0) ? std::string(buffer, n) : std::string{};
}

std::string Time::toISO8601(bool includeDividers) const {
  char buffer[64];
  if (includeDividers) {
    std::snprintf(buffer, sizeof(buffer), "%04d-%02d-%02dT%02d:%02d:%02d.%03d",
                  year_, month_, day_, hour_, minute_, second_, millisecond_);
  } else {
    std::snprintf(buffer, sizeof(buffer), "%04d%02d%02dT%02d%02d%02d.%03d",
                  year_, month_, day_, hour_, minute_, second_, millisecond_);
  }
  return std::string(buffer) + formatOffset(utcOffsetSeconds(), includeDividers);
}

std::string Time::toString(bool includeDate, bool includeTime,
                           bool includeSeconds, bool use24HourClock) const {
  char buf[64] = {};
  char* p = buf;
  const char* const end = buf + sizeof(buf);

  if (includeDate) {
    p += std::snprintf(p, static_cast<size_t>(end - p), "%d %s %d",
                       day_, kMonthShort[month_ - 1], year_);
    if (includeTime) *p++ = ' ';
  }

  if (includeTime) {
    const int h = use24HourClock ? hour_ : (hour_ % 12 == 0 ? 12 : hour_ % 12);
    p += std::snprintf(p, static_cast<size_t>(end - p), "%02d:%02d", h, minute_);
    if (includeSeconds)
      p += std::snprintf(p, static_cast<size_t>(end - p), ":%02d", second_);
    if (!use24HourClock)
      p += std::snprintf(p, static_cast<size_t>(end - p), "%s", hour_ >= 12 ? "pm" : "am");
  }

  return buf;
}

std::string Time::getMonthName(int month, bool abbreviated) {
  const int index = ((month - 1) % 12 + 12) % 12;
  return abbreviated ? kMonthShort[index] : kMonthLong[index];
}

std::string Time::getWeekdayName(int day, bool abbreviated) {
  const int index = (day % 7 + 7) % 7;
  return abbreviated ? kWeekdayShort[index] : kWeekdayLong[index];
}

UtcTime Time::fromISO8601(const std::string& iso) {
  int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0, millisecond = 0;
  bool parsed = false;

  // 扩展格式：2024-01-15T10:30:00[.123]
  if (std::sscanf(iso.c_str(), "%4d-%2d-%2dT%2d:%2d:%2d.%3d",
                  &year, &month, &day, &hour, &minute, &second, &millisecond) >= 6 ||
      std::sscanf(iso.c_str(), "%4d-%2d-%2dT%2d:%2d:%2d",
                  &year, &month, &day, &hour, &minute, &second) >= 6) {
    parsed = true;
  }
  // 紧凑格式：20240115T103000[.123]
  if (!parsed &&
      (std::sscanf(iso.c_str(), "%4d%2d%2dT%2d%2d%2d.%3d",
                   &year, &month, &day, &hour, &minute, &second, &millisecond) >= 6 ||
       std::sscanf(iso.c_str(), "%4d%2d%2dT%2d%2d%2d",
                   &year, &month, &day, &hour, &minute, &second) >= 6)) {
    parsed = true;
  }

  const auto inRange = [](int v, int lo, int hi) { return v >= lo && v <= hi; };
  if (!parsed || !inRange(year, kMinYear, kMaxYear) || !inRange(month, 1, 12) ||
      !inRange(day, 1, 31) || !inRange(hour, 0, 23) || !inRange(minute, 0, 59) ||
      !inRange(second, 0, 59) || !inRange(millisecond, 0, 999)) {
    return UtcTime(Timestamp(Timestamp::epoch()));
  }

  // 将解析出的字段按 UTC 解释得到 ns，再根据字符串中的时区偏移回退到真正的 UTC
  int64_t utcNs = UtcTime(year, month, day, hour, minute, second, millisecond).utcstamp().total();

  const size_t tPos = iso.find('T');
  if (tPos != std::string::npos) {
    for (size_t i = tPos + 6; i < iso.size(); ++i) {
      const char c = iso[i];
      if (c == 'Z') break;
      if (c == '+' || c == '-') {
        int tzH = 0, tzM = 0;
        const int sign = (c == '+') ? 1 : -1;
        if (std::sscanf(iso.c_str() + i + 1, "%2d:%2d", &tzH, &tzM) < 1)
          std::sscanf(iso.c_str() + i + 1, "%2d%2d", &tzH, &tzM);
        utcNs -= static_cast<int64_t>(sign) * (tzH * 3600ll + tzM * 60ll) * 1000000000ll;
        break;
      }
    }
  }

  return UtcTime(Timestamp(utcNs));
}

void Time::swap(Time& other) noexcept {
  using std::swap;
  swap(year_, other.year_);
  swap(month_, other.month_);
  swap(weekday_, other.weekday_);
  swap(day_, other.day_);
  swap(hour_, other.hour_);
  swap(minute_, other.minute_);
  swap(second_, other.second_);
  swap(millisecond_, other.millisecond_);
  swap(microsecond_, other.microsecond_);
  swap(nanosecond_, other.nanosecond_);
}

namespace {
std::tuple<int, int, int, int, int, int, int, int, int> fieldsOf(const Time& t) {
  return std::make_tuple(t.year(), t.month(), t.day(), t.hour(), t.minute(),
                         t.second(), t.millisecond(), t.microsecond(), t.nanosecond());
}
}  // namespace

bool operator==(const Time& t1, const Time& t2) noexcept { return fieldsOf(t1) == fieldsOf(t2); }
bool operator!=(const Time& t1, const Time& t2) noexcept { return fieldsOf(t1) != fieldsOf(t2); }
bool operator<(const Time& t1, const Time& t2) noexcept { return fieldsOf(t1) < fieldsOf(t2); }
bool operator>(const Time& t1, const Time& t2) noexcept { return fieldsOf(t1) > fieldsOf(t2); }
bool operator<=(const Time& t1, const Time& t2) noexcept { return fieldsOf(t1) <= fieldsOf(t2); }
bool operator>=(const Time& t1, const Time& t2) noexcept { return fieldsOf(t1) >= fieldsOf(t2); }

UtcTime::UtcTime(const Timestamp& timestamp) : Time(timestamp) {}  // 复用基类的 UTC 分解

UtcTime::UtcTime(const LocalTime& time) : UtcTime(time.localstamp()) {}

LocalTime::LocalTime(const Timestamp& timestamp) {
  const int64_t total = timestamp.total();
  const int64_t seconds = floorDiv(total, 1000000000ll);
  const int64_t sub = total - seconds * 1000000000ll;

  const std::tm r = localtimeOf(static_cast<time_t>(seconds));
  year_ = r.tm_year + 1900;
  month_ = r.tm_mon + 1;
  weekday_ = r.tm_wday;
  day_ = r.tm_mday;
  hour_ = r.tm_hour;
  minute_ = r.tm_min;
  second_ = r.tm_sec;
  millisecond_ = static_cast<int>(sub / 1000000);
  microsecond_ = static_cast<int>((sub / 1000) % 1000);
  nanosecond_ = static_cast<int>(sub % 1000);
}

LocalTime::LocalTime(const UtcTime& time) : LocalTime(time.utcstamp()) {}

int LocalTime::utcOffsetSeconds() const {
  // 本地挂钟字段按 UTC 与按本地两种解释之差即为本地相对 UTC 的偏移
  const int64_t diff = utcstamp().total() - localstamp().total();
  return static_cast<int>(diff / 1000000000ll);
}

}  // namespace time
}  // namespace pickup