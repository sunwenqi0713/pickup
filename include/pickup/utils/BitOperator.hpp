#pragma once
#include <bitset>
#include <iostream>

namespace pickup {
namespace utils {

template <typename T>
class BitOperator {
 public:
  BitOperator(T initialValue = 0) : data_(initialValue) {}
  ~BitOperator() = default;

  // 设置特定位为1
  void setBit(size_t position) { data_ |= (static_cast<T>(1) << position); }

  // 清除特定位（设置为0）
  void clearBit(size_t position) { data_ &= ~(static_cast<T>(1) << position); }

  // 取反特定位
  void toggleBit(size_t position) { data_ ^= (static_cast<T>(1) << position); }

  // 检查特定位是否为1
  bool checkBit(size_t position) const { return (data_ & (static_cast<T>(1) << position)) != 0; }

  // 获取当前数据值
  T getValue() const { return data_; }

  // 设置当前数据值
  void setValue(T newValue) { data_ = newValue; }

  // 打印二进制表示
  void print() const { std::cout << "Binary: " << std::bitset<sizeof(T) * 8>(data_) << std::endl; }

 private:
  T data_;  // 存储位操作的数据
};

}  // namespace utils
}  // namespace pickup