#pragma once

#include <cmath>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace pickup {
namespace math {

constexpr float kPrecisionEpsilon = 1e-6f;  ///< 严格误差阈值（1e-6）
constexpr float kToleranceEpsilon = 1e-3f;  ///< 宽松误差阈值（1e-3）

/** @brief 数值比较 */

/**
 * @brief 精确比较整型数值是否相等
 * @tparam T 整型类型
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @return true 相等，false 不等
 */
template <typename T>
[[nodiscard]] typename std::enable_if<std::is_integral<T>::value, bool>::type Equal(const T& lhs,
                                                                                    const T& rhs) noexcept {
  return lhs == rhs;
}

/**
 * @brief 浮点数值近似相等比较（严格模式，阈值 kPrecisionEpsilon）
 * @tparam T 浮点类型
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @return |lhs - rhs| < kPrecisionEpsilon 时返回 true
 */
template <typename T>
[[nodiscard]] typename std::enable_if<std::is_floating_point<T>::value, bool>::type Equal(const T& lhs,
                                                                                           const T& rhs) noexcept {
  return std::fabs(lhs - rhs) < static_cast<T>(kPrecisionEpsilon);
}

/**
 * @brief 浮点数值近似相等比较（宽松模式，阈值 kToleranceEpsilon）
 * @tparam T 浮点类型
 * @param lhs 左操作数
 * @param rhs 右操作数
 * @return |lhs - rhs| < kToleranceEpsilon 时返回 true
 */
template <typename T>
[[nodiscard]] typename std::enable_if<std::is_floating_point<T>::value, bool>::type Near(const T& lhs,
                                                                                          const T& rhs) noexcept {
  return std::abs(lhs - rhs) < static_cast<T>(kToleranceEpsilon);
}

/** @brief 零值判断 */

/**
 * @brief 判断整型是否为零
 * @tparam T 整型类型
 * @param value 待判断值
 * @return value == 0 时返回 true
 */
template <typename T>
[[nodiscard]] bool EqualZero(T value) noexcept {
  static_assert(std::is_integral<T>::value, "Integral required.");
  return value == 0;
}

/**
 * @brief 判断浮点数是否近似为零（阈值 kPrecisionEpsilon）
 * @tparam T 浮点类型
 * @param value 待判断值
 * @return |value| < kPrecisionEpsilon 时返回 true
 */
template <typename T>
[[nodiscard]] typename std::enable_if<std::is_floating_point<T>::value, bool>::type EqualZero(T value) noexcept {
  return std::fabs(value) < static_cast<T>(kPrecisionEpsilon);
}

/** @brief 区间操作 */

/**
 * @brief 判断整型值是否在闭区间 [lower, upper] 内
 * @tparam T 整型类型
 * @param value 待判断值
 * @param lower 区间下界
 * @param upper 区间上界
 * @return lower ≤ value ≤ upper 时返回 true
 */
template <typename T>
[[nodiscard]] typename std::enable_if<std::is_integral<T>::value, bool>::type Between(T value, T lower,
                                                                                       T upper) noexcept {
  return (lower <= value) && (value <= upper);
}

/**
 * @brief 判断浮点值是否在区间内（边界允许 kPrecisionEpsilon 误差）
 * @tparam T 浮点类型
 * @param value 待判断值
 * @param lower 区间下界
 * @param upper 区间上界
 * @return (lower - ε) ≤ value ≤ (upper + ε) 时返回 true
 */
template <typename T>
[[nodiscard]] typename std::enable_if<std::is_floating_point<T>::value, bool>::type Between(T value, T lower,
                                                                                              T upper) noexcept {
  return (lower - kPrecisionEpsilon) <= value && value <= (upper + kPrecisionEpsilon);
}

/**
 * @brief 将数值夹紧到闭区间 [lower, upper]
 * @tparam T 数值类型
 * @param value 输入值
 * @param lower 区间下界
 * @param upper 区间上界
 * @return 区间内的最近值
 * @throws std::invalid_argument 若 lower > upper
 */
template <typename T>
[[nodiscard]] T FitIn(T value, T lower, T upper) {
  if (lower > upper) {
    throw std::invalid_argument("FitIn: lower > upper");
  }
  return (value < lower) ? lower : ((value > upper) ? upper : value);
}

/** @brief 符号操作 */

/**
 * @brief 获取数值符号
 * @tparam T 数值类型
 * @param value 输入值
 * @return value ≥ 0 时返回 1，value < 0 时返回 -1
 */
template <typename T>
[[nodiscard]] T Sign(const T& value) noexcept {
  return std::signbit(value) ? -1 : 1;
}

}  // namespace math
}  // namespace pickup
