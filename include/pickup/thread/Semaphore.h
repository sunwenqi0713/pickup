#pragma once

#include <condition_variable>
#include <mutex>

namespace pickup {
namespace thread {

/**
 * @class Semaphore
 * @brief 信号量类，用于线程间同步控制
 *
 * 通过计数器机制控制资源访问，提供PV原子操作（wait/signal）。
 * 可用于实现互斥锁、资源池限制等同步场景。
 */
class Semaphore {
 public:
  /**
   * @brief 构造函数，初始化信号量计数器
   * @param count 初始资源数量，默认值为0
   * @note 初始化值为1时可作为二元信号量（类似互斥锁）
   */
  Semaphore(int count = 0) : count_(count) {}

  /**
   * @brief P操作（wait），请求获取资源
   *
   * - 若计数器值 > 0，立即减少计数器并继续执行
   * - 若计数器值 = 0，阻塞线程直至其他线程调用signal()
   *
   * @warning 可能引发死锁（如多个线程循环等待时）
   */
  void release() {
    std::unique_lock<std::mutex> lock(mutex_);
    ++count_;
    cv_.notify_one();
  }

  /**
   * @brief V操作（signal），释放资源
   *
   * - 增加计数器值
   * - 唤醒一个正在等待的线程（如果有）
   *
   * @note 总是立即返回，不会阻塞调用线程
   */
  void acquire() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this]() { return count_ > 0; });
    --count_;
  }

 private:
  std::mutex mutex_;            ///< 互斥锁，保护计数器的原子访问
  std::condition_variable cv_;  ///< 条件变量，实现阻塞/唤醒机制
  int count_;                   ///< 资源计数器，表示可用资源数量
};

}  // namespace thread
}  // namespace pickup