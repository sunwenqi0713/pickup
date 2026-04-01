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

namespace pickup {
namespace utils {

namespace detail {

/** @brief 前置声明 */
template <typename T>
std::string toString(const T& obj, int indent);

template <typename T>
std::string toStringContainer(const std::string& type, const T& c, int indent);

template <typename T>
std::string toStringAssocContainer(const std::string& type, const T& c, int indent);

/** @brief 基本类型 */

inline std::string toString(const std::string& s, int = 0) { return "\"" + s + "\""; }

template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, int>::type = 0>
inline std::string toString(T n, int = 0) {
  return std::to_string(n);
}

/** @brief STL 容器特化 */

template <typename T>
std::string toString(const std::vector<T>& c, int indent) {
  return toStringContainer("std::vector<>", c, indent);
}

template <typename T>
std::string toString(const std::list<T>& c, int indent) {
  return toStringContainer("std::list<>", c, indent);
}

template <typename T>
std::string toString(const std::deque<T>& c, int indent) {
  return toStringContainer("std::deque<>", c, indent);
}

template <typename T>
std::string toString(const std::set<T>& c, int indent) {
  return toStringContainer("std::set<>", c, indent);
}

template <typename T>
std::string toString(const std::unordered_set<T>& c, int indent) {
  return toStringContainer("std::unordered_set<>", c, indent);
}

template <typename K, typename V>
std::string toString(const std::map<K, V>& c, int indent) {
  return toStringAssocContainer("std::map<>", c, indent);
}

template <typename K, typename V>
std::string toString(const std::unordered_map<K, V>& c, int indent) {
  return toStringAssocContainer("std::unordered_map<>", c, indent);
}

/** @brief 核心实现 */

template <typename T>
std::string toStringContainer(const std::string& type, const T& c, int indent) {
  std::stringstream ss;
  std::string currentIndent(indent * 4, ' ');
  ss << currentIndent << type << "\n" << currentIndent << "(\n";

  size_t index = 0;
  std::string elementIndent((indent + 1) * 4, ' ');
  for (const auto& elem : c) {
    ss << elementIndent << "[" << index++ << "] => " << toString(elem, indent + 1) << "\n";
  }

  ss << currentIndent << ")\n";
  return ss.str();
}

template <typename T>
std::string toStringAssocContainer(const std::string& type, const T& c, int indent) {
  std::stringstream ss;
  std::string currentIndent(indent * 4, ' ');
  ss << currentIndent << type << "\n" << currentIndent << "(\n";

  std::string elementIndent((indent + 1) * 4, ' ');
  for (const auto& pair : c) {
    ss << elementIndent << "[" << toString(pair.first, indent + 1) << "] => "
       << toString(pair.second, indent + 1) << "\n";
  }

  ss << currentIndent << ")\n";
  return ss.str();
}

}  // namespace detail

/**
 * @brief 将 STL 容器或基本类型格式化为可读字符串
 * @tparam T 要格式化的类型
 * @param obj 要格式化的对象
 * @return 格式化后的字符串
 *
 * @code
 * std::vector<std::map<int, std::string>> v = {...};
 * std::cout << pickup::utils::toString(v) << std::endl;
 * @endcode
 */
template <typename T>
[[nodiscard]] std::string toString(const T& obj) {
  return detail::toString(obj, 0);
}

}  // namespace utils
}  // namespace pickup
