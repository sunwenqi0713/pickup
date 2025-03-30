#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

#ifdef _MSC_VER
#include <stdlib.h>  // `_byteswap_*`
#endif

namespace pickup {
namespace utils {

namespace detail {

#if !defined(PICKUP_NO_BYTESWAP_BUILTINS)
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400)  // Visual Studio 2005及以上版本
#define PICKUP_HAVE_MSC_BYTESWAP 1
#endif
#elif defined(__clang__)
#if __has_builtin(__builtin_bswap32)  // 使用 Clang 特有的 __has_builtin 检查 __builtin_bswap32 是否存在
#define PICKUP_HAVE_GCC_BYTESWAP 1
#endif
#elif defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3))  // GCC 4.3及以上版本
#define PICKUP_HAVE_GCC_BYTESWAP 1
#endif
#endif

// 基础模板声明（保持未实现状态）
template <typename T, std::size_t sz>
struct swap_bytes {
  static_assert(sz == 1 || sz == 2 || sz == 4 || sz == 8, "Unsupported type size for byte swapping");
};

// 1字节类型特化（无操作）
template <typename T>
struct swap_bytes<T, 1> {
  static constexpr T swap(T val) noexcept { return val; }
};

// 2字节类型特化
template <typename T>
struct swap_bytes<T, 2> {
  static constexpr T swap(T val) noexcept {
#if defined(PICKUP_HAVE_MSC_BYTESWAP)
    return _byteswap_ushort(value);
#elif defined(PICKUP_HAVE_GCC_BYTESWAP)
    return __builtin_bswap16(val);
#else
    return static_cast<T>(((val & 0x00ff) << 8) | ((val & 0xff00) >> 8));
#endif
  }
};

// 4字节类型特化
template <typename T>
struct swap_bytes<T, 4> {
  static constexpr T swap(T val) noexcept {
#if defined(PICKUP_HAVE_MSC_BYTESWAP)
    return _byteswap_ulong(val);
#elif defined(PICKUP_HAVE_GCC_BYTESWAP)
    return __builtin_bswap32(val);
#else
    return static_cast<T>(((val & 0xff000000) >> 24) | ((val & 0x00ff0000) >> 8) | ((val & 0x0000ff00) << 8) |
                          ((val & 0x000000ff) << 24));
#endif
  }
};

// 8字节类型特化
template <typename T>
struct swap_bytes<T, 8> {
  static constexpr T swap(T val) noexcept {
#if defined(PICKUP_HAVE_MSC_BYTESWAP)
    return _byteswap_uint64(val);
#elif defined(PICKUP_HAVE_GCC_BYTESWAP)
    return __builtin_bswap64(val);
#else
    return static_cast<T>(((val & 0xff00000000000000ull) >> 56) | ((val & 0x00ff000000000000ull) >> 40) |
                          ((val & 0x0000ff0000000000ull) >> 24) | ((val & 0x000000ff00000000ull) >> 8) |
                          ((val & 0x00000000ff000000ull) << 8) | ((val & 0x0000000000ff0000ull) << 24) |
                          ((val & 0x000000000000ff00ull) << 40) | ((val & 0x00000000000000ffull) << 56));
#endif
  }
};

// 浮点类型特化（通过整型间接处理）
template <>
struct swap_bytes<float, 4> {
  static float swap(float val) noexcept {
    uint32_t temp;
    std::memcpy(&temp, &val, sizeof(temp));
    temp = swap_bytes<uint32_t, 4>::swap(temp);
    std::memcpy(&val, &temp, sizeof(val));
    return val;
  }
};

template <>
struct swap_bytes<double, 8> {
  static double swap(double val) noexcept {
    uint64_t temp;
    std::memcpy(&temp, &val, sizeof(temp));
    temp = swap_bytes<uint64_t, 8>::swap(temp);
    std::memcpy(&val, &temp, sizeof(val));
    return val;
  }
};

}  // namespace detail

/**
 * @brief 字节顺序翻转函数
 * @tparam T 支持的基础数据类型（整型/浮点型）
 * @param value 输入值
 * @return 字节顺序翻转后的值
 */
template <class T>
inline constexpr auto flipBytes(T value) noexcept {
  static_assert(std::is_arithmetic_v<T>, "Only arithmetic types are supported");
  static_assert(sizeof(T) <= 8, "Maximum supported size is 8 bytes");

  return detail::swap_bytes<T, sizeof(T)>::swap(value);
}

}  // namespace utils
}  // namespace pickup