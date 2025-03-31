#pragma once

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace pickup {
namespace utils {

namespace detail {

// 布尔值字符串常量定义（小写）
constexpr const char* strue = "true";    ///< 表示true的字符串
constexpr const char* sfalse = "false";  ///< 表示false的字符串

/**
 * @class Converter
 * @brief 类型转换模板基类，提供类型转换的通用接口
 * @tparam To 目标类型
 * @tparam From 源类型
 */
template <typename To, typename From>
struct Converter {};  // 主模板留空，需特化

// ================== 数值类型转换特化 ================== //
/**
 * @brief 特化：字符串类型到int的转换
 * @tparam From 源类型
 */
template <typename From>
struct Converter<int, From> {
  static int convert(const From& from) { return std::atoi(from); }
};

/**
 * @brief 特化：字符串类型到long的转换
 * @tparam From 源类型
 */
template <typename From>
struct Converter<long, From> {
  static long convert(const From& from) { return std::atol(from); }
};

/**
 * @brief 特化：字符串类型到long long的转换
 * @tparam From 源类型
 */
template <typename From>
struct Converter<long long, From> {
  static long long convert(const From& from) { return std::atoll(from); }
};

/**
 * @brief 特化：字符串类型到double的转换
 * @tparam From 源类型
 */
template <typename From>
struct Converter<double, From> {
  static double convert(const From& from) { return std::atof(from); }
};

/**
 * @brief 特化：字符串类型到float的转换
 * @tparam From 源类型
 */
template <typename From>
struct Converter<float, From> {
  static float convert(const From& from) { return (float)std::atof(from); }
};

// ================== 布尔类型转换特化 ================== //
/**
 * @brief 特化：整数类型到bool的转换
 * @note 非零值转换为true，零值转换为false
 */
template <typename From>
struct Converter<bool, From> {
  static typename std::enable_if<std::is_integral<From>::value, bool>::type convert(From from) { return !!from; }
};

/**
 * @brief 检查字符串是否与给定布尔值字符串匹配（忽略大小写）
 * @param from 输入字符串
 * @param len 字符串长度
 * @param s 目标布尔字符串（"true"或"false"）
 * @return 匹配返回true，否则false
 */
bool check_bool_str(const char* from, size_t len, const char* s) {
  for (size_t i = 0; i < len; ++i) {
    if (std::tolower(from[i]) != s[i]) {  // 忽略大小写
      return false;
    }
  }
  return true;
}

/**
 * @brief 将字符串转换为布尔值
 * @throws std::invalid_argument 输入不符合"true"/"false"格式
 */
bool parse_bool(const char* from) {
  const size_t len = strlen(from);
  if (len != 4 && len != 5) {  // "true"(4), "false"(5)
    throw std::invalid_argument("Invalid bool string: " + std::string(from));
  }

  bool result = false;
  if (len == 4) {
    result = check_bool_str(from, len, strue);
    if (result) return true;
  } else {
    result = check_bool_str(from, len, sfalse);
    if (result) return false;
  }

  throw std::invalid_argument("Invalid bool string: " + std::string(from));
}

// 布尔类型特化（支持std::string, char*, const char*等）
template <>
struct Converter<bool, std::string> {
  static bool convert(const std::string& from) { return parse_bool(from.c_str()); }
};

// ================== 字符串类型转换特化 ================== //
/**
 * @brief 特化：任意类型到std::string的转换
 * @note 使用std::to_string进行转换
 */
template <typename From>
struct Converter<std::string, From> {
  static std::string convert(const From& from) { return std::to_string(from); }
};

}  // namespace detail

// ================== 对外接口 ================== //
/**
 * @fn lexical_cast
 * @brief 类型安全的值转换函数
 * @tparam To 目标类型
 * @tparam From 源类型
 * @param from 源值
 * @return 转换后的目标类型值
 * @throws std::invalid_argument 转换失败时抛出
 *
 * @code
 *   int num = lexical_cast<int>("123");
 *   bool flag = lexical_cast<bool>("true");
 * @endcode
 */
template <typename To, typename From>
typename std::enable_if<!std::is_same<To, From>::value, To>::type lexical_cast(const From& from) {
  return detail::Converter<To, From>::convert(from);
}

// 类型相同时的特化（直接返回）
template <typename To, typename From>
typename std::enable_if<std::is_same<To, From>::value, To>::type lexical_cast(const From& from) {
  return from;
}

}  // namespace utils
}  // namespace pickup