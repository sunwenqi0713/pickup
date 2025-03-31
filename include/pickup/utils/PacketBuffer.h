#pragma once

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

namespace pickup {
namespace utils {
/**
 * @class PacketBuffer
 * @brief 数据包缓冲区类
 * 本类提供了一个简单的数据包缓冲区，支持基础类型的读写操作，
 *
 */
class PacketBuffer {
 public:
  // 构造函数，初始容量可选
  explicit PacketBuffer(size_t initial_capacity = 1024) : buffer_(initial_capacity), read_pos_(0), write_pos_(0) {}

  // 获取可读数据大小
  size_t readableBytes() const noexcept { return write_pos_ - read_pos_; }

  // 获取可写空间大小
  size_t writableBytes() const noexcept { return buffer_.size() - write_pos_; }

  // 零拷贝方式获取可读数据范围
  std::pair<const uint8_t*, const uint8_t*> readableData() const noexcept {
    return {buffer_.data() + read_pos_, buffer_.data() + write_pos_};
  }

  // 确保有足够写入空间（自动扩容）
  void ensureWritable(size_t len) {
    if (writableBytes() < len) {
      expandBuffer(len);
    }
  }

  // 写入基础类型（支持大小端转换）
  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  void write(T value, bool network_order = true) {
    if (network_order && sizeof(T) > 1) {
      value = hton(value);
    }
    writeBytes(reinterpret_cast<const uint8_t*>(&value), sizeof(T));
  }

  // 写入原始字节
  void writeBytes(const uint8_t* data, size_t len) {
    ensureWritable(len);
    std::memcpy(buffer_.data() + write_pos_, data, len);
    write_pos_ += len;
  }

  // 读取基础类型
  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  T read(bool network_order = true) {
    checkReadable(sizeof(T));
    T value;
    std::memcpy(&value, buffer_.data() + read_pos_, sizeof(T));
    read_pos_ += sizeof(T);
    if (network_order && sizeof(T) > 1) {
      value = ntoh(value);
    }
    return value;
  }

  // 零拷贝方式读取数据（不移动读指针）
  void peekBytes(uint8_t* dest, size_t len) const {
    checkReadable(len);
    std::memcpy(dest, buffer_.data() + read_pos_, len);
  }

  // 丢弃已读数据
  void discardBytes(size_t len) {
    checkReadable(len);
    read_pos_ += len;
    compactBuffer();
  }

  // 交换缓冲区内容
  void swap(PacketBuffer& other) noexcept {
    buffer_.swap(other.buffer_);
    std::swap(read_pos_, other.read_pos_);
    std::swap(write_pos_, other.write_pos_);
  }

  // 清空缓冲区
  void clear() noexcept {
    read_pos_ = 0;
    write_pos_ = 0;
    buffer_.resize(buffer_.capacity());  // 保持容量
  }

 private:
  // 检查可读数据是否足够
  void checkReadable(size_t required) const {
    if (readableBytes() < required) {
      throw std::out_of_range("Not enough data to read");
    }
  }

  // 压缩缓冲区（回收已读空间）
  void compactBuffer() {
    if (read_pos_ == 0) return;

    size_t readable = readableBytes();
    std::memmove(buffer_.data(), buffer_.data() + read_pos_, readable);
    read_pos_ = 0;
    write_pos_ = readable;
  }

  // 扩展缓冲区
  void expandBuffer(size_t required) {
    // 先尝试压缩
    compactBuffer();

    // 仍需要扩容则调整大小
    if (writableBytes() < required) {
      size_t new_size = std::max(buffer_.size() * 2, buffer_.size() + required);
      buffer_.resize(new_size);
    }
  }

  // 字节序转换帮助函数
  template <typename T>
  T hton(T value) const {
    if constexpr (sizeof(T) == 2) {
      return htons(value);
    } else if constexpr (sizeof(T) == 4) {
      return htonl(value);
    } else if constexpr (sizeof(T) == 8) {
      uint64_t tmp = value;
      tmp = htonl(tmp >> 32) | (static_cast<uint64_t>(htonl(tmp & 0xFFFFFFFF)) << 32;
      return tmp;
    } else {
      return value;
    }
  }

  template <typename T>
  T ntoh(T value) const {
    return hton(value);  // 网络序转换对称
  }

 private:
  std::vector<uint8_t> buffer_;
  size_t read_pos_;
  size_t write_pos_;
};

}  // namespace utils
}  // namespace pickup