#pragma once

#include <cstddef>
#include <cstdint>

namespace pickup {
namespace utils {

/**
 * @brief FIFO 环形缓冲区
 *
 * 最多可存储比分配空间少 1 字节的数据，因为缓冲区满时
 * 起始和结束标记必须相隔 1 字节，否则无法区分满/空状态。
 * 本类非线程安全。
 */
class CircularBuffer {
 public:
  /**
   * @brief 构造函数（不自动分配内存，需显式调用 allocate）
   */
  CircularBuffer() = default;

  /**
   * @brief 析构函数（自动调用 deallocate）
   */
  ~CircularBuffer();

  /**
   * @brief 在堆上分配环形缓冲区
   * @param buffer_size 要分配的字节数
   * @return 分配失败返回 false
   */
  bool allocate(size_t buffer_size);

  /**
   * @brief 释放环形缓冲区（仅在需要重新分配时才须显式调用）
   */
  void deallocate();

  /**
   * @brief 返回可写入的剩余空间
   * @return 可用字节数
   */
  [[nodiscard]] size_t space_available() const;

  /**
   * @brief 返回已写入的数据量
   * @return 已使用字节数
   */
  [[nodiscard]] size_t space_used() const;

  /**
   * @brief 将数据写入环形缓冲区
   * @param buf     源数据指针
   * @param buf_len 要写入的字节数
   * @return 空间足够并成功写入返回 true，否则返回 false
   */
  bool push_back(const uint8_t* buf, size_t buf_len);

  /**
   * @brief 从环形缓冲区读取数据
   * @param buf         目标缓冲区指针
   * @param max_buf_len 最多读取的字节数
   * @return 实际读取的字节数，缓冲区为空时返回 0
   */
  size_t pop_front(uint8_t* buf, size_t max_buf_len);

 private:
  size_t size_{0};
  uint8_t* buffer_{nullptr};
  size_t start_{0};
  size_t end_{0};
};

}  // namespace utils
}  // namespace pickup
