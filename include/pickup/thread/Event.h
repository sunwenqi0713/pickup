#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace pickup {
namespace thread {

/**
 * @brief 线程同步事件类，用于线程间的通知与等待
 *
 * 此类提供了一种线程间事件通知的机制，支持手动重置和自动重置模式。
 * - 手动重置模式（manualReset = true）：事件被触发后，所有等待线程都会被唤醒，直到显式调用reset()重置事件状态。
 * - 自动重置模式（manualReset = false）：事件被触发后，仅唤醒一个等待线程，然后自动重置事件状态。
 *
 * 示例用法：
 * @code
 * Event event(true); // 手动重置事件
 * event.signal();    // 触发事件
 * event.wait(Event::TIMEOUT_INFINITE); // 无限等待事件
 * @endcode
 */
class Event {
 public:
  static constexpr int64_t TIMEOUT_INFINITE = -1;  // 无限等待超时值，调用 wait 时将阻塞直到事件被触发
  static constexpr int64_t TIMEOUT_IMMEDIATE = 0;  // 立即返回超时值，调用 wait 时将立即返回当前事件状态

  /**
   * @brief 构造事件对象
   * @param manualReset 指定事件是否为手动重置模式：
   * - true  : 事件触发后需手动调用 reset() 重置状态
   * - false : 事件触发后自动重置状态（默认）
   * @note 本构造函数不会抛出异常
   */
  explicit Event(bool manualReset = false) noexcept;

  /**
   * @brief 等待事件被触发
   * @param timeoutMs 等待超时时间（毫秒），允许特殊值：
   * - TIMEOUT_INFINITE : 无限等待直到事件触发
   * - TIMEOUT_IMMEDIATE: 立即返回当前状态
   * @return bool 事件是否在超时前被触发：
   * - true  : 事件已触发（或等待期间被触发）
   * - false : 等待超时或发生错误
   * @note 在自动重置模式下，成功返回后事件状态会被自动重置
   */
  bool wait(int64_t timeoutMs);

  /**
   * @brief 触发事件
   * @note 本操作会唤醒所有等待线程，事件状态将保持触发状态直到调用 reset()
   */
  void set();

  /**
   * @brief 重置事件状态为未触发
   * @note 无论当前事件是否被触发，调用后均会重置状态
   */
  void reset();

 private:
  bool manualReset_;  // 标识是否为手动重置模式

  std::mutex mutex_;
  std::condition_variable cv_;
  std::atomic<bool> triggered_{false};  // 事件触发状态标志
};

}  // namespace thread
}  // namespace pickup
