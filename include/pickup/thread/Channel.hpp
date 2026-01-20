#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

namespace pickup {
namespace thread {

/** @brief 线程安全通道类，用于线程间无需额外同步的数据传输
 *
 * 本通道类提供线程间安全高效的数据传输机制，无需使用共享内存、互斥锁等同步技术。
 * 内置信号通知系统，允许接收线程在数据到达或通道关闭前保持休眠状态。
 *
 * 设计特点：
 * - 单向通信模式，双向通信需要创建两个通道
 * - 严格遵循FIFO（先进先出）顺序
 * - 多线程发送时自动阻塞等待可用
 *
 * @tparam T 传输数据类型
 */
template <typename T>
class Channel {
 public:
  /** @brief 默认构造函数
   *
   * 创建一个通道对象，用于线程间数据传输
   */
  Channel() : closed_(false) {}

  /** @brief 阻塞接收数据
   * @param sentValue 接收数据的引用（将通过swap操作直接修改目标对象）
   * @return true 接收成功 | false 通道已关闭
   *
   * @code
   * Channel<Pixels> channel;
   * Pixels pixels;
   * if (channel.receive(pixels)) {
   *     // 成功接收数据
   * } else {
   *     // 通道已关闭
   * }
   * @endcode
   * @note 本函数将阻塞当前线程，直到有数据到达或通道关闭
   */
  bool receive(T& sentValue) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (closed_) return false;

    while (queue_.empty() && !closed_) {
      cv_.wait(lock);
    }

    if (!closed_) {
      std::swap(sentValue, queue_.front());
      queue_.pop();
      return true;
    }
    return false;
  }

  /** @brief 非阻塞接收尝试
   * @param sentValue 接收数据的引用
   * @return true 接收成功 | false 无数据/通道关闭
   * @note 本函数立即返回，无数据时不会阻塞
   */
  bool tryReceive(T& sentValue) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (closed_ || queue_.empty()) return false;

    std::swap(sentValue, queue_.front());
    queue_.pop();
    return true;
  }

  /** @brief 限时阻塞接收
   * @param sentValue 接收数据的引用
   * @param timeoutMs 最大等待时间（毫秒）
   * @return true 接收成功 | false 超时/通道关闭
   * @note 本函数将阻塞当前线程，直到有数据到达、超时或通道关闭
   */
  bool tryReceive(T& sentValue, int64_t timeoutMs) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (closed_) return false;

    if (queue_.empty()) {
      auto status = cv_.wait_for(lock, std::chrono::milliseconds(timeoutMs));
      if (status == std::cv_status::timeout || queue_.empty()) return false;
    }

    if (!closed_) {
      std::swap(sentValue, queue_.front());
      queue_.pop();
      return true;
    }
    return false;
  }

  /**
   * @brief 拷贝方式发送数据
   * @param value 要发送的数据
   * @return 发送成功返回 true，通道已关闭返回 false
   */
  bool send(const T& value) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (closed_) return false;

    queue_.push(value);
    cv_.notify_one();
    return true;
  }

  /** @brief 移动方式发送数据
   * @param value 要发送的数据（移动方式，原对象将失效）
   * @return true 发送成功 | false 通道已关闭
   * @note 本函数将数据移动到队列，原对象将失效
   */
  bool send(T&& value) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (closed_) return false;

    queue_.push(std::move(value));
    cv_.notify_one();
    return true;
  }

  /** @brief 关闭通道
   *
   * 关闭后：
   * - 禁止继续发送/接收数据
   * - 唤醒所有等待线程
   * - 所有接收操作将立即返回false
   */
  void close() {
    std::unique_lock<std::mutex> lock(mutex_);
    closed_ = true;
    cv_.notify_all();
  }

  /**
   * @brief 清空队列
   * @note 线程安全
   */
  void clear() {
    std::unique_lock<std::mutex> lock(mutex_);
    queue_ = {};
  }

  /**
   * @brief 检查队列是否为空
   * @return 队列为空返回 true，否则返回 false
   * @note 返回值可能瞬时失效（多线程环境）
   */
  bool empty() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return queue_.empty();
  }

  /**
   * @brief 获取队列大小
   * @return 队列中的元素数量
   * @note 返回值可能瞬时失效（多线程环境）
   */
  size_t size() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return queue_.size();
  }

 private:
  std::queue<T> queue_;         ///< 数据存储队列
  mutable std::mutex mutex_;    ///< 队列操作互斥锁
  std::condition_variable cv_;  ///< 条件变量用于线程通知
  bool closed_;                 ///< 通道关闭状态标志
};

}  // namespace thread
}  // namespace pickup