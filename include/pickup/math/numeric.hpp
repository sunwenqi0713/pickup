#pragma once

#include <cmath>
#include <limits>  // std::numeric_limits
#include <type_traits>

namespace pickup {
namespace math {

/// @defgroup 数值工具
/// @brief 提供跨类型的数值操作工具（比较/区间处理/符号判断）

// 浮点数判定阈值常量定义
constexpr float kPrecisionEpsilon = 1e-6f;  //!< 严格误差阈值（1e-6）
constexpr float kToleranceEpsilon = 1e-3f;  //!< 宽松误差阈值（1e-3）

//----------------------- 数值比较 -----------------------

/**
 * @brief 精确比较整型数值是否相等
 * @tparam T 整型类型（通过SFINAE约束）
 * @param[in] lhs 左操作数
 * @param[in] rhs 右操作数
 * @return true 相等，false 不等
 * @note 对整型直接使用 == 运算符
 */
template <typename T>
typename std::enable_if<std::is_integral<T>::value, bool>::type Equal(const T& lhs, const T& rhs) noexcept {
  return lhs == rhs;
}

/**
 * @brief 浮点数值近似相等比较（严格模式）
 * @tparam T 浮点类型（通过SFINAE约束）
 * @param[in] lhs 左操作数
 * @param[in] rhs 右操作数
 * @return |lhs - rhs| < kPrecisionEpsilon 时返回true
 * @warning 注意浮点误差累积效应，建议在算法关键点使用
 */
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, bool>::type Equal(const T& lhs, const T& rhs) noexcept {
  return std::fabs(lhs - rhs) < static_cast<T>(kPrecisionEpsilon);
}

/**
 * @brief 浮点数值近似相等比较（宽松模式）
 * @tparam T 浮点类型（通过SFINAE约束）
 * @param[in] lhs 左操作数
 * @param[in] rhs 右操作数
 * @return |lhs - rhs| < kToleranceEpsilon 时返回true
 */
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, bool>::type Near(const T& lhs, const T& rhs) noexcept {
  return std::abs(lhs - rhs) < static_cast<T>(kToleranceEpsilon);
}

//----------------------- 零值判断 -----------------------

/**
 * @brief 判断整型是否为零
 * @tparam T 整型类型
 * @param[in] v 待判断值
 * @return v == 0 时返回true
 */
template <typename T>
bool EqualZero(T value) noexcept {
  static_assert(std::is_integral<T>::value, "Integral required.");
  return value == 0;
}

/**
 * @brief 判断浮点数是否近似为零
 * @tparam T 浮点类型
 * @param[in] v 待判断值
 * @return |v| < kPrecisionEpsilon 时返回true
 */
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, bool>::type EqualZero(T value) noexcept {
  return std::fabs(value) < static_cast<T>(kPrecisionEpsilon);
}

//----------------------- 区间操作 -----------------------

/**
 * @brief 判断整型值是否在区间内
 * @tparam T 整型类型
 * @param[in] value 待判断值
 * @param[in] lower 区间下界
 * @param[in] upper 区间上界
 * @return lower ≤ value ≤ upper 时返回true
 * @pre lower ≤ upper，否则永远返回false
 */
template <typename T>
typename std::enable_if<std::is_integral<T>::value, bool>::type Between(T value, T lower, T upper) noexcept {
  return (lower <= value) && (value <= upper);
}

/**
 * @brief 判断值是否在区间内（浮点型扩展边界）
 * @tparam T 浮点类型
 * @param[in] value 待判断值
 * @param[in] lower 区间下界
 * @param[in] upper 区间上界
 * @return (lower - ε) ≤ value ≤ (upper + ε) 时返回true
 * @pre lower ≤ upper + 2ε，否则可能产生逻辑矛盾
 */
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, bool>::type Between(T value, T lower, T upper) noexcept {
  return (lower - kPrecisionEpsilon) <= value && value <= (upper + kPrecisionEpsilon);
}

/**
 * @brief 将数值限制在指定区间内
 * @tparam T 数值类型
 * @param[in] value 输入值
 * @param[in] lower 区间下界
 * @param[in] upper 区间上界
 * @return 若value在[lower, upper]内返回原值，否则返回最近的边界
 * @throw 若lower > upper时抛出std::invalid_argument
 */
template <typename T>
T FitIn(T value, T lower, T upper) {
  if (lower > upper) {
    throw std::invalid_argument("FitIn: lower > upper");
  }
  return (value < lower) ? lower : ((value > upper) ? upper : value);
}

//----------------------- 符号操作 -----------------------

/**
 * @brief 获取数值符号
 * @tparam T 数值类型
 * @param[in] value 输入值
 * @return x≥0时返回1，x<0时返回-1
 * @note 对整型0返回1，对浮点-0.0返回1
 */
template <typename T>
T Sign(const T& value) noexcept {
  return std::signbit(value) ? -1 : 1;
}

}  // namespace math
}  // namespace pickup