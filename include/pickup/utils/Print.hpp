#pragma once

#include <deque>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/**
 * @brief 美化打印功能的主命名空间
 * @details 提供对STL容器的递归格式化输出功能，支持嵌套结构显示
 */
namespace pickup {
namespace utils {
/**
 * @namespace detail
 * @brief 实现细节的命名空间（内部使用）
 */
namespace detail {

// 前置声明
template <class T>
std::string to_string(const T &obj, int indent);

/**
 * @brief 通用容器格式化函数（顺序容器）
 * @tparam T 容器类型
 * @param type 容器类型名称
 * @param c 容器引用
 * @param indent 当前缩进级别
 * @return 格式化后的字符串
 */
template <class T>
std::string to_string_container(const std::string &type, const T &c, int indent);

/**
 * @brief 关联容器格式化函数
 * @tparam T 容器类型
 * @param type 容器类型名称
 * @param c 容器引用
 * @param indent 当前缩进级别
 * @return 格式化后的字符串
 */
template <class T>
std::string to_string_assoc_container(const std::string &type, const T &c, int indent);

///////////////////////////////////////////////////////////////////////////////
// 基本类型处理
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief 字符串类型格式化
 * @param s 输入字符串
 * @param 未使用的缩进参数
 * @return 带双引号的字符串
 */
inline std::string to_string(const std::string &s, int = 0) { return "\"" + s + "\""; }

/**
 * @brief 算术类型格式化
 * @tparam T 算术类型（通过SFINAE限制）
 * @param n 数值
 * @param 未使用的缩进参数
 * @return 数值的字符串表示
 * @details 支持所有整数和浮点类型的标准转换
 */
template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, int>::type = 0>
inline std::string to_string(T n, int = 0) {
  return std::to_string(n);
}

///////////////////////////////////////////////////////////////////////////////
// STL容器特化实现
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief std::vector格式化
 * @tparam T 元素类型
 */
template <class T>
std::string to_string(const std::vector<T> &c, int indent) {
  return to_string_container("std::vector<>", c, indent);
}

/**
 * @brief std::list格式化
 * @tparam T 元素类型
 */
template <class T>
std::string to_string(const std::list<T> &c, int indent) {
  return to_string_container("std::list<>", c, indent);
}

/**
 * @brief std::deque格式化
 * @tparam T 元素类型
 */
template <class T>
std::string to_string(const std::deque<T> &c, int indent) {
  return to_string_container("std::deque<>", c, indent);
}

/**
 * @brief std::set格式化
 * @tparam T 元素类型
 */
template <class T>
std::string to_string(const std::set<T> &c, int indent) {
  return to_string_container("std::set<>", c, indent);
}

/**
 * @brief std::unordered_set格式化
 * @tparam T 元素类型
 */
template <class T>
std::string to_string(const std::unordered_set<T> &c, int indent) {
  return to_string_container("std::unordered_set<>", c, indent);
}

/**
 * @brief std::map格式化
 * @tparam K 键类型
 * @tparam V 值类型
 */
template <class K, class V>
std::string to_string(const std::map<K, V> &c, int indent) {
  return to_string_assoc_container("std::map<>", c, indent);
}

/**
 * @brief std::unordered_map格式化
 * @tparam K 键类型
 * @tparam V 值类型
 */
template <class K, class V>
std::string to_string(const std::unordered_map<K, V> &c, int indent) {
  return to_string_assoc_container("std::unordered_map<>", c, indent);
}

///////////////////////////////////////////////////////////////////////////////
// 核心实现细节
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief 顺序容器格式化核心实现
 * @tparam T 容器类型
 * @param type 容器类型名称
 * @param c 容器常量引用
 * @param indent 当前缩进级别（每级=4空格）
 * @return 格式化后的带缩进字符串
 * @details 生成格式示例：
 * @code
 * std::vector<>
 * (
 *     [0] => "element1"
 *     [1] => 123
 * )
 * @endcode
 */
template <class T>
std::string to_string_container(const std::string &type, const T &c, int indent) {
  std::stringstream ss;
  std::string current_indent(indent * 4, ' ');
  ss << current_indent << type << "\n" << current_indent << "(\n";

  size_t index = 0;
  std::string element_indent((indent + 1) * 4, ' ');
  for (const auto &elem : c) {
    ss << element_indent << "[" << index++ << "] => " << to_string(elem, indent + 1) << "\n";
  }

  ss << current_indent << ")\n";
  return ss.str();
}

/**
 * @brief 关联容器格式化核心实现
 * @tparam T 容器类型
 * @param type 容器类型名称
 * @param c 容器常量引用
 * @param indent 当前缩进级别
 * @return 格式化后的带缩进字符串
 * @details 生成格式示例：
 * @code
 * std::map<>
 * (
 *     [1] => "one"
 *     [2] => "two"
 * )
 * @endcode
 */
template <class T>
std::string to_string_assoc_container(const std::string &type, const T &c, int indent) {
  std::stringstream ss;
  std::string current_indent(indent * 4, ' ');
  ss << current_indent << type << "\n" << current_indent << "(\n";

  std::string element_indent((indent + 1) * 4, ' ');
  for (const auto &pair : c) {
    ss << element_indent << "[" << to_string(pair.first, indent + 1) << "] => " << to_string(pair.second, indent + 1)
       << "\n";
  }

  ss << current_indent << ")\n";
  return ss.str();
}

}  // namespace detail

///////////////////////////////////////////////////////////////////////////////
// 用户接口
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief 通用美化打印入口函数
 * @tparam T 要打印的类型
 * @param obj 要格式化的对象
 * @return 格式化后的字符串
 * @details 使用示例：
 * @code
 * std::vector<std::map<int, std::string>> v = {...};
 * std::cout << pickup::utils::to_string(v) << std::endl;
 * @endcode
 */
template <class T>
std::string to_string(const T &obj) {
  return detail::to_string(obj, 0);
}

}  // namespace utils
}  // namespace pickup
