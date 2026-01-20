#include "pickup/utils/ByteBuffer.h"

namespace pickup {
namespace utils {

constexpr size_t kMinAllocationSize = sizeof(size_t);

static void deallocateBuffer(ByteBuffer::Byte* buffer) {
  free(buffer);  // NOLINT(cppcoreguidelines-no-malloc)
}

static ByteBuffer::Byte* allocateBuffer(size_t size) {
  if (size == 0) {
    return nullptr;
  }

  return reinterpret_cast<ByteBuffer::Byte*>(
      malloc(sizeof(ByteBuffer::Byte) * size));  // NOLINT(cppcoreguidelines-no-malloc)
}

// 计算下一个2的幂
static unsigned long nextPowerOfTwo(unsigned long v) {
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;
  return v;
}

ByteBuffer::ByteBuffer() = default;

ByteBuffer::ByteBuffer(ByteBuffer&& other) noexcept
    : size_(other.size_), capacity_(other.capacity_), data_(other.data_) {
  other.size_ = 0;
  other.capacity_ = 0;
  other.data_ = nullptr;
}

ByteBuffer::ByteBuffer(const ByteBuffer& other) {
  if (other.size() != 0) {
    reallocate(other.size());
    std::memcpy(data_, other.data(), other.size());
    size_ = other.size();
  }
}

ByteBuffer::ByteBuffer(const std::string& stringBufferToCopy) : ByteBuffer(std::string_view(stringBufferToCopy)) {}

ByteBuffer::ByteBuffer(std::string_view stringBufferToCopy)
    : ByteBuffer(reinterpret_cast<const Byte*>(stringBufferToCopy.data()),
                 reinterpret_cast<const Byte*>(stringBufferToCopy.data() + stringBufferToCopy.length())) {}

ByteBuffer::ByteBuffer(const char* nullTerminatedString) : ByteBuffer(std::string_view(nullTerminatedString)) {}

ByteBuffer::ByteBuffer(const Byte* begin, const Byte* end) { set(begin, end); }

ByteBuffer::~ByteBuffer() { deallocateBuffer(data_); }

ByteBuffer& ByteBuffer::operator=(ByteBuffer&& other) noexcept {
  if (&other != this) {
    deallocateBuffer(data_);

    size_ = other.size_;
    capacity_ = other.capacity_;
    data_ = other.data_;

    other.size_ = 0;
    other.capacity_ = 0;
    other.data_ = nullptr;
  }
  return *this;
}

ByteBuffer& ByteBuffer::operator=(const ByteBuffer& other) {
  if (&other != this) {
    deallocateBuffer(data_);

    size_ = 0;
    capacity_ = 0;
    data_ = nullptr;

    if (other.size() != 0) {
      reallocate(other.size());
      std::memcpy(data_, other.data(), other.size());
      size_ = other.size();
    }
  }

  return *this;
}

bool ByteBuffer::operator==(const ByteBuffer& other) const {
  if (size_ != other.size_) {
    return false;
  }
  if (size_ == 0) {
    return true;
  }
  return std::memcmp(data_, other.data_, size_) == 0;
}

bool ByteBuffer::operator!=(const ByteBuffer& other) const { return !(*this == other); }

void ByteBuffer::clear() { size_ = 0; }

void ByteBuffer::reserve(size_t capacity) {
  if (capacity > capacity_) {
    reallocate(capacity);
  }
}

ByteBuffer::Byte* ByteBuffer::appendWritable(size_t size) {
  if (size == 0) {
    return data_ + size_;
  }

  auto nextSize = size_ + size;
  if (nextSize > capacity_) {
    reallocate(std::max(static_cast<size_t>(nextPowerOfTwo(static_cast<unsigned long>(nextSize))), kMinAllocationSize));
  }

  auto* ptr = data_ + size_;
  size_ = nextSize;
  return ptr;
}

void ByteBuffer::append(const Byte* begin, const Byte* end) {
  auto size = static_cast<size_t>(end - begin);
  if (size > 0) {
    auto* bytes = appendWritable(size);
    std::memcpy(bytes, begin, size);
  }
}

void ByteBuffer::append(const char* begin, const char* end) {
  append(reinterpret_cast<const Byte*>(begin), reinterpret_cast<const Byte*>(end));
}

void ByteBuffer::append(std::string_view stringRegion) {
  append(reinterpret_cast<const Byte*>(stringRegion.data()),
         reinterpret_cast<const Byte*>(stringRegion.data() + stringRegion.length()));
}

void ByteBuffer::append(char c) { append(static_cast<Byte>(c)); }

void ByteBuffer::append(Byte c) { *appendWritable(1) = c; }

void ByteBuffer::set(const Byte* begin, const Byte* end) {
  clear();

  auto size = static_cast<size_t>(end - begin);

  if (size > 0) {
    if (size > capacity_) {
      reallocate(size);
    }
    std::memcpy(data_, begin, size);
    size_ = size;
  }
}

void ByteBuffer::resize(size_t size) {
  if (size != capacity_) {
    reallocate(size);
  }
  size_ = size;
}

void ByteBuffer::shift(size_t size) {
  if (size >= size_) {
    size_ = 0;
    return;
  }

  auto newSize = size_ - size;
  std::memmove(data_, data_ + size, newSize);
  size_ = newSize;
}

void ByteBuffer::reallocate(size_t capacity) {
  auto* newData = allocateBuffer(capacity);

  if (newData != nullptr && data_ != nullptr) {
    std::memcpy(newData, data_, std::min(capacity_, capacity));
  }

  deallocateBuffer(data_);
  capacity_ = capacity;
  data_ = newData;

  if (size_ > capacity_) {
    size_ = capacity_;
  }
}

void ByteBuffer::shrinkToFit() {
  if (capacity_ != size_) {
    reallocate(size_);
  }
}

std::vector<uint8_t> ByteBuffer::toBytesVec() const {
  std::vector<uint8_t> out;
  out.reserve(size_);
  out.insert(out.end(), begin(), end());
  return out;
}

std::string_view ByteBuffer::toStringView() const {
  return std::string_view(reinterpret_cast<const char*>(data_), size_);
}

}  // namespace utils
}  // namespace pickup