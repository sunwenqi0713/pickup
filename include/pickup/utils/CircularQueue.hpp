#pragma once

#include <condition_variable>
#include <mutex>
#include <vector>

namespace pickup {
namespace utils {

/**
 * @brief 线程安全的固定容量循环队列
 * @tparam T 元素类型
 *
 * 支持阻塞和非阻塞的入队/出队操作，适用于生产者-消费者模式。
 */
template <typename T>
class CircularQueue {
 public:
  /**
   * @brief 构造函数
   * @param capacity 队列容量
   */
  explicit CircularQueue(size_t capacity)
      : buffer_(capacity), capacity_(capacity), head_(0), tail_(0), count_(0), closed_(false) {}

  CircularQueue(const CircularQueue&) = delete;
  CircularQueue& operator=(const CircularQueue&) = delete;

  /**
   * @brief 阻塞入队
   * @param item 要入队的元素
   * @return 成功返回 true，队列关闭返回 false
   */
  bool enqueue(const T& item) {
    std::unique_lock<std::mutex> lock(mutex_);
    notFull_.wait(lock, [this]() { return count_ < capacity_ || closed_; });

    if (closed_) {
      return false;
    }

    buffer_[head_] = item;
    head_ = (head_ + 1) % capacity_;
    ++count_;

    notEmpty_.notify_one();
    return true;
  }

  /**
   * @brief 阻塞出队
   * @param item 接收出队元素的引用
   * @return 成功返回 true，队列关闭且为空返回 false
   */
  bool dequeue(T& item) {
    std::unique_lock<std::mutex> lock(mutex_);
    notEmpty_.wait(lock, [this]() { return count_ > 0 || closed_; });

    if (closed_ && count_ == 0) {
      return false;
    }

    item = buffer_[tail_];
    tail_ = (tail_ + 1) % capacity_;
    --count_;

    notFull_.notify_one();
    return true;
  }

  /**
   * @brief 非阻塞入队
   * @param item 要入队的元素
   * @return 成功返回 true，队列满或已关闭返回 false
   */
  bool tryEnqueue(const T& item) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (count_ == capacity_ || closed_) {
      return false;
    }

    buffer_[head_] = item;
    head_ = (head_ + 1) % capacity_;
    ++count_;

    notEmpty_.notify_one();
    return true;
  }

  /**
   * @brief 非阻塞出队
   * @param item 接收出队元素的引用
   * @return 成功返回 true，队列空返回 false
   */
  bool tryDequeue(T& item) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (count_ == 0) {
      return false;
    }

    item = buffer_[tail_];
    tail_ = (tail_ + 1) % capacity_;
    --count_;

    notFull_.notify_one();
    return true;
  }

  /**
   * @brief 获取队列中元素数量
   * @return 元素数量
   */
  size_t size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return count_;
  }

  /**
   * @brief 检查队列是否为空
   * @return 为空返回 true，否则返回 false
   */
  bool empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return count_ == 0;
  }

  /**
   * @brief 检查队列是否已满
   * @return 已满返回 true，否则返回 false
   */
  bool full() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return count_ == capacity_;
  }

  /**
   * @brief 获取队列容量
   * @return 队列容量
   */
  size_t capacity() const { return capacity_; }

  /**
   * @brief 关闭队列，唤醒所有等待线程
   */
  void close() {
    std::lock_guard<std::mutex> lock(mutex_);
    closed_ = true;
    notEmpty_.notify_all();
    notFull_.notify_all();
  }

  /**
   * @brief 检查队列是否已关闭
   * @return 已关闭返回 true，否则返回 false
   */
  bool isClosed() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return closed_;
  }

 private:
  std::vector<T> buffer_;
  size_t capacity_;
  size_t head_ = 0;
  size_t tail_ = 0;
  size_t count_ = 0;

  mutable std::mutex mutex_;
  std::condition_variable notEmpty_;
  std::condition_variable notFull_;

  bool closed_;
};

}  // namespace utils
}  // namespace pickup