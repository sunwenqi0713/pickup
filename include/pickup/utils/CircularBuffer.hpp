#pragma once

#include <cassert>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace pickup {
namespace utils {

/**
 * @class CircularBuffer
 * @brief 高效环形缓冲区模板类，支持动态扩容、移动语义和覆盖统计
 * @tparam T 元素类型
 * @note 该类不支持多线程并发操作，需自行保证线程安全
 */
template <typename T>
class CircularBuffer {
 public:
  /**
   * @brief 构造指定容量的环形缓冲区
   * @param capacity 初始容量，必须大于0
   * @throws std::invalid_argument 如果capacity为0
   */
  explicit CircularBuffer(size_t capacity) {
    if (capacity == 0) {
      throw std::invalid_argument("Capacity must be greater than 0");
    }
    reset(capacity);
  }

  ~CircularBuffer() = default;

  // 拷贝构造/赋值
  CircularBuffer(const CircularBuffer&) = default;
  CircularBuffer& operator=(const CircularBuffer&) = default;

  // 移动构造/赋值
  CircularBuffer(CircularBuffer&& other) noexcept
      : data_(std::move(other.data_)),
        head_(other.head_),
        tail_(other.tail_),
        size_(other.size_),
        capacity_(other.capacity_),
        overrun_count_(other.overrun_count_) {
    other.reset(0);  // 置空原对象
  }

  CircularBuffer& operator=(CircularBuffer&& other) noexcept {
    if (this != &other) {
      data_ = std::move(other.data_);
      head_ = other.head_;
      tail_ = other.tail_;
      size_ = other.size_;
      capacity_ = other.capacity_;
      overrun_count_ = other.overrun_count_;
      other.reset(0);
    }
    return *this;
  }

  /// @name 容量相关操作
  /// @{
  size_t capacity() const noexcept { return capacity_; }
  size_t size() const noexcept { return size_; }
  bool empty() const noexcept { return size_ == 0; }
  bool full() const noexcept { return size_ == capacity_; }
  /// @}

  /// @name 元素访问
  /// @{
  T& front() {
    check_empty();
    return data_[head_];
  }

  const T& front() const {
    check_empty();
    return data_[head_];
  }

  T& back() {
    check_empty();
    return data_[(tail_ + capacity_ - 1) % capacity_];
  }

  const T& back() const {
    check_empty();
    return data_[(tail_ + capacity_ - 1) % capacity_];
  }

  T& operator[](size_t index) { return data_[physical_index(index)]; }

  const T& operator[](size_t index) const { return data_[physical_index(index)]; }
  /// @}

  /// @name 修改器操作
  /// @{
  void push_back(const T& value) { emplace_back_impl(value); }

  void push_back(T&& value) { emplace_back_impl(std::move(value)); }

  template <typename... Args>
  void emplace_back(Args&&... args) {
    emplace_back_impl(std::forward<Args>(args)...);
  }

  void pop_front() noexcept {
    if (!empty()) {
      head_ = next_pos(head_);
      --size_;
    }
  }

  void clear() noexcept {
    head_ = 0;
    tail_ = 0;
    size_ = 0;
    overrun_count_ = 0;
  }

  size_t overrun_count() const noexcept { return overrun_count_; }
  /// @}

  /// @name 容量调整
  /// @{
  void resize(size_t new_capacity) {
    if (new_capacity == capacity_) return;

    std::vector<T> new_data;
    new_data.reserve(new_capacity);

    // 迁移有效元素
    const size_t elements_to_keep = std::min(size_, new_capacity);
    for (size_t i = 0; i < elements_to_keep; ++i) {
      new_data.emplace_back(std::move(operator[](i)));
    }

    // 更新状态
    data_ = std::move(new_data);
    capacity_ = new_capacity;
    head_ = 0;
    tail_ = elements_to_keep % new_capacity;
    size_ = elements_to_keep;

    // 处理被截断的元素
    if (elements_to_keep < size_) {
      overrun_count_ += (size_ - elements_to_keep);
    }
  }
  /// @}

 private:
  // 物理索引计算
  size_t physical_index(size_t logical) const {
    if (logical >= size_) throw std::out_of_range("Logical index out of range");
    return (head_ + logical) % capacity_;
  }

  // 下一个物理位置
  size_t next_pos(size_t pos) const noexcept { return (pos + 1) % capacity_; }

  // 重置缓冲区状态
  void reset(size_t new_capacity) {
    data_.clear();
    data_.resize(new_capacity);
    capacity_ = new_capacity;
    clear();
  }

  // 非空检查
  void check_empty() const {
    if (empty()) {
      throw std::out_of_range("Accessing empty circular buffer");
    }
  }

  // 通用插入实现
  template <typename... Args>
  void emplace_back_impl(Args&&... args) {
    if (capacity_ == 0) return;

    if (full()) {
      // 覆盖最旧元素
      data_[tail_] = T(std::forward<Args>(args)...);
      head_ = next_pos(head_);
      ++overrun_count_;
    } else {
      // 构造新元素
      data_[tail_] = T(std::forward<Args>(args)...);
      ++size_;
    }
    tail_ = next_pos(tail_);
  }

 private:
  std::vector<T> data_;       ///< 数据存储
  size_t head_ = 0;           ///< 头部物理索引
  size_t tail_ = 0;           ///< 尾部物理索引
  size_t size_ = 0;           ///< 当前元素数量
  size_t capacity_ = 0;       ///< 缓冲区容量
  size_t overrun_count_ = 0;  ///< 元素覆盖计数器
};

}  // namespace utils
}  // namespace pickup