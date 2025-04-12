#include "pickup/time/Timespan.h"
#include <utility>

namespace pickup {
namespace time {

const Timespan::TimeDiff Timespan::MILLISECONDS = 1000;  ///< 1 毫秒 = 1,000 微秒
const Timespan::TimeDiff Timespan::SECONDS = 1000000;    ///< 1 秒 = 1,000,000 微秒
const Timespan::TimeDiff Timespan::MINUTES = 60000000;   ///< 1 分钟 = 60,000,000 微秒
const Timespan::TimeDiff Timespan::HOURS = 3600000000;   ///< 1 小时 = 3,600,000,000 微秒
const Timespan::TimeDiff Timespan::DAYS = 86400000000;   ///< 1 天 = 86,400,000,000 微秒

Timespan::Timespan() : span_(0) {}

Timespan::Timespan(TimeDiff microseconds) : span_(microseconds) {}

Timespan::Timespan(long seconds, long microseconds) : span_(seconds * SECONDS + microseconds) {}

Timespan::Timespan(int days, int hours, int minutes, int seconds, int microseconds)
    : span_(days * DAYS + hours * HOURS + minutes * MINUTES + seconds * SECONDS + microseconds) {}

Timespan::Timespan(const Timespan& timespan) : span_(timespan.span_) {}

Timespan::~Timespan() {}

Timespan& Timespan::operator=(const Timespan& timespan) {
  if (this != &timespan) {
    span_ = timespan.span_;
  }
  return *this;
}

Timespan& Timespan::operator=(TimeDiff microseconds) {
  span_ = microseconds;
  return *this;
}

Timespan& Timespan::assign(int days, int hours, int minutes, int seconds, int microseconds) {
  span_ = days * DAYS + hours * HOURS + minutes * MINUTES + seconds * SECONDS + microseconds;
  return *this;
}

Timespan& Timespan::assign(long seconds, long microseconds) {
  span_ = seconds * SECONDS + microseconds;
  return *this;
}

void Timespan::swap(Timespan& timespan) { std::swap(span_, timespan.span_); }

bool Timespan::operator==(const Timespan& other) const { return span_ == other.span_; }
bool Timespan::operator!=(const Timespan& other) const { return span_ != other.span_; }
bool Timespan::operator>(const Timespan& other) const { return span_ > other.span_; }
bool Timespan::operator>=(const Timespan& other) const { return span_ >= other.span_; }
bool Timespan::operator<(const Timespan& other) const { return span_ < other.span_; }
bool Timespan::operator<=(const Timespan& other) const { return span_ <= other.span_; }

bool Timespan::operator==(TimeDiff microseconds) const { return span_ == microseconds; }
bool Timespan::operator!=(TimeDiff microseconds) const { return span_ != microseconds; }
bool Timespan::operator>(TimeDiff microseconds) const { return span_ > microseconds; }
bool Timespan::operator>=(TimeDiff microseconds) const { return span_ >= microseconds; }
bool Timespan::operator<(TimeDiff microseconds) const { return span_ < microseconds; }
bool Timespan::operator<=(TimeDiff microseconds) const { return span_ <= microseconds; }

Timespan Timespan::operator+(const Timespan& other) const { return Timespan(span_ + other.span_); }
Timespan Timespan::operator-(const Timespan& other) const { return Timespan(span_ - other.span_); }
Timespan& Timespan::operator+=(const Timespan& other) {
  span_ += other.span_;
  return *this;
}
Timespan& Timespan::operator-=(const Timespan& other) {
  span_ -= other.span_;
  return *this;
}
Timespan Timespan::operator+(TimeDiff microseconds) const { return Timespan(span_ + microseconds); }
Timespan Timespan::operator-(TimeDiff microseconds) const { return Timespan(span_ - microseconds); }
Timespan& Timespan::operator+=(TimeDiff microseconds) {
  span_ += microseconds;
  return *this;
}
Timespan& Timespan::operator-=(TimeDiff microseconds) {
  span_ -= microseconds;
  return *this;
}

int Timespan::days() const { return static_cast<int>(span_ / DAYS); }
int Timespan::hours() const { return static_cast<int>((span_ % DAYS) / HOURS); }
int Timespan::totalHours() const { return static_cast<int>(span_ / HOURS); }
int Timespan::minutes() const { return static_cast<int>((span_ % HOURS) / MINUTES); }
int Timespan::totalMinutes() const { return static_cast<int>(span_ / MINUTES); }
int Timespan::seconds() const { return static_cast<int>((span_ % MINUTES) / SECONDS); }
int Timespan::totalSeconds() const { return static_cast<int>(span_ / SECONDS); }
int Timespan::milliseconds() const { return static_cast<int>((span_ % SECONDS) / MILLISECONDS); }
Timespan::TimeDiff Timespan::totalMilliseconds() const { return static_cast<int>(span_ / MILLISECONDS); }
int Timespan::microseconds() const { return static_cast<int>(span_ % SECONDS); }
int Timespan::useconds() const { return static_cast<int>(span_ % 1000000); }
Timespan::TimeDiff Timespan::totalMicroseconds() const { return span_; }

}  // namespace time
}  // namespace pickup