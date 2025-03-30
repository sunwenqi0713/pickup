#pragma once

#include <cassert>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <utility>  // for std::swap

namespace pickup {
namespace utils {
// 这里是一个简单的Buffer类实现，支持动态内存管理和基本的操作
// 该类可以用于存储和操作字节数据，类似于std::vector<uint8_t>，但提供了一些额外的功能
// 例如，支持从原始内存指针创建Buffer对象，以及在Buffer对象之间移动数据等操作
class Buffer {
 public:
  explicit Buffer(std::size_t length) : capacity_(length), used_(length), ptr_(nullptr), ownMem_(true) {
    if (length > 0) {
      ptr_ = new uint8_t[length];
    }
  }

  Buffer(uint8_t* pMem, std::size_t length) : capacity_(length), used_(length), ptr_(pMem), ownMem_(false) {}

  Buffer(const uint8_t* pMem, std::size_t length) : capacity_(length), used_(length), ptr_(nullptr), ownMem_(true) {
    if (capacity_ > 0) {
      ptr_ = new uint8_t[capacity_];
      std::memcpy(ptr_, pMem, used_);
    }
  }

  Buffer(const Buffer& other) : capacity_(other.used_), used_(other.used_), ptr_(nullptr), ownMem_(true) {
    if (used_ > 0) {
      ptr_ = new uint8_t[used_];
      std::memcpy(ptr_, other.ptr_, used_);
    }
  }

  Buffer(Buffer&& other) noexcept
      : capacity_(other.capacity_), used_(other.used_), ptr_(other.ptr_), ownMem_(other.ownMem_) {
    other.capacity_ = 0;
    other.used_ = 0;
    other.ptr_ = nullptr;
    other.ownMem_ = false;
  }

  Buffer& operator=(const Buffer& other) {
    if (this != &other) {
      Buffer tmp(other);
      swap(tmp);
    }
    return *this;
  }

  Buffer& operator=(Buffer&& other) noexcept {
    if (ownMem_) delete[] ptr_;

    capacity_ = other.capacity_;
    used_ = other.used_;
    ptr_ = other.ptr_;
    ownMem_ = other.ownMem_;

    other.capacity_ = 0;
    other.used_ = 0;
    other.ptr_ = nullptr;
    other.ownMem_ = false;

    return *this;
  }

  ~Buffer() {
    if (ownMem_) delete[] ptr_;
  }

  void resize(std::size_t newCapacity, bool preserveContent = true) {
    if (!ownMem_) throw std::runtime_error("Cannot resize buffer which does not own its storage.");

    if (newCapacity > capacity_) {
      uint8_t* ptr = new uint8_t[newCapacity];
      if (preserveContent && ptr_) {
        std::memcpy(ptr, ptr_, used_);
      }
      delete[] ptr_;
      ptr_ = ptr;
      capacity_ = newCapacity;
    }
    used_ = newCapacity;
  }

  void setCapacity(std::size_t newCapacity, bool preserveContent = true) {
    if (!ownMem_) throw std::runtime_error("Cannot resize buffer which does not own its storage.");

    if (newCapacity != capacity_) {
      uint8_t* ptr = nullptr;
      if (newCapacity > 0) {
        ptr = new uint8_t[newCapacity];
        if (preserveContent && ptr_) {
          const std::size_t copySize = (used_ < newCapacity) ? used_ : newCapacity;
          std::memcpy(ptr, ptr_, copySize);
        }
      }
      delete[] ptr_;
      ptr_ = ptr;
      capacity_ = newCapacity;

      if (newCapacity < used_) used_ = newCapacity;
    }
  }

  void assign(const uint8_t* buf, std::size_t sz) {
    if (sz == 0) return;
    if (sz > capacity_) resize(sz, false);
    std::memcpy(ptr_, buf, sz);
    used_ = sz;
  }

  void append(const uint8_t* buf, std::size_t sz) {
    if (sz == 0) return;
    const std::size_t oldSize = used_;
    resize(used_ + sz, true);
    std::memcpy(ptr_ + oldSize, buf, sz);
  }

  void append(uint8_t val) {
    const std::size_t oldSize = used_;
    resize(used_ + 1, true);
    ptr_[oldSize] = val;
  }

  void append(const Buffer& buf) { append(buf.begin(), buf.size()); }

  std::size_t capacity() const { return capacity_; }
  std::size_t capacityBytes() const { return capacity_; }

  void swap(Buffer& other) noexcept {
    std::swap(ptr_, other.ptr_);
    std::swap(capacity_, other.capacity_);
    std::swap(used_, other.used_);
    std::swap(ownMem_, other.ownMem_);
  }

  bool operator==(const Buffer& other) const {
    if (this == &other) return true;
    if (used_ != other.used_) return false;
    return std::memcmp(ptr_, other.ptr_, used_) == 0;
  }

  bool operator!=(const Buffer& other) const { return !(*this == other); }

  void clear() {
    if (ptr_) std::memset(ptr_, 0, used_);
  }

  std::size_t size() const { return used_; }
  std::size_t sizeBytes() const { return used_; }

  uint8_t* begin() { return ptr_; }
  const uint8_t* begin() const { return ptr_; }
  uint8_t* end() { return ptr_ + used_; }
  const uint8_t* end() const { return ptr_ + used_; }

  bool empty() const { return used_ == 0; }

  uint8_t& operator[](std::size_t index) {
    assert(index < used_);
    if (index >= used_) throw std::out_of_range("Buffer index out of range.");
    return ptr_[index];
  }

  const uint8_t& operator[](std::size_t index) const {
    assert(index < used_);
    if (index >= used_) throw std::out_of_range("Buffer index out of range.");
    return ptr_[index];
  }

 private:
  std::size_t capacity_;
  std::size_t used_;
  uint8_t* ptr_;
  bool ownMem_;
};

}  // namespace utils
}  // namespace pickup