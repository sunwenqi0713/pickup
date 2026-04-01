#include "pickup/time/Timespan.h"

namespace pickup {
namespace time {

// 公开构造函数：从 double 秒数转换，精度降至微秒级（double 约 15 位有效数字）
Timespan::Timespan(double seconds) noexcept
    : microseconds_(static_cast<int64_t>(seconds * 1e6)) {}

// 工厂方法：整数路径完全精确，不经过 double

Timespan Timespan::milliseconds(int ms) noexcept {
  return Timespan(static_cast<int64_t>(ms) * static_cast<int64_t>(1000));
}

Timespan Timespan::milliseconds(int64_t ms) noexcept {
  return Timespan(ms * static_cast<int64_t>(1000));
}

Timespan Timespan::seconds(double s) noexcept {
  return Timespan(static_cast<int64_t>(s * 1e6));
}

Timespan Timespan::minutes(double m) noexcept {
  return Timespan(static_cast<int64_t>(m * 60e6));
}

Timespan Timespan::hours(double h) noexcept {
  return Timespan(static_cast<int64_t>(h * 3600e6));
}

Timespan Timespan::days(double d) noexcept {
  return Timespan(static_cast<int64_t>(d * 86400e6));
}

Timespan Timespan::weeks(double w) noexcept {
  return Timespan(static_cast<int64_t>(w * 604800e6));
}

// 访问器

int64_t Timespan::inMilliseconds() const noexcept { return microseconds_ / 1000; }
double  Timespan::inMinutes()      const noexcept { return static_cast<double>(microseconds_) / 60e6; }
double  Timespan::inHours()        const noexcept { return static_cast<double>(microseconds_) / 3600e6; }
double  Timespan::inDays()         const noexcept { return static_cast<double>(microseconds_) / 86400e6; }
double  Timespan::inWeeks()        const noexcept { return static_cast<double>(microseconds_) / 604800e6; }

// 复合赋值运算符

Timespan Timespan::operator+=(Timespan t) noexcept {
  microseconds_ += t.microseconds_;
  return *this;
}

Timespan Timespan::operator-=(Timespan t) noexcept {
  microseconds_ -= t.microseconds_;
  return *this;
}

Timespan Timespan::operator+=(double secs) noexcept {
  microseconds_ += static_cast<int64_t>(secs * 1e6);
  return *this;
}

Timespan Timespan::operator-=(double secs) noexcept {
  microseconds_ -= static_cast<int64_t>(secs * 1e6);
  return *this;
}

// 自由函数运算符

Timespan operator+(Timespan t1, Timespan t2) noexcept { return t1 += t2; }
Timespan operator-(Timespan t1, Timespan t2) noexcept { return t1 -= t2; }

// 比较基于精确整数，无浮点舍入问题
bool operator==(Timespan t1, Timespan t2) noexcept { return t1.inMicroseconds() == t2.inMicroseconds(); }
bool operator!=(Timespan t1, Timespan t2) noexcept { return t1.inMicroseconds() != t2.inMicroseconds(); }
bool operator> (Timespan t1, Timespan t2) noexcept { return t1.inMicroseconds() >  t2.inMicroseconds(); }
bool operator< (Timespan t1, Timespan t2) noexcept { return t1.inMicroseconds() <  t2.inMicroseconds(); }
bool operator>=(Timespan t1, Timespan t2) noexcept { return t1.inMicroseconds() >= t2.inMicroseconds(); }
bool operator<=(Timespan t1, Timespan t2) noexcept { return t1.inMicroseconds() <= t2.inMicroseconds(); }

}  // namespace time
}  // namespace pickup
