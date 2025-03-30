#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace pickup {
namespace utils {

namespace detail {

/**
 * @brief 字节交换模板基类，提供不同尺寸类型的特化实现
 * @tparam T 需要交换字节的类型
 * @tparam sz 类型尺寸（字节数）
 *
 * 通过模板特化实现对不同大小类型的字节交换操作，支持1/2/4/8字节的基础类型和浮点类型
 */
template <typename T, std::size_t sz>
struct swap_bytes {
  // 编译期静态断言
  static_assert(sz == 1 || sz == 2 || sz == 4 || sz == 8, "Unsupported type size for byte swapping");

  // 使用constexpr和noexcept优化编译期计算
  constexpr T operator()(T val) const noexcept {
    return val;  // 实际由特化版本实现
  }
};

//---------------------------------------- 特化实现 ----------------------------------------
// 1字节类型（无操作）
template <typename T>
struct swap_bytes<T, 1> {
  constexpr T operator()(T val) const noexcept { return val; }
};

// 2字节类型（short, uint16_t等）
template <typename T>
struct swap_bytes<T, 2> {
  constexpr T operator()(T val) const noexcept {
    return static_cast<T>(((val & 0x00ff) << 8) | 
           static_cast<T>(((val & 0xff00) >> 8));
  }
};

// 4字节类型（int32_t, uint32_t, float等）
template <typename T>
struct swap_bytes<T, 4> {
  constexpr T operator()(T val) const noexcept {
    return static_cast<T>(((val & 0xff000000) >> 24) | ((val & 0x00ff0000) >> 8) | ((val & 0x0000ff00) << 8) |
                          ((val & 0x000000ff) << 24));
  }
};

// 8字节类型（int64_t, uint64_t, double等）
template <typename T>
struct swap_bytes<T, 8> {
  constexpr T operator()(T val) const noexcept {
    return static_cast<T>(((val & 0xff00000000000000ull) >> 56) | ((val & 0x00ff000000000000ull) >> 40) |
                          ((val & 0x0000ff0000000000ull) >> 24) | ((val & 0x000000ff00000000ull) >> 8) |
                          ((val & 0x00000000ff000000ull) << 8) | ((val & 0x0000000000ff0000ull) << 24) |
                          ((val & 0x000000000000ff00ull) << 40) | ((val & 0x00000000000000ffull) << 56));
  }
};

//---------------------------------------- 浮点类型特化 ----------------------------------------
// 避免类型双关（type punning）问题，使用memcpy安全转换
template <>
struct swap_bytes<float, 4> {
  float operator()(float val) const noexcept {
    uint32_t temp;
    std::memcpy(&temp, &val, sizeof(temp));  // 安全拷贝字节内容
    temp = swap_bytes<uint32_t, 4>()(temp);  // 交换整数形式
    std::memcpy(&val, &temp, sizeof(val));   // 拷贝回浮点数
    return val;
  }
};

template <>
struct swap_bytes<double, 8> {
  double operator()(double val) const noexcept {
    uint64_t temp;
    std::memcpy(&temp, &val, sizeof(temp));  // 安全拷贝字节内容
    temp = swap_bytes<uint64_t, 8>()(temp);  // 交换整数形式
    std::memcpy(&val, &temp, sizeof(val));   // 拷贝回浮点数
    return val;
  }
};

//---------------------------------------- 类型适配器 ----------------------------------------
/**
 * @brief 统一字节交换入口适配器
 * @tparam T 需要交换字节的类型
 */
template <class T>
struct do_byte_swap {
  constexpr T operator()(T value) const noexcept { return swap_bytes<T, sizeof(T)>()(value); }
};

}  // namespace detail

//---------------------------------------- 公有接口 ----------------------------------------
/**
 * @brief 字节顺序翻转函数
 * @tparam T 需要交换字节的基础数据类型
 * @param value 输入值
 * @return 字节顺序翻转后的值
 */
template <class T>
inline constexpr T flipBytes(T value) noexcept {
  // 编译期类型检查
  static_assert(std::is_arithmetic_v<T>, "FlipBytes only supports arithmetic types");
  static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8,
                "Unsupported type size for byte swapping");

  return detail::do_byte_swap<T>()(value);
}

}  // namespace utils
}  // namespace pickup