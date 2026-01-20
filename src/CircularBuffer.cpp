#include "pickup/utils/CircularBuffer.h"

#include <assert.h>
#include <string.h>
#include <algorithm>

namespace pickup {
namespace utils {

CircularBuffer::~CircularBuffer() { deallocate(); }

bool CircularBuffer::allocate(size_t buffer_size) {
  assert(buffer_ == nullptr);

  size_ = buffer_size;
  buffer_ = new uint8_t[size_];
  return buffer_ != nullptr;
}

void CircularBuffer::deallocate() {
  if (buffer_ != nullptr) {
    delete[] buffer_;
    buffer_ = nullptr;
  }
  size_ = 0;
}

size_t CircularBuffer::space_available() const {
  if (start_ > end_) {
    return start_ - end_ - 1;

  } else {
    return start_ - end_ - 1 + size_;
  }
}

size_t CircularBuffer::space_used() const {
  if (start_ <= end_) {
    return end_ - start_;

  } else {
    // 可能存在回绕情况
    return end_ - start_ + size_;
  }
}

bool CircularBuffer::push_back(const uint8_t* buf, size_t buf_len) {
  if (buf_len == 0 || buf == nullptr) {
    // 无数据可添加，最好不执行操作
    return false;
  }

  if (start_ > end_) {
    // 在end_后添加数据直到start_前，无回绕
    // 预留一个字节空间，避免start_与end_相等
    // 这表示缓冲区为空
    const size_t available = start_ - end_ - 1;

    if (available < buf_len) {
      return false;
    }

    memcpy(&buffer_[end_], buf, buf_len);
    end_ += buf_len;

  } else {
    // 在end_后添加数据，可能发生回绕
    const size_t available = start_ - end_ - 1 + size_;

    if (available < buf_len) {
      return false;
    }

    const size_t remaining_packet_len = size_ - end_;

    if (buf_len > remaining_packet_len) {
      memcpy(&buffer_[end_], buf, remaining_packet_len);
      end_ = 0;

      memcpy(&buffer_[end_], buf + remaining_packet_len, buf_len - remaining_packet_len);
      end_ += buf_len - remaining_packet_len;

    } else {
      memcpy(&buffer_[end_], buf, buf_len);
      end_ += buf_len;
    }
  }

  return true;
}

size_t CircularBuffer::pop_front(uint8_t* buf, size_t buf_max_len) {
  if (buf == nullptr) {
    // 用户必须提供有效的指针
    return 0;
  }

  if (start_ == end_) {
    // 缓冲区为空
    return 0;
  }

  if (start_ < end_) {
    // 无回绕情况
    size_t to_copy_len = std::min(end_ - start_, buf_max_len);

    memcpy(buf, &buffer_[start_], to_copy_len);
    start_ += to_copy_len;

    return to_copy_len;

  } else {
    // 可能存在回绕情况
    size_t to_copy_len = end_ - start_ + size_;

    if (to_copy_len > buf_max_len) {
      to_copy_len = buf_max_len;
    }

    const size_t remaining_buf_len = size_ - start_;

    if (to_copy_len > remaining_buf_len) {
      memcpy(buf, &buffer_[start_], remaining_buf_len);
      start_ = 0;
      memcpy(buf + remaining_buf_len, &buffer_[start_], to_copy_len - remaining_buf_len);
      start_ += to_copy_len - remaining_buf_len;

    } else {
      memcpy(buf, &buffer_[start_], to_copy_len);
      start_ += to_copy_len;
    }

    return to_copy_len;
  }
}

}  // namespace utils
}  // namespace pickup