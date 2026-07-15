#include "pickup/time/Timespan.h"

#include <string>

namespace pickup {
namespace time {

std::string Timespan::toString() const {
  if (duration_ == 0) return "0ns";

  const bool negative = duration_ < 0;
  // 用无符号取绝对值，避免 INT64_MIN 取负溢出
  uint64_t magnitude = negative ? (0ull - static_cast<uint64_t>(duration_))
                                : static_cast<uint64_t>(duration_);

  struct Unit {
    uint64_t ns;
    const char* suffix;
  };
  static const Unit units[] = {
      {86400000000000ull, "d"},
      {3600000000000ull,  "h"},
      {60000000000ull,    "m"},
      {1000000000ull,     "s"},
      {1000000ull,        "ms"},
      {1000ull,           "us"},
      {1ull,              "ns"},
  };

  std::string out;
  if (negative) out = "-";
  for (const auto& u : units) {
    const uint64_t q = magnitude / u.ns;
    magnitude %= u.ns;
    if (q != 0) {
      out += std::to_string(q);
      out += u.suffix;
    }
  }
  return out;
}

}  // namespace time
}  // namespace pickup