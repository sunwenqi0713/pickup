#include "pickup/time/Time.h"
#include "pickup/time/Timespan.h"

#include <atomic>
#include <chrono>
#include <cstring>
#include <ctime>
#include <sstream>
#include <thread>
#include <vector>
#include <iomanip>

namespace pickup {
namespace time {

// **静态成员变量的定义**
std::atomic<uint32_t> Time::lastMSCounterValue_{0};

Time::Time(int64_t ms) noexcept : millisSinceEpoch_(ms) {}

Time::Time(int year, int month, int day, int hours, int minutes, int seconds, int milliseconds,
           bool useLocalTime) noexcept {
  std::tm t;
  t.tm_year = year - 1900;
  t.tm_mon = month;
  t.tm_mday = day;
  t.tm_hour = hours;
  t.tm_min = minutes;
  t.tm_sec = seconds;
  t.tm_isdst = -1;

  millisSinceEpoch_ = 1000 * (useLocalTime ? (int64_t)mktime(&t) : mktime_utc(t)) + milliseconds;
}

std::tm Time::millisToLocal(int64_t millis) noexcept {
  std::tm result;
  auto now = (time_t)(millis / 1000);

#ifdef _WIN32
  localtime_s(&result, &now);
#else
  localtime_r(&now, &result);
#endif

  return result;
}

std::tm Time::millisToUTC(int64_t millis) noexcept {
  std::tm result;
  auto now = (time_t)(millis / 1000);

#ifdef _WIN32
  gmtime_s(&result, &now);
#else
  gmtime_r(&now, &result);
#endif

  return result;
}

int Time::getUTCOffsetSecondsFromMillis(int64_t millis) noexcept {
  auto utc = millisToUTC(millis);
  utc.tm_isdst = -1;  // Treat this UTC time as local to find the offset
  return (int)((millis / 1000) - (int64_t)mktime(&utc));
}

int Time::extendedModulo(int64_t value, int modulo) noexcept {
  return (int)(value >= 0 ? (value % modulo) : (value - ((value / modulo) + 1) * modulo));
}

std::string Time::formatString(const std::string& format, const std::tm* tm) {
  const size_t bufferSize = 256;
  char buffer[bufferSize];

  auto numChars = strftime(buffer, bufferSize - 1, format.c_str(), tm);

  if (numChars > 0) return std::string(buffer, numChars);

  return "";
}

bool Time::isLeapYear(int year) noexcept { 
  return (year % 400 == 0) || ((year % 100 != 0) && (year % 4 == 0)); 
}

int Time::daysFromJan1(int year, int month) noexcept {
  const short dayOfYear[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334,
                             0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
  return dayOfYear[(isLeapYear(year) ? 12 : 0) + month];
}

int64_t Time::daysFromYear0(int year) noexcept {
  --year;
  return 365 * year + (year / 400) - (year / 100) + (year / 4);
}

int64_t Time::daysFrom1970(int year) noexcept { 
  return daysFromYear0(year) - daysFromYear0(1970); 
}

int64_t Time::daysFrom1970(int year, int month) noexcept {
  if (month > 11) {
    year += month / 12;
    month %= 12;
  } else if (month < 0) {
    auto numYears = (11 - month) / 12;
    year -= numYears;
    month += 12 * numYears;
  }

  return daysFrom1970(year) + daysFromJan1(year, month);
}

int64_t Time::mktime_utc(const std::tm& t) noexcept {
  return 24 * 3600 * (daysFrom1970(t.tm_year + 1900, t.tm_mon) + (t.tm_mday - 1)) + 
         3600 * t.tm_hour + 60 * t.tm_min + t.tm_sec;
}

std::string Time::getUTCOffsetString(int utcOffsetSeconds, bool includeSemiColon) {
  if (utcOffsetSeconds != 0) {
    auto minutes = utcOffsetSeconds / 60;
    char buffer[8];

    if (includeSemiColon)
      snprintf(buffer, sizeof(buffer), "%+03d:%02d", minutes / 60, abs(minutes) % 60);
    else
      snprintf(buffer, sizeof(buffer), "%+03d%02d", minutes / 60, abs(minutes) % 60);

    return buffer;
  }

  return "Z";
}

int64_t Time::currentTimeMillis() noexcept {
  using namespace std::chrono;
  auto now = system_clock::now();
  return duration_cast<milliseconds>(now.time_since_epoch()).count();
}

Time Time::getCurrentTime() noexcept { 
  return Time(currentTimeMillis()); 
}

uint32_t Time::getMillisecondCounter() noexcept {
  using namespace std::chrono;
  auto now = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
  auto now32 = (uint32_t)now;

  if (now32 < lastMSCounterValue_.load()) {
    if (now32 < lastMSCounterValue_.load() - (uint32_t)1000) 
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

    auto toWait = (int)(targetTime - now);

    if (toWait > 2) {
      std::this_thread::sleep_for(std::chrono::milliseconds(std::min(20, toWait >> 1)));
    } else {
      for (int i = 10; --i >= 0;) 
        std::this_thread::yield();
    }
  }
}

double Time::getMillisecondCounterHiRes() noexcept {
  using namespace std::chrono;
  auto now = duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count();
  return now / 1000000.0;
}

int64_t Time::getHighResolutionTicks() noexcept {
  using namespace std::chrono;
  return high_resolution_clock::now().time_since_epoch().count();
}

int64_t Time::getHighResolutionTicksPerSecond() noexcept {
  using namespace std::chrono;
  return high_resolution_clock::period::den / high_resolution_clock::period::num;
}

double Time::highResolutionTicksToSeconds(int64_t ticks) noexcept {
  return (double)ticks / (double)getHighResolutionTicksPerSecond();
}

int64_t Time::secondsToHighResolutionTicks(double seconds) noexcept {
  return (int64_t)(seconds * (double)getHighResolutionTicksPerSecond());
}

// **实现 operator+= 和 operator-=**
Time& Time::operator+=(Timespan delta) noexcept {
  millisSinceEpoch_ += delta.inMilliseconds();
  return *this;
}

Time& Time::operator-=(Timespan delta) noexcept {
  millisSinceEpoch_ -= delta.inMilliseconds();
  return *this;
}

std::string Time::toString(bool includeDate, bool includeTime, bool includeSeconds, bool use24HourClock) const {
  std::stringstream ss;

  if (includeDate) {
    ss << getDayOfMonth() << ' ' << getMonthName(true) << ' ' << getYear();

    if (includeTime) ss << ' ';
  }

  if (includeTime) {
    auto mins = getMinutes();
    auto hours = use24HourClock ? getHours() : getHoursInAmPmFormat();

    ss << (hours < 10 ? "0" : "") << hours << (mins < 10 ? ":0" : ":") << mins;

    if (includeSeconds) {
      auto secs = getSeconds();
      ss << (secs < 10 ? ":0" : ":") << secs;
    }

    if (!use24HourClock) 
      ss << (isAfternoon() ? "pm" : "am");
  }

  auto result = ss.str();
  // Trim trailing spaces
  while (!result.empty() && result.back() == ' ') 
    result.pop_back();
  return result;
}

std::string Time::formatted(const std::string& format) const {
  std::tm t = millisToLocal(millisSinceEpoch_);
  return formatString(format, &t);
}

int Time::getYear() const noexcept { 
  return millisToLocal(millisSinceEpoch_).tm_year + 1900; 
}

int Time::getMonth() const noexcept { 
  return millisToLocal(millisSinceEpoch_).tm_mon; 
}

int Time::getDayOfYear() const noexcept { 
  return millisToLocal(millisSinceEpoch_).tm_yday; 
}

int Time::getDayOfMonth() const noexcept { 
  return millisToLocal(millisSinceEpoch_).tm_mday; 
}

int Time::getDayOfWeek() const noexcept { 
  return millisToLocal(millisSinceEpoch_).tm_wday; 
}

int Time::getHours() const noexcept { 
  return millisToLocal(millisSinceEpoch_).tm_hour; 
}

int Time::getMinutes() const noexcept { 
  return millisToLocal(millisSinceEpoch_).tm_min; 
}

int Time::getSeconds() const noexcept { 
  return extendedModulo(millisSinceEpoch_ / 1000, 60); 
}

int Time::getMilliseconds() const noexcept { 
  return extendedModulo(millisSinceEpoch_, 1000); 
}

int Time::getHoursInAmPmFormat() const noexcept {
  auto hours = getHours();

  if (hours == 0) return 12;
  if (hours <= 12) return hours;

  return hours - 12;
}

bool Time::isAfternoon() const noexcept { 
  return getHours() >= 12; 
}

bool Time::isDaylightSavingTime() const noexcept { 
  return millisToLocal(millisSinceEpoch_).tm_isdst != 0; 
}

std::string Time::getTimeZone() const {
  std::tm t = millisToLocal(millisSinceEpoch_);
  char buffer[32] = {0};

  if (strftime(buffer, sizeof(buffer) - 1, "%Z", &t) > 0) 
    return buffer;

  return "";
}

int Time::getUTCOffsetSeconds() const noexcept { 
  return getUTCOffsetSecondsFromMillis(millisSinceEpoch_); 
}

std::string Time::getUTCOffsetString(bool includeDividerCharacters) const {
  return getUTCOffsetString(getUTCOffsetSeconds(), includeDividerCharacters);
}

std::string Time::toISO8601(bool includeDividerCharacters) const {
  char buffer[64];

  if (includeDividerCharacters) {
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02dT%02d:%02d:%06.3f", 
             getYear(), getMonth() + 1, getDayOfMonth(),
             getHours(), getMinutes(), getSeconds() + getMilliseconds() / 1000.0);
  } else {
    snprintf(buffer, sizeof(buffer), "%04d%02d%02dT%02d%02d%06.3f", 
             getYear(), getMonth() + 1, getDayOfMonth(),
             getHours(), getMinutes(), getSeconds() + getMilliseconds() / 1000.0);
  }

  return std::string(buffer) + getUTCOffsetString(includeDividerCharacters);
}

Time Time::fromISO8601(const std::string& iso) {
  int year = 0, month = 0, day = 0;
  int hours = 0, minutes = 0, seconds = 0, milliseconds = 0;

  if (sscanf(iso.c_str(), "%4d-%2d-%2dT%2d:%2d:%2d.%3d", 
             &year, &month, &day, &hours, &minutes, &seconds, &milliseconds) >= 6) {
    return Time(year, month - 1, day, hours, minutes, seconds, milliseconds, false);
  }

  if (sscanf(iso.c_str(), "%4d%2d%2dT%2d%2d%2d.%3d", 
             &year, &month, &day, &hours, &minutes, &seconds, &milliseconds) >= 6) {
    return Time(year, month - 1, day, hours, minutes, seconds, milliseconds, false);
  }

  return Time();
}

std::string Time::getMonthName(int monthNumber, bool threeLetterVersion) {
  static const char* const shortMonthNames[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                                "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  static const char* const longMonthNames[] = {"January", "February", "March", "April", 
                                               "May", "June", "July", "August", "September", 
                                               "October", "November", "December"};

  monthNumber %= 12;

  return threeLetterVersion ? shortMonthNames[monthNumber] : longMonthNames[monthNumber];
}

std::string Time::getWeekdayName(int day, bool threeLetterVersion) {
  static const char* const shortDayNames[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  static const char* const longDayNames[] = {"Sunday", "Monday", "Tuesday", "Wednesday",
                                             "Thursday", "Friday", "Saturday"};

  day %= 7;

  return threeLetterVersion ? shortDayNames[day] : longDayNames[day];
}

std::string Time::getMonthName(bool threeLetterVersion) const { 
  return getMonthName(getMonth(), threeLetterVersion); 
}

std::string Time::getWeekdayName(bool threeLetterVersion) const {
  return getWeekdayName(getDayOfWeek(), threeLetterVersion);
}

bool Time::setSystemTimeToThisTime() const {
  // This would require platform-specific code to set system time
  // Not implemented in this portable version
  return false;
}

Time Time::getCompilationDate() {
  // Implementation using __DATE__ and __TIME__ macros
  // This is a simplified version
  const char* dateStr = __DATE__;
  const char* timeStr = __TIME__;
  
  int month = 0, day = 0, year = 0;
  int hour = 0, minute = 0, second = 0;
  
  // Parse __DATE__ (format: "MMM DD YYYY")
  char monthStr[4] = {0};
  sscanf(dateStr, "%3s %d %d", monthStr, &day, &year);
  
  // Parse __TIME__ (format: "HH:MM:SS")
  sscanf(timeStr, "%d:%d:%d", &hour, &minute, &second);
  
  // Convert month string to number
  static const char* const months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                       "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  for (int i = 0; i < 12; ++i) {
    if (strcmp(monthStr, months[i]) == 0) {
      month = i;
      break;
    }
  }
  
  return Time(year, month, day, hour, minute, second, 0, true);
}

// **实现全局操作符函数**
Time operator+(Time time, Timespan delta) noexcept {
  time += delta;
  return time;
}

Time operator+(Timespan delta, Time time) noexcept {
  time += delta;
  return time;
}

Time operator-(Time time, Timespan delta) noexcept {
  time -= delta;
  return time;
}

Timespan operator-(Time time1, Time time2) noexcept {
  return Timespan::milliseconds(time1.toMilliseconds() - time2.toMilliseconds());
}

bool operator==(Time time1, Time time2) noexcept { 
  return time1.toMilliseconds() == time2.toMilliseconds(); 
}

bool operator!=(Time time1, Time time2) noexcept { 
  return time1.toMilliseconds() != time2.toMilliseconds(); 
}

bool operator<(Time time1, Time time2) noexcept { 
  return time1.toMilliseconds() < time2.toMilliseconds(); 
}

bool operator>(Time time1, Time time2) noexcept { 
  return time1.toMilliseconds() > time2.toMilliseconds(); 
}

bool operator<=(Time time1, Time time2) noexcept { 
  return time1.toMilliseconds() <= time2.toMilliseconds(); 
}

bool operator>=(Time time1, Time time2) noexcept { 
  return time1.toMilliseconds() >= time2.toMilliseconds(); 
}

}  // namespace time
}  // namespace pickup