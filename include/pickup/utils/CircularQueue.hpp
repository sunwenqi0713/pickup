#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <stdexcept>
#include <vector>

namespace pickup {
namespace utils {

template <typename T>
class CircularQueue {
 public:
  // 构造函数，指定队列大小
  explicit CircularQueue(size_t capacity)
      : buffer_(capacity), capacity_(capacity), head_(0), tail_(0), count_(0), is_closed_(false) {}

  // 禁止拷贝和移动
  CircularQueue(const CircularQueue&) = delete;
  CircularQueue& operator=(const CircularQueue&) = delete;

  // 入队
  bool enqueue(const T& item) {
    std::unique_lock<std::mutex> lock(mutex_);

    not_full_.wait(lock, [this]() { return count_ < capacity_ || is_closed_; });

    if (is_closed_) {
      return false;
    }

    // 写入数据
    buffer_[head_] = item;
    head_ = (head_ + 1) % capacity_;
    ++count_;

    // 通知消费者有数据可用
    not_empty_.notify_one();
    return true;
  }

  // 出队
  bool dequeue(T& item) {
    std::unique_lock<std::mutex> lock(mutex_);

    not_empty_.wait(lock, [this]() { return count_ > 0 || is_closed_; });

    if (is_closed_ && count_ == 0) {
      return false;
    }

    // 读取数据
    item = buffer_[tail_];
    tail_ = (tail_ + 1) % capacity_;
    --count_;

    // 通知生产者有空间可用
    not_full_.notify_one();
    return true;
  }

  // 尝试入队（非阻塞）
  bool try_enqueue(const T& item) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (count_ == capacity_ || is_closed_) {
      return false;
    }

    buffer_[head_] = item;
    head_ = (head_ + 1) % capacity_;
    ++count_;

    not_empty_.notify_one();
    return true;
  }

  // 尝试出队（非阻塞）
  bool try_dequeue(T& item) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (count_ == 0) {
      return false;
    }

    item = buffer_[tail_];
    tail_ = (tail_ + 1) % capacity_;
    --count_;

    not_full_.notify_one();
    return true;
  }

  // 获取队列中元素数量
  size_t size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return count_;
  }

  // 检查队列是否为空
  bool empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return count_ == 0;
  }

  // 检查队列是否已满
  bool full() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return count_ == capacity_;
  }

  // 获取队列容量
  size_t capacity() const { return capacity_; }

  // 关闭队列，唤醒所有等待的线程
  void close() {
    std::lock_guard<std::mutex> lock(mutex_);
    is_closed_ = true;
    not_empty_.notify_all();
    not_full_.notify_all();
  }

  // 检查队列是否已关闭
  bool is_closed() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return is_closed_;
  }

 private:
  std::vector<T> buffer_;  // 数据存储
  size_t capacity_;        // 容量
  size_t head_ = 0;        // 写入位置
  size_t tail_ = 0;        // 读取位置
  size_t count_ = 0;       // 当前元素数量

  mutable std::mutex mutex_;           // 互斥锁
  std::condition_variable not_empty_;  // 非空条件变量
  std::condition_variable not_full_;   // 非满条件变量

  bool is_closed_;  // 是否已关闭
};

}  // namespace utils
}  // namespace pickup