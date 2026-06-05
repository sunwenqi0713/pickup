#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <new>
#include <type_traits>
#include <utility>

namespace pickup {
namespace utils {

/**
 * @brief 无锁多生产者单消费者（MPSC）有界队列
 *
 * 适用于多线程生产、单一线程消费的场景。
 * 内部使用有界环形缓冲区，生产者通过 CAS 抢占写入槽位，
 * 消费者按提交顺序读取。
 *
 * 容量在构造时固定，满时 tryPush() 返回 false，空时 tryPop() 返回 false。
 *
 * @code
 * MPSCQueue<Message> queue(1024);
 *
 * // 生产者线程（任意数量）
 * if (queue.tryPush(msg)) { // 成功
 * }
 *
 * // 消费者线程（仅一个）
 * Message msg;
 * if (queue.tryPop(msg))    { // 取到消息
 * }
 * @endcode
 *
 * 线程安全：
 *   - 多个线程可并发调用 tryPush / emplace（生产者）
 *   - 仅有唯一线程可调用 tryPop（消费者）
 *   - size() / empty() / capacity() 可从任意线程调用
 *
 * @note 本队列不提供阻塞操作。如需阻塞语义，请配合信号量或条件变量使用。
 *
 * @tparam T 元素类型
 */
template <typename T>
class MPSCQueue {
 public:
  static constexpr std::size_t CACHE_LINE_SIZE = 64;

  /**
   * @brief 构造队列
   * @param capacity 容量（至少为 1，会向上对齐到 2 的幂以便高效取模）
   */
  explicit MPSCQueue(std::size_t capacity)
      : _capacity(nextPowerOfTwo(capacity)),
        _mask(_capacity - 1),
        _slots(static_cast<Slot*>(::operator new(_capacity * sizeof(Slot)))) {
    // 初始化每个槽位的序列号
    for (std::size_t i = 0; i < _capacity; ++i) {
      new (&_slots[i].sequence) std::atomic<std::size_t>(i);
    }
  }

  /** @brief 析构队列 */
  ~MPSCQueue() {
    // 析构残留的元素
    std::size_t tail = _tail.load(std::memory_order_relaxed);
    std::size_t head = _head.load(std::memory_order_acquire);
    while (tail != head) {
      std::size_t idx = index(tail);
      _slots[idx].ptr()->~T();
      ++tail;
    }
    // 析构槽位中的 atomic 并释放内存
    for (std::size_t i = 0; i < _capacity; ++i) {
      _slots[i].sequence.~atomic();
    }
    ::operator delete(_slots);
  }

  MPSCQueue(const MPSCQueue&) = delete;
  MPSCQueue& operator=(const MPSCQueue&) = delete;
  MPSCQueue(MPSCQueue&&) = delete;
  MPSCQueue& operator=(MPSCQueue&&) = delete;

  /**
   * @brief 尝试以拷贝方式入队
   * @param item 要入队的元素
   * @return 成功返回 true，队列已满返回 false
   * @note 多生产者并发安全
   */
  bool tryPush(const T& item) { return emplace(item); }

  /**
   * @brief 尝试以移动方式入队
   * @param item 要入队的元素（移动后源对象失效）
   * @return 成功返回 true，队列已满返回 false
   * @note 多生产者并发安全
   */
  bool tryPush(T&& item) { return emplace(std::move(item)); }

  /**
   * @brief 尝试在队尾原地构造一个元素
   * @tparam Args 构造参数类型
   * @param args  转发给元素构造函数的参数
   * @return 成功返回 true，队列已满返回 false
   * @note 多生产者并发安全
   */
  template <typename... Args>
  bool emplace(Args&&... args) {
    std::size_t head = _head.load(std::memory_order_relaxed);

    for (;;) {
      Slot& slot = _slots[index(head)];
      std::size_t seq = slot.sequence.load(std::memory_order_acquire);
      std::intptr_t diff = static_cast<std::intptr_t>(seq) - static_cast<std::intptr_t>(head);

      if (diff == 0) {
        // 槽位可写，尝试抢占
        if (_head.compare_exchange_weak(head, head + 1, std::memory_order_relaxed)) {
          // 抢占成功，原地构造元素
          new (slot.ptr()) T(std::forward<Args>(args)...);
          // 发布：通知消费者此槽位已就绪
          slot.sequence.store(head + 1, std::memory_order_release);
          return true;
        }
        // CAS 失败，其他生产者已抢占，使用更新后的 head 重试
      } else if (diff < 0) {
        // 队列已满（head 追上 tail）
        return false;
      } else {
        // 其他生产者正在写此槽位，重新加载 head 并重试
        head = _head.load(std::memory_order_relaxed);
      }
    }
  }

  /**
   * @brief 尝试出队
   * @param item 接收出队元素的引用
   * @return 成功返回 true，队列为空返回 false
   * @note 仅允许单一消费者线程调用
   */
  bool tryPop(T& item) {
    std::size_t tail = _tail.load(std::memory_order_relaxed);
    Slot& slot = _slots[index(tail)];
    std::size_t seq = slot.sequence.load(std::memory_order_acquire);
    std::intptr_t diff = static_cast<std::intptr_t>(seq) - static_cast<std::intptr_t>(tail + 1);

    if (diff == 0) {
      // 槽位已写入完成，可读取
      item = std::move(*slot.ptr());
      slot.ptr()->~T();
      // 将序列号重置为 tail + capacity，标记此槽位可供生产者再次使用
      slot.sequence.store(tail + _capacity, std::memory_order_release);
      _tail.store(tail + 1, std::memory_order_relaxed);
      return true;
    } else if (diff < 0) {
      // 队列为空，或生产者尚未完成写入
      return false;
    }

    // 单一消费者下不应到达此处
    return false;
  }

  /**
   * @brief 返回队列中元素的近似数量
   * @return 元素数量（近似值，生产者可能正在并发修改）
   */
  [[nodiscard]] std::size_t size() const {
    std::size_t head = _head.load(std::memory_order_acquire);
    std::size_t tail = _tail.load(std::memory_order_acquire);
    return head - tail;
  }

  /**
   * @brief 判断队列是否为空
   * @return 队列为空返回 true，否则返回 false
   * @note 与 size() 相同的原因，结果为近似值
   */
  [[nodiscard]] bool empty() const { return size() == 0; }

  /** @brief 返回队列容量 */
  [[nodiscard]] std::size_t capacity() const { return _capacity; }

 private:
  struct Slot {
    std::atomic<std::size_t> sequence;
    typename std::aligned_storage<sizeof(T), alignof(T)>::type storage;

    T* ptr() { return reinterpret_cast<T*>(&storage); }
    const T* ptr() const { return reinterpret_cast<const T*>(&storage); }
  };

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

  std::size_t index(std::size_t pos) const { return pos & _mask; }

  const std::size_t _capacity;
  const std::size_t _mask;
  Slot* const _slots;

  // head 与 tail 分离到不同缓存行，避免 false sharing
  alignas(CACHE_LINE_SIZE) std::atomic<std::size_t> _head{0};  ///< 由生产者写入（CAS）
  alignas(CACHE_LINE_SIZE) std::atomic<std::size_t> _tail{0};  ///< 仅由消费者写入
};

}  // namespace utils
}  // namespace pickup
