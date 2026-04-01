#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>

#if defined(_MSC_VER)
#include <intrin.h>
#pragma intrinsic(_byteswap_ushort)
#pragma intrinsic(_byteswap_ulong)
#pragma intrinsic(_byteswap_uint64)
#elif defined(__GNUC__) || defined(__clang__)
/** @brief GCC/Clang 内置函数无需额外头文件 */
#endif

namespace pickup {
namespace utils {

/**
 * @brief 交换 16 位整数的字节序
 * @param value 16 位无符号整数
 * @return 字节交换后的值
 */
inline uint16_t byteswap16(uint16_t value) noexcept { return (value >> 8) | (value << 8); }

/**
 * @brief 交换 32 位整数的字节序
 * @param value 32 位无符号整数
 * @return 字节交换后的值
 */
inline uint32_t byteswap32(uint32_t value) noexcept {
  return ((value & 0x000000FFu) << 24) | ((value & 0x0000FF00u) << 8) | ((value & 0x00FF0000u) >> 8) |
         ((value & 0xFF000000u) >> 24);
}

/**
 * @brief 交换 64 位整数的字节序
 * @param value 64 位无符号整数
 * @return 字节交换后的值
 */
inline uint64_t byteswap64(uint64_t value) noexcept {
  return ((value & 0x00000000000000FFull) << 56) | ((value & 0x000000000000FF00ull) << 40) |
         ((value & 0x0000000000FF0000ull) << 24) | ((value & 0x00000000FF000000ull) << 8) |
         ((value & 0x000000FF00000000ull) >> 8) | ((value & 0x0000FF0000000000ull) >> 24) |
         ((value & 0x00FF000000000000ull) >> 40) | ((value & 0xFF00000000000000ull) >> 56);
}

namespace detail {

inline uint16_t builtin_byteswap16(uint16_t value) noexcept {
#if defined(__has_builtin) && __has_builtin(__builtin_bswap16)
  return __builtin_bswap16(value);
#elif defined(_MSC_VER)
  return _byteswap_ushort(value);
#else
  return byteswap16(value);
#endif
}

inline uint32_t builtin_byteswap32(uint32_t value) noexcept {
#if defined(__has_builtin) && __has_builtin(__builtin_bswap32)
  return __builtin_bswap32(value);
#elif defined(_MSC_VER)
  return _byteswap_ulong(value);
#else
  return byteswap32(value);
#endif
}

inline uint64_t builtin_byteswap64(uint64_t value) noexcept {
#if defined(__has_builtin) && __has_builtin(__builtin_bswap64)
  return __builtin_bswap64(value);
#elif defined(_MSC_VER)
  return _byteswap_uint64(value);
#else
  return byteswap64(value);
#endif
}

enum class TypeSize : std::size_t { Size1 = 1, Size2 = 2, Size4 = 4, Size8 = 8, Other = 0 };

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

template <typename T>
inline T byteswap_float(T value) noexcept {
  static_assert(std::is_floating_point_v<T>, "T must be a floating point type");

  if constexpr (sizeof(T) == 4) {
    uint32_t int_val;
    std::memcpy(&int_val, &value, sizeof(value));
    int_val = builtin_byteswap32(int_val);
    T result;
    std::memcpy(&result, &int_val, sizeof(result));
    return result;
  } else if constexpr (sizeof(T) == 8) {
    uint64_t int_val;
    std::memcpy(&int_val, &value, sizeof(value));
    int_val = builtin_byteswap64(int_val);
    T result;
    std::memcpy(&result, &int_val, sizeof(result));
    return result;
  } else {
    static_assert(sizeof(T) == 4 || sizeof(T) == 8, "Only 32-bit and 64-bit floating point types are supported");
    return value;
  }
}

}  // namespace detail

/**
 * @brief 通用字节序交换函数
 *
 * 支持整数类型（1、2、4、8 字节）及浮点类型（float、double）。
 * 非标准大小的类型将触发编译期断言。
 *
 * @tparam T 算术类型
 * @param value 要交换字节序的值
 * @return 字节交换后的值
 */
template <typename T>
[[nodiscard]] inline T byteswap(T value) noexcept {
  static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type");

  constexpr auto type_size = detail::get_type_size<T>();

  if constexpr (type_size == detail::TypeSize::Size1) {
    return value;
  } else if constexpr (type_size == detail::TypeSize::Size2 && std::is_integral_v<T>) {
    using UnsignedType = std::make_unsigned_t<T>;
    return static_cast<T>(static_cast<UnsignedType>(detail::builtin_byteswap16(
        static_cast<uint16_t>(static_cast<UnsignedType>(value)))));
  } else if constexpr (type_size == detail::TypeSize::Size4 && std::is_integral_v<T>) {
    using UnsignedType = std::make_unsigned_t<T>;
    return static_cast<T>(static_cast<UnsignedType>(detail::builtin_byteswap32(
        static_cast<uint32_t>(static_cast<UnsignedType>(value)))));
  } else if constexpr (type_size == detail::TypeSize::Size8 && std::is_integral_v<T>) {
    using UnsignedType = std::make_unsigned_t<T>;
    return static_cast<T>(static_cast<UnsignedType>(detail::builtin_byteswap64(
        static_cast<uint64_t>(static_cast<UnsignedType>(value)))));
  } else if constexpr (std::is_floating_point_v<T>) {
    return detail::byteswap_float(value);
  } else {
    static_assert(type_size != detail::TypeSize::Other, "Unsupported type size for byteswap");
    return value;
  }
}

}  // namespace utils
}  // namespace pickup
