#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>

#include "pickup/time/Timespan.h"

namespace pickup {
namespace time {

/**
 * @brief 绝对时刻（自 Unix epoch 起的纳秒数）
 *
 * 以 int64_t 纳秒存储，epoch 为 UTC 1970-01-01 00:00:00。采用有符号整数，可表示
 * epoch 之前的时刻并与 Timespan 保持一致；可表示范围约为 1678-01 至 2262-04
 * （int64 纳秒的上下界）。
 *
 * 当前时刻通过静态时钟源 utc()/local()/nano()/rdts() 获取。
 *
 * @note 非线程安全；静态时钟源函数为线程安全。
 * @see Timespan, Time
 */
class Timestamp {
 public:
  /** @brief 初始化为 epoch（0） */
  constexpr Timestamp() noexcept : timestamp_(0) {}

  /**
   * @brief 从纳秒时刻构造
   * @param timestamp 自 epoch 起的纳秒数
   */
  explicit constexpr Timestamp(int64_t timestamp) noexcept : timestamp_(timestamp) {}

  /**
   * @brief 从任意 std::chrono 时间点构造（精度截断到纳秒）
   */
  template <class Clock, class Duration>
  explicit Timestamp(const std::chrono::time_point<Clock, Duration>& time_point) noexcept
      : timestamp_(std::chrono::duration_cast<std::chrono::nanoseconds>(time_point.time_since_epoch()).count()) {}

  Timestamp(const Timestamp&) noexcept = default;
  Timestamp(Timestamp&&) noexcept = default;
  ~Timestamp() noexcept = default;

  Timestamp& operator=(int64_t timestamp) noexcept {
    timestamp_ = timestamp;
    return *this;
  }
  Timestamp& operator=(const Timestamp&) noexcept = default;
  Timestamp& operator=(Timestamp&&) noexcept = default;

  constexpr Timestamp& operator+=(int64_t offset) noexcept { timestamp_ += offset; return *this; }
  constexpr Timestamp& operator+=(const Timespan& offset) noexcept { timestamp_ += offset.total(); return *this; }
  constexpr Timestamp& operator-=(int64_t offset) noexcept { timestamp_ -= offset; return *this; }
  constexpr Timestamp& operator-=(const Timespan& offset) noexcept { timestamp_ -= offset.total(); return *this; }

  /** @brief 转换为 std::chrono::system_clock 时间点 */
  std::chrono::system_clock::time_point chrono() const noexcept {
    return std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        std::chrono::time_point<std::chrono::system_clock>() + std::chrono::nanoseconds(timestamp_));
  }

  // 自 epoch 起以对应单位表示的总量（向零取整）
  constexpr int64_t days()         const noexcept { return timestamp_ / (24 * 60 * 60 * 1000000000ll); }
  constexpr int64_t hours()        const noexcept { return timestamp_ / (60 * 60 * 1000000000ll); }
  constexpr int64_t minutes()      const noexcept { return timestamp_ / (60 * 1000000000ll); }
  constexpr int64_t seconds()      const noexcept { return timestamp_ / 1000000000ll; }
  constexpr int64_t milliseconds() const noexcept { return timestamp_ / 1000000ll; }
  constexpr int64_t microseconds() const noexcept { return timestamp_ / 1000ll; }
  constexpr int64_t nanoseconds()  const noexcept { return timestamp_; }

  /** @brief 返回底层总纳秒数 */
  constexpr int64_t total() const noexcept { return timestamp_; }

  static constexpr Timestamp days(int64_t days)                 noexcept { return Timestamp(days * (24 * 60 * 60 * 1000000000ll)); }
  static constexpr Timestamp hours(int64_t hours)               noexcept { return Timestamp(hours * (60 * 60 * 1000000000ll)); }
  static constexpr Timestamp minutes(int64_t minutes)           noexcept { return Timestamp(minutes * (60 * 1000000000ll)); }
  static constexpr Timestamp seconds(int64_t seconds)           noexcept { return Timestamp(seconds * 1000000000ll); }
  static constexpr Timestamp milliseconds(int64_t milliseconds) noexcept { return Timestamp(milliseconds * 1000000ll); }
  static constexpr Timestamp microseconds(int64_t microseconds) noexcept { return Timestamp(microseconds * 1000ll); }
  static constexpr Timestamp nanoseconds(int64_t nanoseconds)   noexcept { return Timestamp(nanoseconds); }

  /** @brief epoch 时刻（0），线程安全 */
  static constexpr int64_t epoch() noexcept { return 0; }

  /**
   * @brief 当前 UTC 挂钟时刻（纳秒，自 epoch）
   * @details 非单调，随系统时钟调整而变化。线程安全。
   */
  static int64_t utc();

  /**
   * @brief 当前本地挂钟时刻（纳秒，自 epoch，含本地时区偏移）
   * @details 线程安全。
   */
  static int64_t local();

  /**
   * @brief 高分辨率单调时钟（纳秒）
   * @details 单调递增，不受系统时钟调整影响，适用于计时/性能测量。
   *          其零点未定义，仅两次读数之差有意义。线程安全。
   */
  static int64_t nano();

  /**
   * @brief 读取 CPU 时间戳计数器（RDTSC，自复位以来的时钟周期数）
   * @details 不保证跨核一致或频率恒定，仅用于极短区间的粗略计数；
   *          不支持的平台回退到 nano()。线程安全。
   */
  static int64_t rdts();

  /** @brief 交换两个实例 */
  void swap(Timestamp& other) noexcept {
    using std::swap;
    swap(timestamp_, other.timestamp_);
  }

 protected:
  int64_t timestamp_;
};

constexpr Timestamp operator+(const Timestamp& ts, int64_t offset) noexcept { return Timestamp(ts.total() + offset); }
constexpr Timestamp operator+(int64_t offset, const Timestamp& ts) noexcept { return Timestamp(offset + ts.total()); }
constexpr Timestamp operator+(const Timestamp& ts, const Timespan& offset) noexcept { return Timestamp(ts.total() + offset.total()); }
constexpr Timestamp operator+(const Timespan& offset, const Timestamp& ts) noexcept { return Timestamp(offset.total() + ts.total()); }

constexpr Timestamp operator-(const Timestamp& ts, int64_t offset) noexcept { return Timestamp(ts.total() - offset); }
constexpr Timestamp operator-(const Timestamp& ts, const Timespan& offset) noexcept { return Timestamp(ts.total() - offset.total()); }
constexpr Timespan  operator-(const Timestamp& ts1, const Timestamp& ts2) noexcept { return Timespan(ts1.total() - ts2.total()); }

constexpr bool operator==(const Timestamp& t1, const Timestamp& t2) noexcept { return t1.total() == t2.total(); }
constexpr bool operator!=(const Timestamp& t1, const Timestamp& t2) noexcept { return t1.total() != t2.total(); }
constexpr bool operator>(const Timestamp& t1, const Timestamp& t2) noexcept { return t1.total() > t2.total(); }
constexpr bool operator<(const Timestamp& t1, const Timestamp& t2) noexcept { return t1.total() < t2.total(); }
constexpr bool operator>=(const Timestamp& t1, const Timestamp& t2) noexcept { return t1.total() >= t2.total(); }
constexpr bool operator<=(const Timestamp& t1, const Timestamp& t2) noexcept { return t1.total() <= t2.total(); }

inline void swap(Timestamp& t1, Timestamp& t2) noexcept { t1.swap(t2); }

// UtcTimestamp / LocalTimestamp 用类型标注"该时刻按 UTC 还是本地解释得来"，
// 作为 Time::utcstamp() / localstamp() 的返回类型；默认构造分别取当前 UTC /
// 本地挂钟时刻。其余时钟源（epoch/nano/rdts）直接调用对应静态函数即可。

/** @brief 以"当前 UTC 时刻"初始化的具名包装 */
class UtcTimestamp : public Timestamp {
 public:
  using Timestamp::Timestamp;
  UtcTimestamp() : Timestamp(Timestamp::utc()) {}
  UtcTimestamp(const Timestamp& timestamp) noexcept : Timestamp(timestamp) {}
};

/** @brief 以"当前本地时刻"初始化的具名包装 */
class LocalTimestamp : public Timestamp {
 public:
  using Timestamp::Timestamp;
  LocalTimestamp() : Timestamp(Timestamp::local()) {}
  LocalTimestamp(const Timestamp& timestamp) noexcept : Timestamp(timestamp) {}
};

}  // namespace time
}  // namespace pickup