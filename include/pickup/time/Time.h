#pragma once

#include <limits>
#include <string>

namespace pickup {
namespace time {

// 时间类，用于表示和操作时间，精度为纳秒
class Time {
 public:
  static const Time MAX;  // 最大时间值
  static const Time MIN;  // 最小时间值

  Time() = default;  // 默认构造函数，初始化为0纳秒

  // 通过不同时间单位构造时间对象
  explicit Time(uint64_t nanoseconds);           // 直接使用纳秒值初始化
  explicit Time(int nanoseconds);                // 整型纳秒构造（注意符号）
  explicit Time(double seconds);                 // 浮点型秒数构造
  Time(uint32_t seconds, uint32_t nanoseconds);  // 秒和纳秒分量构造
  Time(const Time& other);                       // 拷贝构造函数
  Time& operator=(const Time& other);            // 赋值运算符

  /**
   * @brief 获取当前系统时间（可能受系统时间调整影响）
   */
  static Time getCurrentTime();

  /**
   * @brief 获取当前单调时间（不受系统时间调整影响）
   */
  static Time getCurrentMonoTime();

  // 时间单位转换
  double toSecond() const;         // 转换为秒（浮点型，含小数部分）
  uint64_t toMicrosecond() const;  // 转换为微秒（整数）
  uint64_t toNanosecond() const;   // 转换为纳秒（整数）

  /**
   * @brief 判断是否为0时间
   */
  bool isZero() const;

  /**
   * @brief 转换为可读字符串格式
   */
  std::string toString() const;

  // 比较运算符重载
  bool operator==(const Time& rhs) const;
  bool operator!=(const Time& rhs) const;
  bool operator>(const Time& rhs) const;
  bool operator<(const Time& rhs) const;
  bool operator>=(const Time& rhs) const;
  bool operator<=(const Time& rhs) const;

 private:
  uint64_t nanoseconds_ = 0;  // 内部存储以纳秒为单位的时间
};

// 输出流运算符重载，用于直接输出时间对象
std::ostream& operator<<(std::ostream& os, const Time& rhs);

}  // namespace time
}  // namespace pickup