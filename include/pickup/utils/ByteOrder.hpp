#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

#include "pickup/utils/FlipBytes.hpp"

namespace pickup {
namespace utils {

namespace detail {

// 编译期字节序检测
constexpr bool is_little_endian() noexcept {
  constexpr uint16_t test = 0x0001;
  return reinterpret_cast<const uint8_t*>(&test)[0] == 0x01;
}

}  // namespace detail

//---------------------------------------- 公有接口扩展 ----------------------------------------
/**
 * @brief 编译期字节序常量
 */
constexpr bool kIsLittleEndian = detail::is_little_endian();

/**
 * @brief 从大端字节序转换回本机字节序
 * @tparam T 支持的基础数据类型
 */
template <class T>
inline constexpr T fromBigEndian(T value) noexcept {
  static_assert(std::is_arithmetic_v<T>, "Only arithmetic types are supported");
  if constexpr (kIsLittleEndian) {
    return flipBytes(value);
  }
  return value;
}

/**
 * @brief 从小端字节序转换回本机字节序
 * @tparam T 支持的基础数据类型
 */
template <class T>
inline constexpr T fromLittleEndian(T value) noexcept {
  static_assert(std::is_arithmetic_v<T>, "Only arithmetic types are supported");
  if constexpr (!kIsLittleEndian) {
    return flipBytes(value);
  }
  return value;
}

/**
 * @brief 将主机字节序转换为网络字节序（big endian）
 * @tparam T 支持的基础数据类型
 */
template <class T>
inline constexpr T hostToNetwork(T value) noexcept {
  static_assert(std::is_arithmetic_v<T>, "Only arithmetic types are supported");
  if constexpr (kIsLittleEndian) {
    return flipBytes(value);
  }
  return value;
}

/**
 * @brief 将网络字节序（big endian）转换为主机字节序
 * @tparam T 支持的基础数据类型
 */
template <class T>
inline constexpr T networkToHost(T value) noexcept {
  static_assert(std::is_arithmetic_v<T>, "Only arithmetic types are supported");
  if constexpr (kIsLittleEndian) {
    return flipBytes(value);
  }
  return value;
}

}  // namespace utils
}  // namespace pickup