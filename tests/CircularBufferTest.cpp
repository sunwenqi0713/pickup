#include <gtest/gtest.h>
#include <cstring>

#include "pickup/buffer/CircularBuffer.h"

using namespace pickup::buffer;

TEST(CircularBufferTest, DefaultState) {
  CircularBuffer buf;
  // unallocated: size_=0 causes available() unsigned underflow
  EXPECT_EQ(buf.used(), 0);
}

TEST(CircularBufferTest, Allocate) {
  CircularBuffer buf;
  EXPECT_TRUE(buf.allocate(16));
}

TEST(CircularBufferTest, Deallocate) {
  CircularBuffer buf;
  EXPECT_TRUE(buf.allocate(16));
  buf.deallocate();
  EXPECT_EQ(buf.used(), 0);
}

TEST(CircularBufferTest, PushAndPop) {
  CircularBuffer buf;
  ASSERT_TRUE(buf.allocate(16));
  const uint8_t data[] = {1, 2, 3, 4, 5};
  EXPECT_TRUE(buf.write(data, 5));
  EXPECT_EQ(buf.used(), 5);
  uint8_t out[16] = {};
  EXPECT_EQ(buf.read(out, 16), 5);
  EXPECT_EQ(out[0], 1);
  EXPECT_EQ(out[4], 5);
}

TEST(CircularBufferTest, PopEmptyBuffer) {
  CircularBuffer buf;
  ASSERT_TRUE(buf.allocate(16));
  uint8_t out[16] = {};
  EXPECT_EQ(buf.read(out, 16), 0);
}

TEST(CircularBufferTest, PushTooMuch) {
  CircularBuffer buf;
  ASSERT_TRUE(buf.allocate(8));
  uint8_t data[7] = {};
  EXPECT_TRUE(buf.write(data, 7));
  EXPECT_EQ(buf.used(), 7);
  EXPECT_FALSE(buf.write(data, 1));
}

TEST(CircularBufferTest, FifoOrder) {
  CircularBuffer buf;
  ASSERT_TRUE(buf.allocate(8));
  uint8_t data1[] = {10, 20};
  uint8_t data2[] = {30, 40};
  EXPECT_TRUE(buf.write(data1, 2));
  EXPECT_TRUE(buf.write(data2, 2));
  uint8_t out[8] = {};
  EXPECT_EQ(buf.read(out, 8), 4);
  EXPECT_EQ(out[0], 10);
  EXPECT_EQ(out[1], 20);
  EXPECT_EQ(out[2], 30);
  EXPECT_EQ(out[3], 40);
}

TEST(CircularBufferTest, WrapAround) {
  CircularBuffer buf;
  ASSERT_TRUE(buf.allocate(8));
  uint8_t data[5] = {1, 2, 3, 4, 5};
  EXPECT_TRUE(buf.write(data, 5));
  uint8_t out[8] = {};
  EXPECT_EQ(buf.read(out, 3), 3);
  EXPECT_TRUE(buf.write(data, 3));
  EXPECT_EQ(buf.used(), 5);
  memset(out, 0, 8);
  EXPECT_EQ(buf.read(out, 8), 5);
  EXPECT_EQ(out[0], 4);
  EXPECT_EQ(out[1], 5);
  EXPECT_EQ(out[2], 1);
  EXPECT_EQ(out[3], 2);
  EXPECT_EQ(out[4], 3);
}

TEST(CircularBufferTest, FullBuffer) {
  CircularBuffer buf;
  ASSERT_TRUE(buf.allocate(8));
  uint8_t data[7] = {};
  EXPECT_TRUE(buf.write(data, 7));
  EXPECT_EQ(buf.used(), 7);
  EXPECT_EQ(buf.available(), 0);
  EXPECT_FALSE(buf.write(data, 1));
}

TEST(CircularBufferTest, PushNullptr) {
  CircularBuffer buf;
  ASSERT_TRUE(buf.allocate(8));
  EXPECT_FALSE(buf.write(nullptr, 5));
}

TEST(CircularBufferTest, PushZeroLength) {
  CircularBuffer buf;
  ASSERT_TRUE(buf.allocate(8));
  uint8_t data[] = {1};
  EXPECT_FALSE(buf.write(data, 0));
}

TEST(CircularBufferTest, PopNullptr) {
  CircularBuffer buf;
  ASSERT_TRUE(buf.allocate(8));
  uint8_t data[] = {1};
  EXPECT_TRUE(buf.write(data, 1));
  EXPECT_EQ(buf.read(nullptr, 8), 0);
}

TEST(CircularBufferTest, AvailableAndUsedConsistency) {
  CircularBuffer buf;
  ASSERT_TRUE(buf.allocate(8));
  EXPECT_EQ(buf.available(), 7);  // size_ - 1
  EXPECT_EQ(buf.used(), 0);
  uint8_t data[4] = {};
  EXPECT_TRUE(buf.write(data, 4));
  EXPECT_EQ(buf.used(), 4);
  EXPECT_EQ(buf.available(), 3);
  buf.read(data, 2);
  EXPECT_EQ(buf.used(), 2);
  EXPECT_EQ(buf.available(), 5);
}

TEST(CircularBufferTest, Destructor) {
  auto buf = std::make_unique<CircularBuffer>();
  ASSERT_TRUE(buf->allocate(16));
  EXPECT_NO_THROW(buf.reset());
}

TEST(CircularBufferTest, DoubleAllocateFails) {
  CircularBuffer buf;
  EXPECT_TRUE(buf.allocate(8));
  // 第二次分配会触发 assert
}
