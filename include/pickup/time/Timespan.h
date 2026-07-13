#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>

namespace pickup {
namespace time {

/**
 * @brief 相对时间度量（时间跨度）
 *
 * 内部以 int64_t 纳秒存储，避免浮点累积误差；可为正或负。
 * 提供各时间单位的换算与工厂方法。
 *
 * int64 纳秒的可表示范围约为 ±292 年，足以覆盖任意实际时间跨度。
 *
 * 若需要表示某个绝对时刻（自 epoch 起的时间点），请参阅 Timestamp；
 * 若需要日历字段（年/月/日…），请参阅 Time。
 *
 * @note 非线程安全。
 */
class Timespan {
 public:
  /** @brief 构造零时间跨度 */
  constexpr Timespan() noexcept : duration_(0) {}

  /**
   * @brief 从纳秒数构造
   * @param nanoseconds 纳秒数，可正可负
   */
  explicit constexpr Timespan(int64_t nanoseconds) noexcept : duration_(nanoseconds) {}

  /**
   * @brief 从任意 std::chrono::duration 构造（精度截断到纳秒）
   */
  template <class Rep, class Period>
  explicit Timespan(const std::chrono::duration<Rep, Period>& duration) noexcept
      : duration_(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count()) {}

  Timespan(const Timespan&) noexcept = default;
  Timespan(Timespan&&) noexcept = default;
  ~Timespan() noexcept = default;

  Timespan& operator=(int64_t nanoseconds) noexcept {
    duration_ = nanoseconds;
    return *this;
  }
  Timespan& operator=(const Timespan&) noexcept = default;
  Timespan& operator=(Timespan&&) noexcept = default;

  constexpr Timespan operator+() const noexcept { return Timespan(+duration_); }
  constexpr Timespan operator-() const noexcept { return Timespan(-duration_); }

  constexpr Timespan& operator+=(int64_t offset) noexcept { duration_ += offset; return *this; }
  constexpr Timespan& operator+=(const Timespan& offset) noexcept { duration_ += offset.duration_; return *this; }
  constexpr Timespan& operator-=(int64_t offset) noexcept { duration_ -= offset; return *this; }
  constexpr Timespan& operator-=(const Timespan& offset) noexcept { duration_ -= offset.duration_; return *this; }

  /** @brief 转换为 std::chrono 纳秒时长 */
  std::chrono::nanoseconds chrono() const noexcept { return std::chrono::nanoseconds(duration_); }

  // 以对应单位表示的总量（向零取整），而非该单位内的分量
  constexpr int64_t days()         const noexcept { return duration_ / (24 * 60 * 60 * 1000000000ll); }
  constexpr int64_t hours()        const noexcept { return duration_ / (60 * 60 * 1000000000ll); }
  constexpr int64_t minutes()      const noexcept { return duration_ / (60 * 1000000000ll); }
  constexpr int64_t seconds()      const noexcept { return duration_ / 1000000000ll; }
  constexpr int64_t milliseconds() const noexcept { return duration_ / 1000000ll; }
  constexpr int64_t microseconds() const noexcept { return duration_ / 1000ll; }
  constexpr int64_t nanoseconds()  const noexcept { return duration_; }

  /** @brief 返回底层总纳秒数 */
  constexpr int64_t total() const noexcept { return duration_; }

  static constexpr Timespan days(int64_t days)                 noexcept { return Timespan(days * (24 * 60 * 60 * 1000000000ll)); }
  static constexpr Timespan hours(int64_t hours)               noexcept { return Timespan(hours * (60 * 60 * 1000000000ll)); }
  static constexpr Timespan minutes(int64_t minutes)           noexcept { return Timespan(minutes * (60 * 1000000000ll)); }
  static constexpr Timespan seconds(int64_t seconds)           noexcept { return Timespan(seconds * 1000000000ll); }
  static constexpr Timespan milliseconds(int64_t milliseconds) noexcept { return Timespan(milliseconds * 1000000ll); }
  static constexpr Timespan microseconds(int64_t microseconds) noexcept { return Timespan(microseconds * 1000ll); }
  static constexpr Timespan nanoseconds(int64_t nanoseconds)   noexcept { return Timespan(nanoseconds); }

  /** @brief 返回零时间跨度 */
  static constexpr Timespan zero() noexcept { return Timespan(0); }

  /**
   * @brief 格式化为可读字符串，如 "1d2h3m4s567ms"、"-1500us"、"0ns"
   * @details 从最高非零单位起，依次输出 d/h/m/s/ms/us/ns 中的非零分量；
   *          负跨度前缀 '-'。
   */
  std::string toString() const;

  /** @brief 交换两个实例 */
  void swap(Timespan& other) noexcept {
    using std::swap;
    swap(duration_, other.duration_);
  }

 private:
  int64_t duration_;
};

constexpr Timespan operator+(const Timespan& t, int64_t offset) noexcept { return Timespan(t.total() + offset); }
constexpr Timespan operator+(int64_t offset, const Timespan& t) noexcept { return Timespan(offset + t.total()); }
constexpr Timespan operator+(const Timespan& t1, const Timespan& t2) noexcept { return Timespan(t1.total() + t2.total()); }

constexpr Timespan operator-(const Timespan& t, int64_t offset) noexcept { return Timespan(t.total() - offset); }
constexpr Timespan operator-(int64_t offset, const Timespan& t) noexcept { return Timespan(offset - t.total()); }
constexpr Timespan operator-(const Timespan& t1, const Timespan& t2) noexcept { return Timespan(t1.total() - t2.total()); }

constexpr bool operator==(const Timespan& t, int64_t offset) noexcept { return t.total() == offset; }
constexpr bool operator==(int64_t offset, const Timespan& t) noexcept { return offset == t.total(); }
constexpr bool operator==(const Timespan& t1, const Timespan& t2) noexcept { return t1.total() == t2.total(); }

constexpr bool operator!=(const Timespan& t, int64_t offset) noexcept { return t.total() != offset; }
constexpr bool operator!=(int64_t offset, const Timespan& t) noexcept { return offset != t.total(); }
constexpr bool operator!=(const Timespan& t1, const Timespan& t2) noexcept { return t1.total() != t2.total(); }

constexpr bool operator>(const Timespan& t, int64_t offset) noexcept { return t.total() > offset; }
constexpr bool operator>(int64_t offset, const Timespan& t) noexcept { return offset > t.total(); }
constexpr bool operator>(const Timespan& t1, const Timespan& t2) noexcept { return t1.total() > t2.total(); }

constexpr bool operator<(const Timespan& t, int64_t offset) noexcept { return t.total() < offset; }
constexpr bool operator<(int64_t offset, const Timespan& t) noexcept { return offset < t.total(); }
constexpr bool operator<(const Timespan& t1, const Timespan& t2) noexcept { return t1.total() < t2.total(); }

constexpr bool operator>=(const Timespan& t, int64_t offset) noexcept { return t.total() >= offset; }
constexpr bool operator>=(int64_t offset, const Timespan& t) noexcept { return offset >= t.total(); }
constexpr bool operator>=(const Timespan& t1, const Timespan& t2) noexcept { return t1.total() >= t2.total(); }

constexpr bool operator<=(const Timespan& t, int64_t offset) noexcept { return t.total() <= offset; }
constexpr bool operator<=(int64_t offset, const Timespan& t) noexcept { return offset <= t.total(); }
constexpr bool operator<=(const Timespan& t1, const Timespan& t2) noexcept { return t1.total() <= t2.total(); }

inline void swap(Timespan& t1, Timespan& t2) noexcept { t1.swap(t2); }

}  // namespace time
}  // namespace pickup