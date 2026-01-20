#pragma once

#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace pickup {
namespace utils {

/**
 * 简单的字节缓冲区，设计为 std::vector<Byte> 的轻量级替代品。
 */
class ByteBuffer {
 public:
  using Byte = uint8_t;  // 字节类型
  // 构造函数
  ByteBuffer();                                                // 默认构造函数
  ByteBuffer(ByteBuffer&& other) noexcept;                     // 移动构造函数
  ByteBuffer(const ByteBuffer& other);                         // 复制构造函数
  explicit ByteBuffer(const std::string& stringBufferToCopy);  // 从字符串构造
  explicit ByteBuffer(std::string_view stringBufferToCopy);    // 从字符串视图构造
  explicit ByteBuffer(const char* nullTerminatedString);       // 从C风格字符串构造
  ByteBuffer(const Byte* begin, const Byte* end);              // 从字节范围构造
  ~ByteBuffer();                                               // 析构函数

  // 运算符重载
  ByteBuffer& operator=(const ByteBuffer& other);      // 复制赋值运算符
  ByteBuffer& operator=(ByteBuffer&& other) noexcept;  // 移动赋值运算符
  bool operator==(const ByteBuffer& other) const;      // 相等比较运算符
  bool operator!=(const ByteBuffer& other) const;      // 不等比较运算符

  void clear();  // 清空缓冲区

  // 数据访问
  Byte* data() const {
    return data_;  // 返回指向底层数据的指针
  }

  size_t size() const {
    return size_;  // 返回当前存储的字节数
  }

  bool empty() const {
    return size_ == 0;  // 检查是否为空
  }

  size_t capacity() const {
    return capacity_;  // 返回当前分配的容量
  }

  Byte* begin() const {
    return data();  // 返回指向第一个字节的指针
  }

  Byte* end() const {
    return begin() + size();  // 返回指向最后一个字节之后的指针
  }

  void reserve(size_t capacity);  // 预分配容量

  Byte& operator[](size_t i) const {
    return data_[i];  // 返回第i个字节的引用
  }

  /**
   * 向缓冲区追加N个字节，并返回指向新增区域起始位置的可写指针
   */
  Byte* appendWritable(size_t size);

  /**
   * 追加并写入给定的字节区域到缓冲区
   */
  void append(const Byte* begin, const Byte* end);

  /**
   * 追加并写入给定的字符区域到缓冲区
   */
  void append(const char* begin, const char* end);

  /**
   * 追加并写入给定的字符串视图到缓冲区
   */
  void append(std::string_view stringRegion);

  /**
   * 追加单个字符到缓冲区
   */
  void append(char c);

  /**
   * 追加单个字节到缓冲区
   */
  void append(Byte c);

  /**
   * 将整个ByteBuffer设置为给定的字节区域
   */
  void set(const Byte* begin, const Byte* end);

  /**
   * 将整个ByteBuffer设置为给定的初始化列表
   */
  void set(std::initializer_list<Byte> bytes) { set(bytes.begin(), bytes.end()); }

  /**
   * 从缓冲区开头移除N个字节
   */
  void shift(size_t size);

  /**
   * 将缓冲区大小调整为指定尺寸，必要时会减少容量
   */
  void resize(size_t size);

  /**
   * 收缩缓冲区以适应当前大小
   */
  void shrinkToFit();

  /**
   * 返回此ByteBuffer的字节向量副本
   */
  std::vector<uint8_t> toBytesVec() const;

  /**
   * 返回字符串视图
   */
  std::string_view toStringView() const;

 private:
  /**
   * 重新分配内存到指定容量
   */
  void reallocate(size_t capacity);

 private:
  size_t size_ = 0;       // 当前存储的字节数
  size_t capacity_ = 0;   // 分配的容量
  Byte* data_ = nullptr;  // 指向数据的指针
};

}  // namespace utils
}  // namespace pickup