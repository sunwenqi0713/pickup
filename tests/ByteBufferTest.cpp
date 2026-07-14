#include <gtest/gtest.h>
#include <cstring>
#include <string>
#include <string_view>

#include "pickup/utils/ByteBuffer.h"

using namespace pickup::utils;

TEST(ByteBufferTest, DefaultConstructor) {
  ByteBuffer buf;
  EXPECT_TRUE(buf.empty());
  EXPECT_EQ(buf.size(), 0);
  EXPECT_EQ(buf.capacity(), 0);
  EXPECT_EQ(buf.data(), nullptr);
}

TEST(ByteBufferTest, StringConstructor) {
  ByteBuffer buf("hello");
  EXPECT_FALSE(buf.empty());
  EXPECT_EQ(buf.size(), 5);
  EXPECT_EQ(buf.toStringView(), "hello");
}

TEST(ByteBufferTest, StringViewConstructor) {
  std::string_view sv = "world";
  ByteBuffer buf(sv);
  EXPECT_EQ(buf.toStringView(), "world");
}

TEST(ByteBufferTest, RangeConstructor) {
  ByteBuffer::Byte data[] = {1, 2, 3, 4, 5};
  ByteBuffer buf(data, data + 5);
  ASSERT_EQ(buf.size(), 5);
  EXPECT_EQ(buf[0], 1);
  EXPECT_EQ(buf[4], 5);
}

TEST(ByteBufferTest, AppendByteRange) {
  ByteBuffer buf;
  ByteBuffer::Byte data[] = {10, 20, 30};
  buf.append(data, data + 3);
  ASSERT_EQ(buf.size(), 3);
  EXPECT_EQ(buf[0], 10);
  EXPECT_EQ(buf[2], 30);
}

TEST(ByteBufferTest, AppendStringView) {
  ByteBuffer buf;
  buf.append(std::string_view("abc"));
  EXPECT_EQ(buf.size(), 3);
  EXPECT_EQ(buf.toStringView(), "abc");
}

TEST(ByteBufferTest, AppendChar) {
  ByteBuffer buf;
  buf.append('x');
  buf.append('y');
  ASSERT_EQ(buf.size(), 2);
  EXPECT_EQ(buf[0], 'x');
  EXPECT_EQ(buf[1], 'y');
}

TEST(ByteBufferTest, AppendByte) {
  ByteBuffer buf;
  buf.append(static_cast<ByteBuffer::Byte>(0xFF));
  ASSERT_EQ(buf.size(), 1);
  EXPECT_EQ(buf[0], 0xFF);
}

TEST(ByteBufferTest, AppendWritable) {
  ByteBuffer buf;
  auto* ptr = buf.appendWritable(4);
  ASSERT_NE(ptr, nullptr);
  ASSERT_EQ(buf.size(), 4);
  ptr[0] = 0x0A;
  ptr[3] = 0x0D;
  EXPECT_EQ(buf[0], 0x0A);
  EXPECT_EQ(buf[3], 0x0D);
}

TEST(ByteBufferTest, SetRange) {
  ByteBuffer buf;
  ByteBuffer::Byte data[] = {1, 2, 3};
  buf.append(data, data + 3);
  ByteBuffer::Byte newData[] = {4, 5};
  buf.set(newData, newData + 2);
  ASSERT_EQ(buf.size(), 2);
  EXPECT_EQ(buf[0], 4);
  EXPECT_EQ(buf[1], 5);
}

TEST(ByteBufferTest, SetInitializerList) {
  ByteBuffer buf;
  buf.set({0xAA, 0xBB, 0xCC});
  ASSERT_EQ(buf.size(), 3);
  EXPECT_EQ(buf[0], 0xAA);
  EXPECT_EQ(buf[2], 0xCC);
}

TEST(ByteBufferTest, Shift) {
  ByteBuffer buf("abcdef");
  buf.shift(2);
  EXPECT_EQ(buf.size(), 4);
  EXPECT_EQ(buf.toStringView(), "cdef");
  buf.shift(4);
  EXPECT_TRUE(buf.empty());
}

TEST(ByteBufferTest, ResizeShrink) {
  ByteBuffer buf("hello world");
  buf.resize(5);
  EXPECT_EQ(buf.size(), 5);
  EXPECT_EQ(buf.toStringView(), "hello");
}

TEST(ByteBufferTest, ResizeGrow) {
  ByteBuffer buf("abc");
  buf.resize(6);
  ASSERT_EQ(buf.size(), 6);
  EXPECT_EQ(buf[0], 'a');
  EXPECT_EQ(buf[3], 0);
}

TEST(ByteBufferTest, Clear) {
  ByteBuffer buf("data");
  ASSERT_FALSE(buf.empty());
  buf.clear();
  EXPECT_TRUE(buf.empty());
  EXPECT_EQ(buf.size(), 0);
}

TEST(ByteBufferTest, Reserve) {
  ByteBuffer buf;
  buf.reserve(128);
  EXPECT_GE(buf.capacity(), 128);
  EXPECT_EQ(buf.size(), 0);
}

TEST(ByteBufferTest, ShrinkToFit) {
  ByteBuffer buf("hello");
  buf.reserve(1024);
  buf.shrinkToFit();
  EXPECT_EQ(buf.capacity(), buf.size());
}

TEST(ByteBufferTest, CopyConstructor) {
  ByteBuffer original("copy");
  ByteBuffer copy(original);
  EXPECT_EQ(copy.toStringView(), "copy");
  EXPECT_EQ(original.toStringView(), "copy");
  EXPECT_NE(copy.data(), original.data());
}

TEST(ByteBufferTest, MoveConstructor) {
  ByteBuffer original("move");
  auto* origPtr = original.data();
  ByteBuffer moved(std::move(original));
  EXPECT_EQ(moved.toStringView(), "move");
  EXPECT_EQ(moved.data(), origPtr);
}

TEST(ByteBufferTest, Assignment) {
  ByteBuffer a("aaa");
  ByteBuffer b("bb");
  b = a;
  EXPECT_EQ(b.toStringView(), "aaa");
  EXPECT_NE(b.data(), a.data());
}

TEST(ByteBufferTest, Equality) {
  ByteBuffer a("same");
  ByteBuffer b("same");
  ByteBuffer c("different");
  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a == c);
  EXPECT_TRUE(a != c);
}

TEST(ByteBufferTest, ToBytesVec) {
  ByteBuffer buf("test");
  auto vec = buf.toBytesVec();
  ASSERT_EQ(vec.size(), 4);
  EXPECT_EQ(vec[0], 't');
  EXPECT_EQ(vec[3], 't');
}

TEST(ByteBufferTest, OperatorIndex) {
  ByteBuffer buf("ABCD");
  EXPECT_EQ(buf[0], 'A');
  EXPECT_EQ(buf[3], 'D');
  buf[1] = 'X';
  EXPECT_EQ(buf[1], 'X');
}

TEST(ByteBufferTest, NullTerminatedConstructor) {
  ByteBuffer buf(static_cast<const char*>("hello"));
  EXPECT_EQ(buf.size(), 5);
  EXPECT_EQ(buf.toStringView(), "hello");
}

TEST(ByteBufferTest, LargeAppend) {
  ByteBuffer buf;
  const size_t count = 10000;
  buf.reserve(count);
  for (size_t i = 0; i < count; ++i) {
    buf.append(static_cast<ByteBuffer::Byte>(i & 0xFF));
  }
  ASSERT_EQ(buf.size(), count);
  for (size_t i = 0; i < count; ++i) {
    EXPECT_EQ(buf[i], static_cast<ByteBuffer::Byte>(i & 0xFF));
  }
}
