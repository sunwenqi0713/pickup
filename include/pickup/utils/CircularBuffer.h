#pragma once

#include <cstddef> 
#include <cstdint>

// FIFO 环形缓冲区实现。
//
// 该环形缓冲区最多可存储比分配空间少 1 字节的数据，
// 因为在缓冲区已满时，起始和结束标记必须相隔 1 字节，
// 否则缓冲区会突然变为空。
//
// 此缓冲区不是线程安全的。

namespace pickup {
namespace utils {

class CircularBuffer {
 public:
  /* @brief 构造函数
   *
   * @note 不自动分配内存。
   */
  CircularBuffer() = default;

  /*
   * @brief 析构函数
   *
   * 自动调用 deallocate。
   */
  ~CircularBuffer();

  /* @brief 分配环形缓冲区
   *
   * @param buffer_size 要在堆上分配的字节数。
   *
   * @returns 如果分配失败则返回 false。
   */
  bool allocate(size_t buffer_size);

  /*
   * @brief 释放环形缓冲区
   *
   * @note 仅在需要释放并重新分配时才需要调用。
   */
  void deallocate();

  /*
   * @brief 可复制数据的可用空间
   *
   * @returns 可用字节数。
   */
  size_t space_available() const;

  /*
   * @brief 可用于复制的已用空间
   *
   * @returns 已使用字节数。
   */
  size_t space_used() const;

  /*
   * @brief 将数据复制到环形缓冲区
   *
   * @param buf 指向要复制的源缓冲区的指针。
   * @param buf_len 要复制的字节数。
   *
   * @returns 如果数据包可以复制到缓冲区则返回 true。
   */
  bool push_back(const uint8_t* buf, size_t buf_len);

  /*
   * @brief 从环形缓冲区获取数据
   *
   * @param buf 指向要复制到的目标缓冲区的指针。
   * @param max_buf_len 要复制的最大字节数。
   *
   * @returns 如果缓冲区为空则返回 0。
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