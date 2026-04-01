#pragma once

#include <cstddef>

namespace pickup {
namespace utils {

/**
 * @brief 位操作工具，封装对整数类型的位级读写
 * @tparam T 底层整数类型（如 uint8_t、uint32_t 等）
 */
template <typename T>
class BitOperator {
 public:
  /** @brief 构造，默认初始值为 0 */
  explicit BitOperator(T initialValue = 0) noexcept : data_(initialValue) {}

  ~BitOperator() = default;

  /** @brief 将指定位置的位设为 1 */
  void setBit(size_t position) noexcept { data_ |= (static_cast<T>(1) << position); }

  /** @brief 将指定位置的位清为 0 */
  void clearBit(size_t position) noexcept { data_ &= ~(static_cast<T>(1) << position); }

  /** @brief 将指定位置的位取反 */
  void toggleBit(size_t position) noexcept { data_ ^= (static_cast<T>(1) << position); }

  /** @brief 检查指定位置的位是否为 1 */
  [[nodiscard]] bool checkBit(size_t position) const noexcept {
    return (data_ & (static_cast<T>(1) << position)) != 0;
  }

  /** @brief 返回当前存储的整数值 */
  [[nodiscard]] T getValue() const noexcept { return data_; }

  /** @brief 设置整数值 */
  void setValue(T newValue) noexcept { data_ = newValue; }

 private:
  T data_;  ///< 存储位操作的数据
};

}  // namespace utils
}  // namespace pickup
