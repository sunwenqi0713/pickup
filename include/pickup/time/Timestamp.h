#pragma once

#include <chrono>
#include <cstdint>
#include <ctime>
#include <string>

namespace pickup {
namespace time {

/**
 * @brief Timestamp 类表示一个单调递增的时间值，理论精度为微秒级。
 * Timestamp 支持相互比较和简单的算术运算。
 * 注意：Timestamp 的单调性依赖于系统时钟的单调性（例如，系统时钟不应被回拨）。
 * Timestamp 基于 UTC（协调世界时），因此与系统时区无关。
 */
class Timestamp {
 public:
  using TimeVal = std::int64_t;   ///< 单调递增的 UTC 时间值（微秒精度）
  using TimeDiff = std::int64_t;  ///< 两个时间戳的差值（微秒单位）

  // 创建当前时间的 Timestamp
  Timestamp();

  // 通过指定 TimeVal 值创建 Timestamp
  Timestamp(TimeVal tv);

  // 拷贝构造函数
  Timestamp(const Timestamp& other);

  // 析构函数
  ~Timestamp();

  Timestamp& operator=(const Timestamp& other);
  Timestamp& operator=(TimeVal tv);

  // 与另一个 Timestamp 交换值
  void swap(Timestamp& timestamp);

  // 更新为当前时间
  void update();

  bool operator==(const Timestamp& ts) const;
  bool operator!=(const Timestamp& ts) const;
  bool operator>(const Timestamp& ts) const;
  bool operator>=(const Timestamp& ts) const;
  bool operator<(const Timestamp& ts) const;
  bool operator<=(const Timestamp& ts) const;

  Timestamp operator+(TimeDiff d) const;
  Timestamp operator-(TimeDiff d) const;
  TimeDiff operator-(const Timestamp& ts) const;
  Timestamp& operator+=(TimeDiff d);
  Timestamp& operator-=(TimeDiff d);

  /**
   * @return  自 Unix 纪元以来的毫秒数
   * @note 该时间戳的精度为毫秒级
   */
  TimeVal epochMilliseconds() const;
  /**
   * @return 自 Unix 纪元以来的微秒数
   * @note 该时间戳的精度为微秒级
   */
  TimeVal epochMicroseconds() const;

  /**
   * @return 自该时间戳表示的时间起经过的时间
   */
  TimeDiff elapsed() const;

  /**
   * @param interval 时间间隔
   * @return 如果指定时间间隔已从该时间戳表示的时间起经过，则返回 true
   */
  bool isElapsed(const TimeDiff interval) const;

 private:
  TimeVal tv_;  ///< 内部时间戳（微秒单位，自 1970-01-01 UTC 起）
};

}  // namespace time
}  // namespace pickup