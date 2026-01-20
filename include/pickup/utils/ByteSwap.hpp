#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>

#if defined(_MSC_VER)
    #include <intrin.h>     // Windows内置函数
    #pragma intrinsic(_byteswap_ushort)
    #pragma intrinsic(_byteswap_ulong)
    #pragma intrinsic(_byteswap_uint64)
#elif defined(__GNUC__) || defined(__clang__)
    // GCC和Clang内置函数无需额外头文件
#else
    // 其他编译器
#endif
namespace pickup {
namespace utils {

// 基础字节交换函数

/**
 * @brief   交换16位整数的字节序
 *
 * @param[in]   value 16位无符号整数
 * @return      字节交换后的16位无符号整数
 */
inline uint16_t byteswap16(uint16_t value) noexcept { return (value >> 8) | (value << 8); }

/**
 * @brief   交换32位整数的字节序
 *
 * @param[in]   value 32位无符号整数
 * @return      字节交换后的32位无符号整数
 */
inline uint32_t byteswap32(uint32_t value) noexcept {
  return ((value & 0x000000FF) << 24) | ((value & 0x0000FF00) << 8) | ((value & 0x00FF0000) >> 8) |
         ((value & 0xFF000000) >> 24);
}

/**
 * @brief   交换64位整数的字节序
 *
 * @param[in]   value 64位无符号整数
 * @return      字节交换后的64位无符号整数
 */
inline uint64_t byteswap64(uint64_t value) noexcept {
  return ((value & 0x00000000000000FFULL) << 56) | ((value & 0x000000000000FF00ULL) << 40) |
         ((value & 0x0000000000FF0000ULL) << 24) | ((value & 0x00000000FF000000ULL) << 8) |
         ((value & 0x000000FF00000000ULL) >> 8) | ((value & 0x0000FF0000000000ULL) >> 24) |
         ((value & 0x00FF000000000000ULL) >> 40) | ((value & 0xFF00000000000000ULL) >> 56);
}

// 编译器内置函数封装
namespace detail {

/**
 * @brief   使用编译器内置函数交换16位整数字节序
 *
 * @param[in]   value 16位无符号整数
 * @return      字节交换后的16位无符号整数
 */
inline uint16_t builtin_byteswap16(uint16_t value) noexcept {
#if defined(__has_builtin) && __has_builtin(__builtin_bswap16)
  return __builtin_bswap16(value);
#elif defined(_MSC_VER)
  return _byteswap_ushort(value);
#else
  return byteswap16(value);
#endif
}

/**
 * @brief   使用编译器内置函数交换32位整数字节序
 *
 * @param[in]   value 32位无符号整数
 * @return      字节交换后的32位无符号整数
 */
inline uint32_t builtin_byteswap32(uint32_t value) noexcept {
#if defined(__has_builtin) && __has_builtin(__builtin_bswap32)
  return __builtin_bswap32(value);
#elif defined(_MSC_VER)
  return _byteswap_ulong(value);
#else
  return byteswap32(value);
#endif
}

/**
 * @brief   使用编译器内置函数交换64位整数字节序
 *
 * @param[in]   value 64位无符号整数
 * @return      字节交换后的64位无符号整数
 */
inline uint64_t builtin_byteswap64(uint64_t value) noexcept {
#if defined(__has_builtin) && __has_builtin(__builtin_bswap64)
  return __builtin_bswap64(value);
#elif defined(_MSC_VER)
  return _byteswap_uint64(value);
#else
  return byteswap64(value);
#endif
}

}  // namespace detail

// 类型特性辅助
namespace detail {

/**
 * @brief   类型大小分类
 */
enum class TypeSize : std::size_t { Size1 = 1, Size2 = 2, Size4 = 4, Size8 = 8, Other = 0 };

/**
 * @brief   获取类型的大小分类
 */
template <typename T>
constexpr TypeSize get_type_size() noexcept {
  constexpr std::size_t size = sizeof(T);
  if constexpr (size == 1)
    return TypeSize::Size1;
  else if constexpr (size == 2)
    return TypeSize::Size2;
  else if constexpr (size == 4)
    return TypeSize::Size4;
  else if constexpr (size == 8)
    return TypeSize::Size8;
  else
    return TypeSize::Other;
}

}  // namespace detail

// 浮点数字节交换

namespace detail {

/**
 * @brief   交换浮点数的字节序（通过内存拷贝）
 *
 * @tparam  T 浮点类型（float或double）
 * @param[in]   value 浮点数
 * @return      字节交换后的浮点数
 */
template <typename T>
inline T byteswap_float(T value) noexcept {
  static_assert(std::is_floating_point_v<T>, "T must be a floating point type");

  if constexpr (sizeof(T) == 4) {
    // float类型（32位）
    uint32_t int_val;
    std::memcpy(&int_val, &value, sizeof(value));
    int_val = builtin_byteswap32(int_val);
    T result;
    std::memcpy(&result, &int_val, sizeof(result));
    return result;
  } else if constexpr (sizeof(T) == 8) {
    // double类型（64位）
    uint64_t int_val;
    std::memcpy(&int_val, &value, sizeof(value));
    int_val = builtin_byteswap64(int_val);
    T result;
    std::memcpy(&result, &int_val, sizeof(result));
    return result;
  } else {
    // 不支持其他大小的浮点数
    static_assert(sizeof(T) == 4 || sizeof(T) == 8, "Only 32-bit and 64-bit floating point types are supported");
    return value;
  }
}

}  // namespace detail

// 主字节交换函数

/**
 * @brief   通用字节交换函数
 *
 * @tparam  T 要交换字节序的类型
 * @param[in]   value 要交换字节序的值
 * @return      字节交换后的值
 *
 * @note    支持整数类型（1, 2, 4, 8字节）和浮点类型（float, double）
 * @warning 对于非标准大小的类型会编译失败
 */
template <typename T>
inline T byteswap(T value) noexcept {
  static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type");

  constexpr auto type_size = detail::get_type_size<T>();

  // 1字节类型不需要交换
  if constexpr (type_size == detail::TypeSize::Size1) {
    return value;
  }

  // 2字节整数类型
  else if constexpr (type_size == detail::TypeSize::Size2 && std::is_integral_v<T>) {
    using UnsignedType = std::make_unsigned_t<T>;
    auto unsigned_val = static_cast<UnsignedType>(value);
    auto swapped = detail::builtin_byteswap16(static_cast<uint16_t>(unsigned_val));
    return static_cast<T>(static_cast<UnsignedType>(swapped));
  }

  // 4字节整数类型
  else if constexpr (type_size == detail::TypeSize::Size4 && std::is_integral_v<T>) {
    using UnsignedType = std::make_unsigned_t<T>;
    auto unsigned_val = static_cast<UnsignedType>(value);
    auto swapped = detail::builtin_byteswap32(static_cast<uint32_t>(unsigned_val));
    return static_cast<T>(static_cast<UnsignedType>(swapped));
  }

  // 8字节整数类型
  else if constexpr (type_size == detail::TypeSize::Size8 && std::is_integral_v<T>) {
    using UnsignedType = std::make_unsigned_t<T>;
    auto unsigned_val = static_cast<UnsignedType>(value);
    auto swapped = detail::builtin_byteswap64(static_cast<uint64_t>(unsigned_val));
    return static_cast<T>(static_cast<UnsignedType>(swapped));
  }

  // 浮点类型
  else if constexpr (std::is_floating_point_v<T>) {
    return detail::byteswap_float(value);
  }

  // 不支持的类型
  else {
    static_assert(type_size != detail::TypeSize::Other, "Unsupported type size for byteswap");
    return value;
  }
}

}  // namespace utils
}  // namespace pickup