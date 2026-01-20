#include "pickup/time/Timespan.h"

namespace pickup {
namespace time {

Timespan::Timespan(const double secs) noexcept : seconds_(secs) {}
Timespan::Timespan(const Timespan& other) noexcept : seconds_(other.seconds_) {}
Timespan::~Timespan() noexcept {}

Timespan Timespan::milliseconds(int milliseconds) noexcept { return Timespan((double)milliseconds * 0.001); }
Timespan Timespan::milliseconds(int64_t milliseconds) noexcept { return Timespan((double)milliseconds * 0.001); }
Timespan Timespan::seconds(double s) noexcept { return Timespan(s); }
Timespan Timespan::minutes(double numberOfMinutes) noexcept { return Timespan(numberOfMinutes * 60.0); }
Timespan Timespan::hours(double numberOfHours) noexcept { return Timespan(numberOfHours * (60.0 * 60.0)); }
Timespan Timespan::days(double numberOfDays) noexcept { return Timespan(numberOfDays * (60.0 * 60.0 * 24.0)); }
Timespan Timespan::weeks(double numberOfWeeks) noexcept { return Timespan(numberOfWeeks * (60.0 * 60.0 * 24.0 * 7.0)); }

int64_t Timespan::inMilliseconds() const noexcept { return (int64_t)(seconds_ * 1000.0); }
double Timespan::inMinutes() const noexcept { return seconds_ / 60.0; }
double Timespan::inHours() const noexcept { return seconds_ / (60.0 * 60.0); }
double Timespan::inDays() const noexcept { return seconds_ / (60.0 * 60.0 * 24.0); }
double Timespan::inWeeks() const noexcept { return seconds_ / (60.0 * 60.0 * 24.0 * 7.0); }

Timespan& Timespan::operator=(const Timespan& other) noexcept {
  seconds_ = other.seconds_;
  return *this;
}

Timespan Timespan::operator+=(Timespan t) noexcept {
  seconds_ += t.seconds_;
  return *this;
}
Timespan Timespan::operator-=(Timespan t) noexcept {
  seconds_ -= t.seconds_;
  return *this;
}
Timespan Timespan::operator+=(double secs) noexcept {
  seconds_ += secs;
  return *this;
}
Timespan Timespan::operator-=(double secs) noexcept {
  seconds_ -= secs;
  return *this;
}

Timespan operator+(Timespan t1, Timespan t2) noexcept { return t1 += t2; }
Timespan operator-(Timespan t1, Timespan t2) noexcept { return t1 -= t2; }

bool operator==(Timespan t1, Timespan t2) noexcept { return t1.inSeconds() == t2.inSeconds(); }
bool operator!=(Timespan t1, Timespan t2) noexcept { return !(t1 == t2); }
bool operator>(Timespan t1, Timespan t2) noexcept { return t1.inSeconds() > t2.inSeconds(); }
bool operator<(Timespan t1, Timespan t2) noexcept { return t1.inSeconds() < t2.inSeconds(); }
bool operator>=(Timespan t1, Timespan t2) noexcept { return t1.inSeconds() >= t2.inSeconds(); }
bool operator<=(Timespan t1, Timespan t2) noexcept { return t1.inSeconds() <= t2.inSeconds(); }

}  // namespace time
}  // namespace pickup
