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

inline bool iequals(std::string_view a, std::string_view b) {
  if (a.size() != b.size()) return false;
  for (size_t i = 0; i < a.size(); ++i) {
    if (std::tolower(static_cast<unsigned char>(a[i])) != std::tolower(static_cast<unsigned char>(b[i]))) {
      return false;
    }
  }
  return true;
}

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

/**
 * @brief 类型安全的字面量转换
 *
 * 支持：字符串 ↔ 整数/浮点/bool，相同类型直接返回，算术类型 → 字符串。
 * 转换失败返回 std::nullopt，不抛异常。
 *
 * @tparam To   目标类型
 * @tparam From 源类型
 * @param from  源值
 * @return      转换结果，失败时为 std::nullopt
 */
template <typename To, typename From>
[[nodiscard]] std::optional<To> lexicalCast(const From& from) {
  /** @brief 相同类型直接返回 */
  if constexpr (std::is_same_v<To, From>) {
    return from;
  }

  /** @brief 字符串源 → 各目标类型 */
  else if constexpr (std::is_same_v<From, std::string> || std::is_same_v<From, std::string_view> ||
                     std::is_same_v<From, const char*>) {
    std::string_view sv(from);

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
      static_assert(!sizeof(To), "Unsupported lexicalCast target type");
    }
  }

  /** @brief 算术类型 → 字符串 */
  else if constexpr (std::is_same_v<To, std::string> && std::is_arithmetic_v<From>) {
    return std::to_string(from);
  }

  /** @brief 整数 → bool */
  else if constexpr (std::is_same_v<To, bool> && std::is_integral_v<From>) {
    return from != 0;
  }

  else {
    static_assert(!sizeof(To), "Unsupported lexicalCast conversion");
  }
}

}  // namespace utils
}  // namespace pickup
