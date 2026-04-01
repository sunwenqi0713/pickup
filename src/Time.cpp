#include "pickup/time/Time.h"
#include "pickup/time/Timespan.h"

#include <atomic>
#include <chrono>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>
#include <vector>

namespace pickup {
namespace time {

// 实现辅助函数（不暴露到头文件，仅在此翻译单元可见）
namespace {

// 将毫秒数转换为本地时间 tm 结构（零初始化，系统调用失败时返回全零）
std::tm millisToLocal(int64_t millis) noexcept {
  std::tm result{};
  auto now = static_cast<time_t>(millis / 1000);
#ifdef _WIN32
  localtime_s(&result, &now);
#else
  localtime_r(&now, &result);
#endif
  return result;
}

// 将毫秒数转换为 UTC 时间 tm 结构（零初始化，系统调用失败时返回全零）
std::tm millisToUTC(int64_t millis) noexcept {
  std::tm result{};
  auto now = static_cast<time_t>(millis / 1000);
#ifdef _WIN32
  gmtime_s(&result, &now);
#else
  gmtime_r(&now, &result);
#endif
  return result;
}

int getUTCOffsetSecondsFromMillis(int64_t millis) noexcept {
  auto utc = millisToUTC(millis);
  utc.tm_isdst = -1;
  return static_cast<int>((millis / 1000) - static_cast<int64_t>(mktime(&utc)));
}

int extendedModulo(int64_t value, int modulo) noexcept {
  return static_cast<int>(value >= 0 ? (value % modulo)
                                     : (value - ((value / modulo) + 1) * modulo));
}

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

int64_t mktime_utc(const std::tm& t) noexcept {
  return 24 * 3600 * (daysFrom1970(t.tm_year + 1900, t.tm_mon) + (t.tm_mday - 1)) +
         3600 * t.tm_hour + 60 * t.tm_min + t.tm_sec;
}

// 格式化 UTC 偏移字符串，如 "+08:00"、"-05:00" 或 "Z"
std::string formatUTCOffset(int utcOffsetSeconds, bool includeSemiColon) {
  if (utcOffsetSeconds == 0) return "Z";

  int minutes   = utcOffsetSeconds / 60;
  int absMinutes = (minutes >= 0) ? minutes : -minutes;
  char buffer[8];
  if (includeSemiColon)
    snprintf(buffer, sizeof(buffer), "%+03d:%02d", minutes / 60, absMinutes % 60);
  else
    snprintf(buffer, sizeof(buffer), "%+03d%02d", minutes / 60, absMinutes % 60);
  return buffer;
}

}  // namespace

// 静态成员初始化

std::atomic<uint32_t> Time::lastMSCounterValue_{0};

// 构造函数

Time::Time(int64_t ms) noexcept : millisSinceEpoch_(ms) {}

Time::Time(int year, int month, int day, int hours, int minutes, int seconds,
           int milliseconds, bool useLocalTime) noexcept {
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

// 当前时间

int64_t Time::currentTimeMillis() noexcept {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

Time Time::getCurrentTime() noexcept {
  return Time(currentTimeMillis());
}

// 从其他类型构造

Time Time::fromTimePoint(std::chrono::system_clock::time_point tp) noexcept {
  using namespace std::chrono;
  return Time(duration_cast<milliseconds>(tp.time_since_epoch()).count());
}

Time Time::fromISO8601(const std::string& iso) {
  int year = 0, month = 0, day = 0;
  int hours = 0, minutes = 0, seconds = 0, milliseconds = 0;
  bool parsed = false;

  // 尝试带分隔符格式：2024-01-15T10:30:00.123
  if (sscanf(iso.c_str(), "%4d-%2d-%2dT%2d:%2d:%2d.%3d",
             &year, &month, &day, &hours, &minutes, &seconds, &milliseconds) >= 6 ||
      sscanf(iso.c_str(), "%4d-%2d-%2dT%2d:%2d:%2d",
             &year, &month, &day, &hours, &minutes, &seconds) >= 6) {
    parsed = true;
  }
  // 尝试紧凑格式：20240115T103000.123
  if (!parsed &&
      (sscanf(iso.c_str(), "%4d%2d%2dT%2d%2d%2d.%3d",
              &year, &month, &day, &hours, &minutes, &seconds, &milliseconds) >= 6 ||
       sscanf(iso.c_str(), "%4d%2d%2dT%2d%2d%2d",
              &year, &month, &day, &hours, &minutes, &seconds) >= 6)) {
    parsed = true;
  }
  if (!parsed) return Time();

  // 将日期/时间字段当作 UTC 解析
  constexpr bool kUseLocalTime = false;
  int64_t utcMs = Time(year, month - 1, day, hours, minutes, seconds,
                       milliseconds, kUseLocalTime).toMilliseconds();

  // 解析时区偏移（格式：Z | +HH:MM | -HH:MM | +HHMM | -HHMM）
  const size_t searchFrom = 19;  // 跳过 "YYYY-MM-DDTHH:MM:SS"
  for (size_t i = std::min(iso.size(), searchFrom); i < iso.size(); ++i) {
    const char c = iso[i];
    if (c == 'Z') {
      break;  // UTC，偏移为 0
    }
    if (c == '+' || c == '-') {
      int tzH = 0, tzM = 0;
      int sign = (c == '+') ? 1 : -1;
      // 先尝试 HH:MM，再尝试 HHMM
      if (sscanf(iso.c_str() + i + 1, "%2d:%2d", &tzH, &tzM) < 1)
        sscanf(iso.c_str() + i + 1, "%2d%2d", &tzH, &tzM);
      // UTC 时间 = 解析时间 - 时区偏移
      utcMs -= static_cast<int64_t>(sign) * (tzH * 3600LL + tzM * 60LL) * 1000LL;
      break;
    }
  }

  return Time(utcMs);
}

Time Time::getCompilationDate() {
  const char* dateStr = __DATE__;
  const char* timeStr = __TIME__;

  char monthStr[4] = {};
  int  day = 0, year = 0, hour = 0, minute = 0, second = 0;

  if (sscanf(dateStr, "%3s %d %d", monthStr, &day, &year) != 3) return Time();
  if (sscanf(timeStr, "%d:%d:%d", &hour, &minute, &second) != 3) return Time();

  static const char* const months[] = {
      "Jan", "Feb", "Mar", "Apr", "May", "Jun",
      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

  int month = 0;
  for (int i = 0; i < 12; ++i) {
    if (strcmp(monthStr, months[i]) == 0) { month = i; break; }
  }

  constexpr bool kUseLocalTime = true;
  return Time(year, month, day, hour, minute, second, 0, kUseLocalTime);
}

// 转换

std::chrono::system_clock::time_point Time::toTimePoint() const noexcept {
  using namespace std::chrono;
  return system_clock::time_point(milliseconds(millisSinceEpoch_));
}

// 字段分解

Time::BrokenDownTime Time::toBrokenDown() const noexcept {
  // 单次 localtime_r 获取所有字段
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

int Time::getYear()       const noexcept { return millisToLocal(millisSinceEpoch_).tm_year + 1900; }
int Time::getMonth()      const noexcept { return millisToLocal(millisSinceEpoch_).tm_mon; }
int Time::getDayOfYear()  const noexcept { return millisToLocal(millisSinceEpoch_).tm_yday; }
int Time::getDayOfMonth() const noexcept { return millisToLocal(millisSinceEpoch_).tm_mday; }
int Time::getDayOfWeek()  const noexcept { return millisToLocal(millisSinceEpoch_).tm_wday; }
int Time::getHours()      const noexcept { return millisToLocal(millisSinceEpoch_).tm_hour; }
int Time::getMinutes()    const noexcept { return millisToLocal(millisSinceEpoch_).tm_min; }
int Time::getSeconds()    const noexcept { return extendedModulo(millisSinceEpoch_ / 1000, 60); }
int Time::getMilliseconds() const noexcept { return extendedModulo(millisSinceEpoch_, 1000); }

int Time::getHoursInAmPmFormat() const noexcept {
  auto h = getHours();
  if (h == 0) return 12;
  if (h <= 12) return h;
  return h - 12;
}

bool Time::isAfternoon()          const noexcept { return getHours() >= 12; }
bool Time::isDaylightSavingTime() const noexcept { return millisToLocal(millisSinceEpoch_).tm_isdst != 0; }
int  Time::getUTCOffsetSeconds()  const noexcept { return getUTCOffsetSecondsFromMillis(millisSinceEpoch_); }

std::string Time::getTimeZone() const {
  std::tm t = millisToLocal(millisSinceEpoch_);
  char buffer[32] = {};
  if (strftime(buffer, sizeof(buffer) - 1, "%Z", &t) > 0) return buffer;
  return {};
}

std::string Time::getUTCOffsetString(bool includeDivider) const {
  return formatUTCOffset(getUTCOffsetSeconds(), includeDivider);
}

// 格式化（单次 localtime_r）

std::string Time::toString(bool includeDate, bool includeTime,
                           bool includeSeconds, bool use24HourClock) const {
  // 单次 localtime_r，避免逐字段调用产生 N 次系统调用
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
  // 单次 localtime_r 获取所有字段，不调用各个 getXxx()
  const std::tm t = millisToLocal(millisSinceEpoch_);
  const int ms    = extendedModulo(millisSinceEpoch_, 1000);
  const int utcOffset = getUTCOffsetSecondsFromMillis(millisSinceEpoch_);

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

// 静态名称工具

std::string Time::getMonthName(int monthNumber, bool threeLetterVersion) {
  static const char* const shortNames[] = {
      "Jan", "Feb", "Mar", "Apr", "May", "Jun",
      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  static const char* const longNames[] = {
      "January", "February", "March",     "April",   "May",      "June",
      "July",    "August",   "September", "October", "November", "December"};
  monthNumber %= 12;
  return threeLetterVersion ? shortNames[monthNumber] : longNames[monthNumber];
}

std::string Time::getWeekdayName(int day, bool threeLetterVersion) {
  static const char* const shortNames[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  static const char* const longNames[]  = {
      "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  day %= 7;
  return threeLetterVersion ? shortNames[day] : longNames[day];
}

std::string Time::getMonthName(bool threeLetterVersion)   const { return getMonthName(getMonth(), threeLetterVersion); }
std::string Time::getWeekdayName(bool threeLetterVersion) const { return getWeekdayName(getDayOfWeek(), threeLetterVersion); }

// 单调时钟

uint32_t Time::getMillisecondCounter() noexcept {
  using namespace std::chrono;
  auto now  = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
  auto now32 = static_cast<uint32_t>(now);

  // 单次 load，避免两次 load 之间的 TOCTOU 竞争
  const auto last = lastMSCounterValue_.load();
  if (now32 < last) {
    if (now32 < last - static_cast<uint32_t>(1000))
      lastMSCounterValue_.store(now32);
  } else {
    lastMSCounterValue_.store(now32);
  }
  return now32;
}

uint32_t Time::getApproximateMillisecondCounter() noexcept {
  auto t = lastMSCounterValue_.load();
  return t == 0 ? getMillisecondCounter() : t;
}

void Time::waitForMillisecondCounter(uint32_t targetTime) noexcept {
  for (;;) {
    auto now = getMillisecondCounter();
    if (now >= targetTime) break;
    auto toWait = static_cast<int>(targetTime - now);
    if (toWait > 2)
      std::this_thread::sleep_for(std::chrono::milliseconds(std::min(20, toWait >> 1)));
    else
      for (int i = 10; --i >= 0;) std::this_thread::yield();
  }
}

double Time::getMillisecondCounterHiRes() noexcept {
  using namespace std::chrono;
  return duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count() / 1e6;
}

int64_t Time::getHighResolutionTicks() noexcept {
  return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

double  Time::highResolutionTicksToSeconds(int64_t ticks) noexcept {
  return static_cast<double>(ticks) / static_cast<double>(getHighResolutionTicksPerSecond());
}

int64_t Time::secondsToHighResolutionTicks(double seconds) noexcept {
  return static_cast<int64_t>(seconds * static_cast<double>(getHighResolutionTicksPerSecond()));
}

// 系统时钟设置

bool Time::setSystemTimeToThisTime() const {
  return false;  // 需要平台特定实现及管理员权限
}

// 运算符

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
