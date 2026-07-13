#pragma once

#include <string>

#include "pickup/time/Time.h"
#include "pickup/time/Timespan.h"

namespace pickup {
namespace time {

/**
 * @brief 时区（名称 + UTC 偏移 + 夏令时偏移）
 *
 * 封装某时区相对 UTC 的固定偏移与夏令时偏移，可在 UtcTime 与 LocalTime 之间
 * 换算。默认构造读取当前系统本地时区。
 *
 * @note 非线程安全；静态方法 utc() 线程安全。
 */
class Timezone {
 public:
  /** @brief 以当前系统本地时区初始化 */
  Timezone();

  /**
   * @brief 以给定名称与偏移初始化
   * @param name      时区名
   * @param offset    相对 UTC 的基准偏移
   * @param dstoffset 夏令时附加偏移（默认 0）
   */
  explicit Timezone(const std::string& name, const Timespan& offset,
                    const Timespan& dstoffset = Timespan::zero())
      : name_(name), offset_(offset), dstoffset_(dstoffset) {}

  Timezone(const Timezone&) = default;
  Timezone(Timezone&&) = default;
  ~Timezone() = default;

  Timezone& operator=(const Timezone&) = default;
  Timezone& operator=(Timezone&&) = default;

  // ---- 比较（基于总偏移）----
  friend bool operator==(const Timezone& tz1, const Timezone& tz2) noexcept { return tz1.total() == tz2.total(); }
  friend bool operator!=(const Timezone& tz1, const Timezone& tz2) noexcept { return tz1.total() != tz2.total(); }
  friend bool operator>(const Timezone& tz1, const Timezone& tz2) noexcept { return tz1.total() > tz2.total(); }
  friend bool operator<(const Timezone& tz1, const Timezone& tz2) noexcept { return tz1.total() < tz2.total(); }
  friend bool operator>=(const Timezone& tz1, const Timezone& tz2) noexcept { return tz1.total() >= tz2.total(); }
  friend bool operator<=(const Timezone& tz1, const Timezone& tz2) noexcept { return tz1.total() <= tz2.total(); }

  const std::string& name()     const noexcept { return name_; }       ///< 时区名
  const Timespan&    offset()   const noexcept { return offset_; }     ///< 基准偏移
  const Timespan&    daylight() const noexcept { return dstoffset_; }  ///< 夏令时偏移

  /** @brief 总偏移（基准 + 夏令时） */
  Timespan total() const noexcept { return offset_ + dstoffset_; }

  /** @brief 用本时区将 UTC 时间换算为本地时间 */
  LocalTime convert(const UtcTime& utctime) const { return LocalTime(utctime + total()); }
  /** @brief 用本时区将本地时间换算为 UTC 时间 */
  UtcTime convert(const LocalTime& localtime) const { return UtcTime(localtime - total()); }

  /** @brief UTC 时区（GMT，零偏移）。线程安全 */
  static Timezone utc() { return Timezone("GMT", Timespan::zero()); }
  /** @brief 当前系统本地时区 */
  static Timezone local() { return Timezone(); }

  /** @brief 交换两个实例 */
  void swap(Timezone& other) noexcept;

 protected:
  std::string name_;
  Timespan offset_;
  Timespan dstoffset_;
};

inline void swap(Timezone& tz1, Timezone& tz2) noexcept { tz1.swap(tz2); }

}  // namespace time
}  // namespace pickup