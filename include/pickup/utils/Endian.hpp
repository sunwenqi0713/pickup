#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <utility>

#include "pickup/utils/ByteSwap.hpp"

namespace pickup {
namespace utils {

// 字节序检测和转换

/**
 * @brief   字节序类型枚举
 */
enum class Endian {
  Little,  ///< 小端字节序
  Big,     ///< 大端字节序
  Unknown  ///< 未知字节序
};

/**
 * @brief   检测当前系统的字节序
 *
 * @return  系统字节序
 */
constexpr Endian system_endian() noexcept {
  constexpr uint32_t test_value = 0x01020304;
  constexpr uint8_t first_byte = static_cast<const uint8_t*>(static_cast<const void*>(&test_value))[0];

  if constexpr (first_byte == 0x01) {
    return Endian::Big;
  } else if constexpr (first_byte == 0x04) {
    return Endian::Little;
  } else {
    return Endian::Unknown;
  }
}

/**
 * @brief   如果需要则交换字节序
 *
 * @tparam  T 要处理的类型
 * @param[in]   value 要处理的值
 * @param[in]   from  原始字节序
 * @param[in]   to    目标字节序
 * @return      转换后的值
 *
 * @note    如果from和to相同，直接返回原值
 */
template <typename T>
inline T convert_endian(T value, Endian from, Endian to) noexcept {
  if (from == to || from == Endian::Unknown || to == Endian::Unknown) {
    return value;
  }
  return byteswap(value);
}

/**
 * @brief   从主机字节序转换到网络字节序（大端）
 *
 * @tparam  T 要转换的类型
 * @param[in]   value 主机字节序的值
 * @return      网络字节序的值
 */
template <typename T>
inline T host_to_network(T value) noexcept {
  if constexpr (system_endian() == Endian::Little) {
    return byteswap(value);  // 小端转大端
  } else {
    return value;  // 大端系统直接返回
  }
}

/**
 * @brief   从网络字节序（大端）转换到主机字节序
 *
 * @tparam  T 要转换的类型
 * @param[in]   value 网络字节序的值
 * @return      主机字节序的值
 */
template <typename T>
inline T network_to_host(T value) noexcept {
  return host_to_network(value);  // 转换是对称的
}

// 传统命名别名
template <typename T>
inline T hton(T value) noexcept {
  return host_to_network(value);
}

template <typename T>
inline T ntoh(T value) noexcept {
  return network_to_host(value);
}

// 带类型的别名
inline uint16_t htons(uint16_t value) noexcept { return host_to_network(value); }
inline uint32_t htonl(uint32_t value) noexcept { return host_to_network(value); }
inline uint64_t htonll(uint64_t value) noexcept { return host_to_network(value); }

inline uint16_t ntohs(uint16_t value) noexcept { return network_to_host(value); }
inline uint32_t ntohl(uint32_t value) noexcept { return network_to_host(value); }
inline uint64_t ntohll(uint64_t value) noexcept { return network_to_host(value); }

// 批量转换函数

/**
 * @brief   批量转换字节序（原地）
 *
 * @tparam  T 元素类型
 * @param[in,out] data  数据指针
 * @param[in]     count 元素数量
 */
template <typename T>
inline void byteswap_inplace(T* data, std::size_t count) noexcept {
  for (std::size_t i = 0; i < count; ++i) {
    data[i] = byteswap(data[i]);
  }
}

/**
 * @brief   批量转换字节序（复制）
 *
 * @tparam  T 元素类型
 * @param[in]  src   源数据指针
 * @param[out] dst   目标数据指针
 * @param[in]  count 元素数量
 */
template <typename T>
inline void byteswap_copy(const T* src, T* dst, std::size_t count) noexcept {
  for (std::size_t i = 0; i < count; ++i) {
    dst[i] = byteswap(src[i]);
  }
}

}  // namespace utils
}  // namespace pickup