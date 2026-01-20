#pragma once

#include <limits>
#include <mutex>

namespace pickup {
namespace utils {
/**
 * 一个类似于 std::latch 的实用工具类，但允许增加计数。
 * 当需要进入某代码块的线程数可能动态变化时，这个类很有用。
 * 它可用于同步两个代码块：第一个块可以被 'n' 个线程同时执行，
 * 而第二个块需要等待所有线程退出第一个块。
 *
 * 示例代码：
 * function1()
 * {
 *    if(latch.countUp())
 *    {
 *       // 在此执行关键操作
 *       latch.countDown();
 *    }
 * }
 * function2()
 * {
 *    latch.wait(); // 此调用会阻塞线程，直到所有执行过 function1 的线程都已完成
 *    // 在此执行后续操作
 * }
 */
class CounterLatch final {
 public:
  CounterLatch() : count_(0) {}
  CounterLatch(CounterLatch const&) = delete;
  CounterLatch(CounterLatch&&) = delete;
  CounterLatch& operator=(CounterLatch const&) = delete;
  CounterLatch& operator=(CounterLatch&&) = delete;
  ~CounterLatch() = default;

  /**
   * 增加门闩的计数（如果当前计数不是负数）
   * return 如果计数成功增加，则返回 true；否则返回 false
   */
  bool countUp() {
    std::lock_guard<std::mutex> lock{mtx_};
    if (count_ >= 0) {
      ++count_;
      return true;
    }
    return false;
  }

  /**
   * 减少门闩的计数，如果计数达到零，则释放所有等待的线程。
   * 如果当前计数大于零，则将其递减。
   * 如果新计数为零，则通知所有等待的线程。
   * 如果当前计数等于零，则不执行任何操作。
   */
  void countDown() {
    std::lock_guard<std::mutex> lock{mtx_};
    if (count_ > 0) {
      if (--count_ == 0) {
        // 通知等待的线程
        cond_.notify_all();
      }
    }
  }

  /**
   * 等待计数器达到 0。此方法返回后，计数器的值将变为无效（负数）。
   * 此方法设计为仅供一次性使用。
   *
   * throws std::runtime_error 如果当前计数为负数。
   */
  void wait() {
    std::unique_lock<std::mutex> lock{mtx_};
    if (count_ < 0) {
      throw std::runtime_error("CounterLatch is in invalid state.");
    }
    cond_.wait(lock, [&]() { return count_ == 0; });
    count_ = (std::numeric_limits<long>::min)();  // 使门闩对其他线程不可用
  }

  /**
   * 返回门闩的当前计数
   */
  long getCount() {
    std::unique_lock<std::mutex> lock{mtx_};
    return count_;
  }

 private:
  std::mutex mtx_;                ///< 互斥锁，用于保护对计数器的访问
  std::condition_variable cond_;  ///< 用于通知等待的线程
  long count_;                    ///< 门闩计数器
};

}  // namespace utils
}  // namespace pickup