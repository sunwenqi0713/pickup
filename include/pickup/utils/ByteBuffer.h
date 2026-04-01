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
 * @brief 简单的字节缓冲区，std::vector<uint8_t> 的轻量级替代品
 */
class ByteBuffer {
 public:
  using Byte = uint8_t;  ///<  字节类型

  ByteBuffer();
  ByteBuffer(ByteBuffer&& other) noexcept;
  ByteBuffer(const ByteBuffer& other);
  explicit ByteBuffer(const std::string& stringBufferToCopy);
  explicit ByteBuffer(std::string_view stringBufferToCopy);
  explicit ByteBuffer(const char* nullTerminatedString);
  ByteBuffer(const Byte* begin, const Byte* end);
  ~ByteBuffer();

  ByteBuffer& operator=(const ByteBuffer& other);
  ByteBuffer& operator=(ByteBuffer&& other) noexcept;
  bool operator==(const ByteBuffer& other) const;
  bool operator!=(const ByteBuffer& other) const;

  /** @brief 清空缓冲区 */
  void clear();

  /** @brief 返回指向底层数据的指针 */
  [[nodiscard]] Byte* data() const { return data_; }

  /** @brief 返回当前存储的字节数 */
  [[nodiscard]] size_t size() const { return size_; }

  /** @brief 检查缓冲区是否为空 */
  [[nodiscard]] bool empty() const { return size_ == 0; }

  /** @brief 返回当前分配的容量 */
  [[nodiscard]] size_t capacity() const { return capacity_; }

  /** @brief 返回指向第一个字节的指针 */
  [[nodiscard]] Byte* begin() const { return data(); }

  /** @brief 返回指向最后一个字节之后的指针 */
  [[nodiscard]] Byte* end() const { return begin() + size(); }

  /** @brief 预分配至少 capacity 字节的空间 */
  void reserve(size_t capacity);

  /** @brief 返回第 i 个字节的引用 */
  Byte& operator[](size_t i) const { return data_[i]; }

  /**
   * @brief 追加 N 个字节并返回指向新增区域起始位置的可写指针
   * @param size 要追加的字节数
   * @return 指向新增区域起始位置的指针（调用方必须使用该指针写入数据）
   */
  [[nodiscard]] Byte* appendWritable(size_t size);

  /**
   * @brief 将字节范围 [begin, end) 追加到缓冲区
   * @param begin 源数据起始指针
   * @param end   源数据结束指针（不含）
   */
  void append(const Byte* begin, const Byte* end);

  /**
   * @brief 将字符范围 [begin, end) 追加到缓冲区
   * @param begin 源数据起始指针
   * @param end   源数据结束指针（不含）
   */
  void append(const char* begin, const char* end);

  /**
   * @brief 将字符串视图追加到缓冲区
   * @param stringRegion 要追加的字符串视图
   */
  void append(std::string_view stringRegion);

  /**
   * @brief 追加单个字符
   * @param c 要追加的字符
   */
  void append(char c);

  /**
   * @brief 追加单个字节
   * @param c 要追加的字节
   */
  void append(Byte c);

  /**
   * @brief 将缓冲区内容替换为字节范围 [begin, end)
   * @param begin 源数据起始指针
   * @param end   源数据结束指针（不含）
   */
  void set(const Byte* begin, const Byte* end);

  /**
   * @brief 将缓冲区内容替换为初始化列表
   * @param bytes 字节列表
   */
  void set(std::initializer_list<Byte> bytes) { set(bytes.begin(), bytes.end()); }

  /**
   * @brief 从缓冲区开头移除 N 个字节
   * @param size 要移除的字节数
   */
  void shift(size_t size);

  /**
   * @brief 将缓冲区大小调整为指定尺寸
   * @param size 新的大小，必要时会缩减容量
   */
  void resize(size_t size);

  /** @brief 收缩分配容量以适应当前大小 */
  void shrinkToFit();

  /**
   * @brief 返回缓冲区内容的字节向量副本
   * @return std::vector<uint8_t>
   */
  [[nodiscard]] std::vector<uint8_t> toBytesVec() const;

  /**
   * @brief 返回缓冲区内容的字符串视图（零拷贝）
   * @return std::string_view
   */
  [[nodiscard]] std::string_view toStringView() const;

 private:
  /** @brief 重新分配内存到指定容量 */
  void reallocate(size_t capacity);

  size_t size_{0};       ///<  当前存储的字节数
  size_t capacity_{0};   ///<  分配的容量
  Byte* data_{nullptr};  ///<  指向数据的指针
};

}  // namespace utils
}  // namespace pickup
