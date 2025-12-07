#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>

namespace pickup {
namespace utils {

// 基础字节交换函数
inline uint16_t byteswap16(uint16_t value) noexcept { return (value >> 8) | (value << 8); }

inline uint32_t byteswap32(uint32_t value) noexcept {
  return ((value & 0x000000FF) << 24) | ((value & 0x0000FF00) << 8) | ((value & 0x00FF0000) >> 8) |
         ((value & 0xFF000000) >> 24);
}

inline uint64_t byteswap64(uint64_t value) noexcept {
  return ((value & 0x00000000000000FFULL) << 56) | ((value & 0x000000000000FF00ULL) << 40) |
         ((value & 0x0000000000FF0000ULL) << 24) | ((value & 0x00000000FF000000ULL) << 8) |
         ((value & 0x000000FF00000000ULL) >> 8) | ((value & 0x0000FF0000000000ULL) >> 24) |
         ((value & 0x00FF000000000000ULL) >> 40) | ((value & 0xFF00000000000000ULL) >> 56);
}

// 模板化字节交换函数
// Modified from
// https://stackoverflow.com/questions/105252/how-do-i-convert-between-big-endian-and-little-endian-values-in-c
template <typename T, std::size_t N>
struct byte_swapper;

template <typename T>
struct byte_swapper<T, 1> {
  T operator()(T val) { return val; }
};

template <typename T>
struct byte_swapper<T, 2> {
  T operator()(T val) {
#if defined(__has_builtin) && __has_builtin(__builtin_bswap16)
    if constexpr (std::is_same_v<T, uint16_t> || std::is_same_v<T, int16_t>) {
      return static_cast<T>(__builtin_bswap16(static_cast<uint16_t>(value)));
    }
#elif defined(_MSC_VER)
    if constexpr (std::is_same_v<T, uint16_t> || std::is_same_v<T, int16_t>) {
      return static_cast<T>(_byteswap_ushort(static_cast<uint16_t>(value)));
    }
#endif
    return static_cast<T>(byteswap16(static_cast<uint16_t>(value)));
  }
};

template <typename T>
struct byte_swapper<T, 4> {
  T operator()(T val) {
#if defined(__has_builtin) && __has_builtin(__builtin_bswap32)
    if constexpr (std::is_integral_v<T>) {
      return static_cast<T>(__builtin_bswap32(static_cast<uint32_t>(value)));
    }
#elif defined(_MSC_VER)
    if constexpr (std::is_integral_v<T>) {
      return static_cast<T>(_byteswap_ulong(static_cast<uint32_t>(value)));
    }
#endif

    if constexpr (std::is_floating_point_v<T>) {
      uint32_t int_val;
      std::memcpy(&int_val, &value, sizeof(value));
      int_val = byteswap32(int_val);
      T result;
      std::memcpy(&result, &int_val, sizeof(result));
      return result;
    } else {
      return static_cast<T>(byteswap32(static_cast<uint32_t>(value)));
    }
  }
};

template <typename T>
struct byte_swapper<T, 8> {
  T operator()(T val) {
#if defined(__has_builtin) && __has_builtin(__builtin_bswap64)
    if constexpr (std::is_integral_v<T>) {
      return static_cast<T>(__builtin_bswap64(static_cast<uint64_t>(value)));
    }
#elif defined(_MSC_VER)
    if constexpr (std::is_integral_v<T>) {
      return static_cast<T>(_byteswap_uint64(static_cast<uint64_t>(value)));
    }
#endif

    if constexpr (std::is_floating_point_v<T>) {
      uint64_t int_val;
      std::memcpy(&int_val, &value, sizeof(value));
      int_val = byteswap64(int_val);
      T result;
      std::memcpy(&result, &int_val, sizeof(result));
      return result;
    } else {
      return static_cast<T>(byteswap64(static_cast<uint64_t>(value)));
    }
  }
};

template <typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
T byteswap(T value) {
  return byte_swapper<T, sizeof(T)>{}(value);
}

}  // namespace utils
}  // namespace pickup