#pragma once

#include <cctype>
#include <cerrno>
#include <charconv>
#include <cstdlib>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

namespace pickup {
namespace utils {

namespace detail {

// helpers
inline bool iequals(std::string_view a, std::string_view b) {
  if (a.size() != b.size()) return false;
  for (size_t i = 0; i < a.size(); ++i) {
    if (std::tolower(static_cast<unsigned char>(a[i])) != std::tolower(static_cast<unsigned char>(b[i]))) {
      return false;
    }
  }
  return true;
}

// parsing
template <typename T>
std::optional<T> parseInteger(std::string_view sv) {
  static_assert(std::is_integral_v<T> && !std::is_same_v<T, bool>);
  T value{};
  auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), value);
  if (ec != std::errc{} || ptr != sv.data() + sv.size()) {
    return std::nullopt;
  }
  return value;
}

inline std::optional<float> parseFloat(std::string_view sv) {
  char* end = nullptr;
  errno = 0;
  float v = std::strtof(sv.data(), &end);
  if (errno != 0 || end != sv.data() + sv.size()) {
    return std::nullopt;
  }
  return v;
}

inline std::optional<double> parseDouble(std::string_view sv) {
  char* end = nullptr;
  errno = 0;
  double v = std::strtod(sv.data(), &end);
  if (errno != 0 || end != sv.data() + sv.size()) {
    return std::nullopt;
  }
  return v;
}

inline std::optional<bool> parseBool(std::string_view sv) {
  if (iequals(sv, "true")) return true;
  if (iequals(sv, "false")) return false;
  return std::nullopt;
}

}  // namespace detail

// lexical_cast
template <typename To, typename From>
std::optional<To> lexical_cast(const From& from) {
  // same type
  if constexpr (std::is_same_v<To, From>) {
    return from;
  }

  // string sources
  else if constexpr (std::is_same_v<From, std::string> || std::is_same_v<From, std::string_view> ||
                     std::is_same_v<From, const char*>) {
    std::string_view sv = std::is_same_v<From, const char*> ? std::string_view(from) : std::string_view(from);

    if constexpr (std::is_same_v<To, std::string>) {
      return std::string(sv);
    } else if constexpr (std::is_same_v<To, bool>) {
      return detail::parseBool(sv);
    } else if constexpr (std::is_integral_v<To>) {
      return detail::parseInteger<To>(sv);
    } else if constexpr (std::is_same_v<To, float>) {
      return detail::parseFloat(sv);
    } else if constexpr (std::is_same_v<To, double>) {
      return detail::parseDouble(sv);
    } else {
      static_assert(!sizeof(To), "Unsupported lexical_cast target type");
    }
  }

  // arithmetic to string
  else if constexpr (std::is_same_v<To, std::string> && std::is_arithmetic_v<From>) {
    return std::to_string(from);
  }

  // integral to bool
  else if constexpr (std::is_same_v<To, bool> && std::is_integral_v<From>) {
    return from != 0;
  }

  else {
    static_assert(!sizeof(To), "Unsupported lexical_cast conversion");
  }
}

}  // namespace utils
}  // namespace pickup
