#pragma once

#include <atomic>
#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>

namespace pickup {
namespace thread {

/**
 * @brief 无锁单生产者单消费者（SPSC）有界队列
 *
 * 针对单线程生产、单一线程消费的场景高度优化。
 * 通过缓存已读位置减少原子操作，提供极低延迟和高吞吐。
 *
 * 容量在构造时固定，满时 tryPush() 返回 false，空时 tryPop() 返回 false。
 *
 * @code
 * SPSCQueue<Message> queue(1024);
 *
 * // 生产者线程（仅一个）
 * if (queue.tryPush(msg)) { // 成功
 * }
 *
 * // 消费者线程（仅一个）
 * Message msg;
 * if (queue.tryPop(msg)) { // 取到消息
 * }
 * @endcode
 *
 * 线程安全：
 *   - 仅有唯一线程可调用 tryPush / emplace（生产者）
 *   - 仅有唯一线程可调用 tryPop（消费者）
 *   - size() / empty() / capacity() 可从任意线程调用
 *
 * @note 本队列不提供阻塞操作。如需阻塞语义，请配合信号量或条件变量使用。
 *
 * @tparam T 元素类型
 */
template <typename T>
class SPSCQueue {
 public:
  static constexpr std::size_t CACHE_LINE_SIZE = 64;

  /**
   * @brief 构造队列
   * @param capacity 容量（至少为 1，会向上对齐到 2 的幂以便高效取模）
   */
  explicit SPSCQueue(std::size_t capacity)
      : capacity_(nextPowerOfTwo(capacity)),
        mask_(capacity_ - 1),
        buffer_(static_cast<T*>(::operator new(capacity_ * sizeof(T)))) {}

  /** @brief 析构队列 */
  ~SPSCQueue() {
    std::size_t head = head_.load(std::memory_order_relaxed);
    std::size_t tail = tail_.load(std::memory_order_relaxed);
    while (tail != head) {
      buffer_[index(tail)].~T();
      ++tail;
    }
    ::operator delete(buffer_);
  }

  SPSCQueue(const SPSCQueue&) = delete;
  SPSCQueue& operator=(const SPSCQueue&) = delete;
  SPSCQueue(SPSCQueue&&) = delete;
  SPSCQueue& operator=(SPSCQueue&&) = delete;

  /**
   * @brief 尝试以拷贝方式入队
   * @param item 要入队的元素
   * @return 成功返回 true，队列已满返回 false
   */
  bool tryPush(const T& item) { return emplace(item); }

  /**
   * @brief 尝试以移动方式入队
   * @param item 要入队的元素（移动后源对象失效）
   * @return 成功返回 true，队列已满返回 false
   */
  bool tryPush(T&& item) { return emplace(std::move(item)); }

  /**
   * @brief 尝试在队尾原地构造一个元素
   * @tparam Args 构造参数类型
   * @param args  转发给元素构造函数的参数
   * @return 成功返回 true，队列已满返回 false
   */
  template <typename... Args>
  bool emplace(Args&&... args) {
    const std::size_t head = head_.load(std::memory_order_relaxed);
    const std::size_t nextHead = head + 1;

    if (index(nextHead) == index(cachedTail_)) {
      cachedTail_ = tail_.load(std::memory_order_acquire);
      if (index(nextHead) == index(cachedTail_)) {
        return false;
      }
    }

    new (&buffer_[index(head)]) T(std::forward<Args>(args)...);
    head_.store(nextHead, std::memory_order_release);
    return true;
  }

  /**
   * @brief 尝试出队
   * @param item 接收出队元素的引用
   * @return 成功返回 true，队列为空返回 false
   * @note 仅允许单一消费者线程调用
   */
  bool tryPop(T& item) {
    const std::size_t tail = tail_.load(std::memory_order_relaxed);

    if (index(tail) == index(cachedHead_)) {
      cachedHead_ = head_.load(std::memory_order_acquire);
      if (index(tail) == index(cachedHead_)) {
        return false;
      }
    }

    item = std::move(buffer_[index(tail)]);
    buffer_[index(tail)].~T();
    tail_.store(tail + 1, std::memory_order_release);
    return true;
  }

  /**
   * @brief 返回队列中元素的近似数量
   * @return 元素数量（近似值，生产者和消费者可能正在并发修改）
   */
  [[nodiscard]] std::size_t size() const {
    const std::size_t head = head_.load(std::memory_order_acquire);
    const std::size_t tail = tail_.load(std::memory_order_acquire);
    return head - tail;
  }

  /**
   * @brief 判断队列是否为空
   * @return 队列为空返回 true，否则返回 false
   * @note 与 size() 相同的原因，结果为近似值
   */
  [[nodiscard]] bool empty() const { return size() == 0; }

  /** @brief 返回队列容量 */
  [[nodiscard]] std::size_t capacity() const { return capacity_; }

 private:
  static std::size_t nextPowerOfTwo(std::size_t n) {
    if (n == 0) return 1;
    --n;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    if constexpr (sizeof(std::size_t) > 4) {
      n |= n >> 32;
    }
    return n + 1;
  }

  std::size_t index(std::size_t pos) const { return pos & mask_; }

  const std::size_t capacity_;
  const std::size_t mask_;
  T* const buffer_;

  // head 与 tail 分离到不同缓存行，避免 false sharing
  alignas(CACHE_LINE_SIZE) std::atomic<std::size_t> head_{0};  ///< 由生产者写入
  alignas(CACHE_LINE_SIZE) std::atomic<std::size_t> tail_{0};  ///< 由消费者写入

  // 缓存的读写位置，减少原子加载
  alignas(CACHE_LINE_SIZE) std::size_t cachedTail_{0};  ///< 由生产者缓存
  alignas(CACHE_LINE_SIZE) std::size_t cachedHead_{0};  ///< 由消费者缓存
};

}  // namespace thread
}  // namespace pickup
