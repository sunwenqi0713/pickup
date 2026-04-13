#include "pickup/time/Time.h"
#include "pickup/time/Timespan.h"

#include <chrono>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace pickup {
namespace time {

// 翻译单元内部工具（不暴露到头文件）
namespace {

// 将毫秒数转换为本地时间 tm 结构（失败时返回全零）
std::tm millisToLocal(int64_t millis) noexcept {
  std::tm result{};
  auto t = static_cast<time_t>(millis / 1000);
#ifdef _WIN32
  localtime_s(&result, &t);
#else
  localtime_r(&t, &result);
#endif
  return result;
}

// 将毫秒数转换为 UTC 时间 tm 结构（失败时返回全零）
std::tm millisToUTC(int64_t millis) noexcept {
  std::tm result{};
  auto t = static_cast<time_t>(millis / 1000);
#ifdef _WIN32
  gmtime_s(&result, &t);
#else
  gmtime_r(&t, &result);
#endif
  return result;
}

// 计算本地时区与 UTC 的偏移秒数
// 原理：将 UTC 的 tm 以"本地时间"解读后用 mktime 逆推，差值即为偏移
int getUTCOffsetSecondsFromMillis(int64_t millis) noexcept {
  auto utc = millisToUTC(millis);
  utc.tm_isdst = -1;
  return static_cast<int>((millis / 1000) - static_cast<int64_t>(mktime(&utc)));
}

// 处理负数情况的取模（使结果符号与 modulo 相同）
int extendedModulo(int64_t value, int modulo) noexcept {
  return static_cast<int>(value >= 0 ? (value % modulo)
                                     : (value - ((value / modulo) + 1) * modulo));
}

// strftime 封装（固定栈缓冲区，格式字符串过长时返回空串）
std::string formatString(const std::string& format, const std::tm* tm) {
  char buffer[256];
  auto n = strftime(buffer, sizeof(buffer) - 1, format.c_str(), tm);
  return n > 0 ? std::string(buffer, n) : std::string{};
}

bool isLeapYear(int year) noexcept {
  return (year % 400 == 0) || ((year % 100 != 0) && (year % 4 == 0));
}

int daysFromJan1(int year, int month) noexcept {
  const short dayOfYear[] = {0,  31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334,
                             0,  31,  60,  91, 121, 152, 182, 213, 244, 274, 305, 335};
  return dayOfYear[(isLeapYear(year) ? 12 : 0) + month];
}

int64_t daysFromYear0(int year) noexcept {
  --year;
  return 365 * year + (year / 400) - (year / 100) + (year / 4);
}

int64_t daysFrom1970(int year) noexcept {
  return daysFromYear0(year) - daysFromYear0(1970);
}

int64_t daysFrom1970(int year, int month) noexcept {
  if (month > 11) {
    year += month / 12;
    month %= 12;
  } else if (month < 0) {
    int numYears = (11 - month) / 12;
    year -= numYears;
    month += 12 * numYears;
  }
  return daysFrom1970(year) + daysFromJan1(year, month);
}

// 不依赖 mktime 的 UTC 时间戳计算（mktime 受本地时区影响）
int64_t mktime_utc(const std::tm& t) noexcept {
  return 24LL * 3600 * (daysFrom1970(t.tm_year + 1900, t.tm_mon) + (t.tm_mday - 1)) +
         3600 * t.tm_hour + 60 * t.tm_min + t.tm_sec;
}

// 格式化 UTC 偏移字符串，如 "+08:00"、"-05:00" 或 "Z"
std::string formatUTCOffset(int utcOffsetSeconds, bool includeSemiColon) {
  if (utcOffsetSeconds == 0) return "Z";

  int minutes    = utcOffsetSeconds / 60;
  int absMinutes = (minutes >= 0) ? minutes : -minutes;
  char buffer[8];
  if (includeSemiColon)
    snprintf(buffer, sizeof(buffer), "%+03d:%02d", minutes / 60, absMinutes % 60);
  else
    snprintf(buffer, sizeof(buffer), "%+03d%02d",  minutes / 60, absMinutes % 60);
  return buffer;
}

}  // namespace

Time::Time(int64_t ms) noexcept : millisSinceEpoch_(ms) {}

Time::Time(int year, int month, int day, int hours, int minutes,
           int seconds, int milliseconds, bool useLocalTime) noexcept {
  std::tm t{};
  t.tm_year  = year - 1900;
  t.tm_mon   = month;
  t.tm_mday  = day;
  t.tm_hour  = hours;
  t.tm_min   = minutes;
  t.tm_sec   = seconds;
  t.tm_isdst = -1;

  millisSinceEpoch_ = 1000LL * (useLocalTime ? static_cast<int64_t>(mktime(&t))
                                             : mktime_utc(t)) + milliseconds;
}

int64_t Time::currentTimeMillis() noexcept {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

Time Time::getCurrentTime() noexcept {
  return Time(currentTimeMillis());
}

Time Time::fromTimePoint(std::chrono::system_clock::time_point tp) noexcept {
  using namespace std::chrono;
  return Time(duration_cast<milliseconds>(tp.time_since_epoch()).count());
}

Time Time::fromISO8601(const std::string& iso) {
  int year = 0, month = 0, day = 0;
  int hours = 0, minutes = 0, seconds = 0, milliseconds = 0;
  bool parsed = false;

  // 扩展格式：2024-01-15T10:30:00[.123]
  if (sscanf(iso.c_str(), "%4d-%2d-%2dT%2d:%2d:%2d.%3d",
             &year, &month, &day, &hours, &minutes, &seconds, &milliseconds) >= 6 ||
      sscanf(iso.c_str(), "%4d-%2d-%2dT%2d:%2d:%2d",
             &year, &month, &day, &hours, &minutes, &seconds) >= 6) {
    parsed = true;
  }

  // 紧凑格式：20240115T103000[.123]
  if (!parsed &&
      (sscanf(iso.c_str(), "%4d%2d%2dT%2d%2d%2d.%3d",
              &year, &month, &day, &hours, &minutes, &seconds, &milliseconds) >= 6 ||
       sscanf(iso.c_str(), "%4d%2d%2dT%2d%2d%2d",
              &year, &month, &day, &hours, &minutes, &seconds) >= 6)) {
    parsed = true;
  }

  if (!parsed) return Time();

  // month 在 ISO-8601 字符串中为 1–12，构造函数期望 0–11
  int64_t utcMs = Time(year, month - 1, day, hours, minutes, seconds,
                       milliseconds, /*useLocalTime=*/false).toMilliseconds();

  // 以 'T' 的位置为锚点查找时区偏移（Z / +HH:MM / -HH:MM / +HHMM / -HHMM）
  // 从 'T' 后第 6 个字符开始搜索（跳过最短时间部分 HHmmss）
  size_t tPos = iso.find('T');
  if (tPos != std::string::npos) {
    for (size_t i = tPos + 6; i < iso.size(); ++i) {
      const char c = iso[i];
      if (c == 'Z') {
        break;  // UTC，偏移为 0
      }
      if (c == '+' || c == '-') {
        int tzH = 0, tzM = 0;
        int sign = (c == '+') ? 1 : -1;
        // 先尝试 HH:MM 格式，再尝试 HHMM 格式
        if (sscanf(iso.c_str() + i + 1, "%2d:%2d", &tzH, &tzM) < 1)
          sscanf(iso.c_str() + i + 1, "%2d%2d", &tzH, &tzM);
        // UTC 时间 = 字符串中的时间 - 时区偏移
        utcMs -= static_cast<int64_t>(sign) * (tzH * 3600LL + tzM * 60LL) * 1000LL;
        break;
      }
    }
  }

  return Time(utcMs);
}

std::chrono::system_clock::time_point Time::toTimePoint() const noexcept {
  using namespace std::chrono;
  return system_clock::time_point(milliseconds(millisSinceEpoch_));
}

Time::BrokenDownTime Time::toBrokenDown() const noexcept {
  const std::tm t = millisToLocal(millisSinceEpoch_);
  return {
      t.tm_year + 1900,
      t.tm_mon,
      t.tm_mday,
      t.tm_hour,
      t.tm_min,
      extendedModulo(millisSinceEpoch_ / 1000, 60),
      extendedModulo(millisSinceEpoch_, 1000)};
}

int Time::getYear()         const noexcept { return millisToLocal(millisSinceEpoch_).tm_year + 1900; }
int Time::getMonth()        const noexcept { return millisToLocal(millisSinceEpoch_).tm_mon; }
int Time::getDayOfYear()    const noexcept { return millisToLocal(millisSinceEpoch_).tm_yday; }
int Time::getDayOfMonth()   const noexcept { return millisToLocal(millisSinceEpoch_).tm_mday; }
int Time::getDayOfWeek()    const noexcept { return millisToLocal(millisSinceEpoch_).tm_wday; }
int Time::getHours()        const noexcept { return millisToLocal(millisSinceEpoch_).tm_hour; }
int Time::getMinutes()      const noexcept { return millisToLocal(millisSinceEpoch_).tm_min; }
int Time::getSeconds()      const noexcept { return extendedModulo(millisSinceEpoch_ / 1000, 60); }
int Time::getMilliseconds() const noexcept { return extendedModulo(millisSinceEpoch_, 1000); }

int Time::getUTCOffsetSeconds() const noexcept {
  return getUTCOffsetSecondsFromMillis(millisSinceEpoch_);
}

std::string Time::getUTCOffsetString(bool includeDivider) const {
  return formatUTCOffset(getUTCOffsetSeconds(), includeDivider);
}

std::string Time::getTimeZone() const {
  std::tm t = millisToLocal(millisSinceEpoch_);
  char buffer[32] = {};
  if (strftime(buffer, sizeof(buffer) - 1, "%Z", &t) > 0) return buffer;
  return {};
}

std::string Time::toString(bool includeDate, bool includeTime,
                           bool includeSeconds, bool use24HourClock) const {
  // 单次 localtime_r，避免逐字段调用产生多次系统调用
  const std::tm t = millisToLocal(millisSinceEpoch_);

  static const char* const shortMonths[] = {
      "Jan", "Feb", "Mar", "Apr", "May", "Jun",
      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

  char buf[64] = {};
  char* p = buf;
  const char* const end = buf + sizeof(buf);

  if (includeDate) {
    p += snprintf(p, static_cast<size_t>(end - p), "%d %s %d",
                  t.tm_mday, shortMonths[t.tm_mon], t.tm_year + 1900);
    if (includeTime) *p++ = ' ';
  }

  if (includeTime) {
    const int h = use24HourClock ? t.tm_hour
                                 : (t.tm_hour % 12 == 0 ? 12 : t.tm_hour % 12);
    p += snprintf(p, static_cast<size_t>(end - p), "%02d:%02d", h, t.tm_min);
    if (includeSeconds)
      p += snprintf(p, static_cast<size_t>(end - p), ":%02d", t.tm_sec);
    if (!use24HourClock)
      p += snprintf(p, static_cast<size_t>(end - p), "%s",
                    t.tm_hour >= 12 ? "pm" : "am");
  }

  return buf;
}

std::string Time::formatted(const std::string& format) const {
  std::tm t = millisToLocal(millisSinceEpoch_);
  return formatString(format, &t);
}

std::string Time::toISO8601(bool includeDivider) const {
  // 单次 localtime_r，获取日期/时间字段
  const std::tm t      = millisToLocal(millisSinceEpoch_);
  const int ms         = extendedModulo(millisSinceEpoch_, 1000);
  const int utcOffset  = getUTCOffsetSecondsFromMillis(millisSinceEpoch_);

  char buffer[64];
  if (includeDivider) {
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02dT%02d:%02d:%02d.%03d",
             t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
             t.tm_hour, t.tm_min, t.tm_sec, ms);
  } else {
    snprintf(buffer, sizeof(buffer), "%04d%02d%02dT%02d%02d%02d.%03d",
             t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
             t.tm_hour, t.tm_min, t.tm_sec, ms);
  }
  return std::string(buffer) + formatUTCOffset(utcOffset, includeDivider);
}

std::string Time::getMonthName(int monthNumber, bool abbreviated) {
  static const char* const shortNames[] = {
      "Jan", "Feb", "Mar", "Apr", "May", "Jun",
      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  static const char* const longNames[] = {
      "January", "February", "March",     "April",   "May",      "June",
      "July",    "August",   "September", "October", "November", "December"};
  monthNumber %= 12;
  return abbreviated ? shortNames[monthNumber] : longNames[monthNumber];
}

std::string Time::getWeekdayName(int day, bool abbreviated) {
  static const char* const shortNames[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  static const char* const longNames[]  = {
      "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  day %= 7;
  return abbreviated ? shortNames[day] : longNames[day];
}

int64_t Time::getMillisecondCounter() noexcept {
  using namespace std::chrono;
  return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

double Time::getMillisecondCounterHiRes() noexcept {
  using namespace std::chrono;
  return duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count() / 1e6;
}

Time& Time::operator+=(Timespan delta) noexcept {
  millisSinceEpoch_ += delta.inMilliseconds();
  return *this;
}

Time& Time::operator-=(Timespan delta) noexcept {
  millisSinceEpoch_ -= delta.inMilliseconds();
  return *this;
}

Time     operator+(Time time, Timespan delta) noexcept { time += delta; return time; }
Time     operator+(Timespan delta, Time time) noexcept { time += delta; return time; }
Time     operator-(Time time, Timespan delta) noexcept { time -= delta; return time; }
Timespan operator-(Time time1, Time time2)   noexcept {
  return Timespan::milliseconds(time1.toMilliseconds() - time2.toMilliseconds());
}

bool operator==(Time t1, Time t2) noexcept { return t1.toMilliseconds() == t2.toMilliseconds(); }
bool operator!=(Time t1, Time t2) noexcept { return t1.toMilliseconds() != t2.toMilliseconds(); }
bool operator< (Time t1, Time t2) noexcept { return t1.toMilliseconds() <  t2.toMilliseconds(); }
bool operator<=(Time t1, Time t2) noexcept { return t1.toMilliseconds() <= t2.toMilliseconds(); }
bool operator> (Time t1, Time t2) noexcept { return t1.toMilliseconds() >  t2.toMilliseconds(); }
bool operator>=(Time t1, Time t2) noexcept { return t1.toMilliseconds() >= t2.toMilliseconds(); }

}  // namespace time
}  // namespace pickup
