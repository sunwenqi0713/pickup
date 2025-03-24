#include "pickup/time/time.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <limits>
#include <sstream>
#include <thread>

#include "pickup/utils/platform.h"

namespace pickup {
namespace time {

const Time Time::MAX = Time(std::numeric_limits<uint64_t>::max());
const Time Time::MIN = Time(0);

Time::Time(uint64_t nanoseconds) { nanoseconds_ = nanoseconds; }

Time::Time(int nanoseconds) { nanoseconds_ = static_cast<uint64_t>(nanoseconds); }

Time::Time(double seconds) { nanoseconds_ = static_cast<uint64_t>(seconds * 1000000000UL); }

Time::Time(uint32_t seconds, uint32_t nanoseconds) {
  nanoseconds_ = static_cast<uint64_t>(seconds) * 1000000000UL + nanoseconds;
}

Time::Time(const Time& other) { nanoseconds_ = other.nanoseconds_; }

Time& Time::operator=(const Time& other) {
  this->nanoseconds_ = other.nanoseconds_;
  return *this;
}

Time Time::getCurrentTime() {
  auto now = std::chrono::high_resolution_clock::now();
  auto time_point = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
  auto epoch = time_point.time_since_epoch();
  uint64_t now_nano = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch).count();
  return Time(now_nano);
}

Time Time::getCurrentMonoTime() {
  auto now = std::chrono::steady_clock::now();
  auto time_point = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
  auto epoch = time_point.time_since_epoch();
  uint64_t now_nano = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch).count();
  return Time(now_nano);
}

double Time::toSecond() const { return static_cast<double>(nanoseconds_) / 1000000000UL; }

uint64_t Time::toNanosecond() const { return nanoseconds_; }

uint64_t Time::toMicrosecond() const { return static_cast<uint64_t>(nanoseconds_ / 1000.0); }

bool Time::isZero() const { return nanoseconds_ == 0; }

std::string Time::toString() const {
  auto tp = std::chrono::system_clock::time_point{} +
            std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::nanoseconds(nanoseconds_));
  auto time = std::chrono::system_clock::to_time_t(tp);
  struct tm tm;
#ifdef PICKUP_WIN_OS
  auto ret = localtime_s(&tm, &time);
  if (ret != 0) {
    return std::to_string(static_cast<double>(nanoseconds_) / 1000000000.0);
  }
#else
  auto ret = localtime_r(&time, &tm);
  if (ret == nullptr) {
    return std::to_string(static_cast<double>(nanoseconds_) / 1000000000.0);
  }
#endif
  std::stringstream ss;
  ss << std::put_time(&tm, "%F %T");
  ss << "." << std::setw(9) << std::setfill('0') << nanoseconds_ % 1000000000UL;
  return ss.str();
}

bool Time::operator==(const Time& rhs) const { return nanoseconds_ == rhs.nanoseconds_; }

bool Time::operator!=(const Time& rhs) const { return nanoseconds_ != rhs.nanoseconds_; }

bool Time::operator>(const Time& rhs) const { return nanoseconds_ > rhs.nanoseconds_; }

bool Time::operator<(const Time& rhs) const { return nanoseconds_ < rhs.nanoseconds_; }

bool Time::operator>=(const Time& rhs) const { return nanoseconds_ >= rhs.nanoseconds_; }

bool Time::operator<=(const Time& rhs) const { return nanoseconds_ <= rhs.nanoseconds_; }

std::ostream& operator<<(std::ostream& os, const Time& rhs) {
  os << rhs.toString();
  return os;
}

}  // namespace time
}  // namespace pickup
