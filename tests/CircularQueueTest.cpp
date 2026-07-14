#include <gtest/gtest.h>
#include <atomic>
#include <thread>
#include <vector>

#include "pickup/utils/CircularQueue.hpp"

using namespace pickup::utils;

TEST(CircularQueueTest, DefaultState) {
  CircularQueue<int> q(8);
  EXPECT_TRUE(q.empty());
  EXPECT_FALSE(q.full());
  EXPECT_EQ(q.size(), 0);
  EXPECT_EQ(q.capacity(), 8);
}

TEST(CircularQueueTest, EnqueueDequeue) {
  CircularQueue<int> q(4);
  EXPECT_TRUE(q.enqueue(10));
  EXPECT_TRUE(q.enqueue(20));
  EXPECT_EQ(q.size(), 2);
  int val = 0;
  EXPECT_TRUE(q.dequeue(val));
  EXPECT_EQ(val, 10);
  EXPECT_TRUE(q.dequeue(val));
  EXPECT_EQ(val, 20);
}

TEST(CircularQueueTest, FifoOrder) {
  CircularQueue<int> q(8);
  q.enqueue(1);
  q.enqueue(2);
  q.enqueue(3);
  int val = 0;
  q.dequeue(val);
  EXPECT_EQ(val, 1);
  q.dequeue(val);
  EXPECT_EQ(val, 2);
  q.dequeue(val);
  EXPECT_EQ(val, 3);
}

TEST(CircularQueueTest, TryEnqueue) {
  CircularQueue<int> q(2);
  EXPECT_TRUE(q.tryEnqueue(1));
  EXPECT_TRUE(q.tryEnqueue(2));
  EXPECT_FALSE(q.tryEnqueue(3));
}

TEST(CircularQueueTest, TryDequeue) {
  CircularQueue<int> q(4);
  int val = 0;
  EXPECT_FALSE(q.tryDequeue(val));
  q.enqueue(42);
  EXPECT_TRUE(q.tryDequeue(val));
  EXPECT_EQ(val, 42);
}

TEST(CircularQueueTest, FullQueue) {
  CircularQueue<int> q(2);
  q.enqueue(1);
  q.enqueue(2);
  EXPECT_TRUE(q.full());
}

TEST(CircularQueueTest, ClosePreventsEnqueue) {
  CircularQueue<int> q(4);
  q.close();
  EXPECT_FALSE(q.enqueue(1));
  EXPECT_TRUE(q.isClosed());
}

TEST(CircularQueueTest, CloseWithEmptyDrain) {
  CircularQueue<int> q(4);
  q.enqueue(1);
  q.enqueue(2);
  q.close();
  int val = 0;
  EXPECT_TRUE(q.dequeue(val));
  EXPECT_EQ(val, 1);
  EXPECT_TRUE(q.dequeue(val));
  EXPECT_EQ(val, 2);
  EXPECT_FALSE(q.dequeue(val));
}

TEST(CircularQueueTest, BlockingDequeueUnblocksOnClose) {
  CircularQueue<int> q(4);
  std::atomic<bool> received{false};
  std::thread t([&]() {
    int val = 0;
    bool ok = q.dequeue(val);
    received.store(ok);
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(30));
  q.close();
  t.join();
  EXPECT_FALSE(received.load());
}

TEST(CircularQueueTest, BlockingEnqueueUnblocksOnClose) {
  CircularQueue<int> q(1);
  q.enqueue(1);
  std::atomic<bool> sent{false};
  std::thread t([&]() {
    bool ok = q.enqueue(2);
    sent.store(ok);
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(30));
  q.close();
  t.join();
  EXPECT_FALSE(sent.load());
}

TEST(CircularQueueTest, ProducerConsumer) {
  CircularQueue<int> q(8);
  constexpr int kCount = 50;
  std::thread producer([&]() {
    for (int i = 0; i < kCount; ++i) {
      q.enqueue(i);
    }
  });
  std::thread consumer([&]() {
    for (int i = 0; i < kCount; ++i) {
      int val = -1;
      q.dequeue(val);
      EXPECT_EQ(val, i);
    }
  });
  producer.join();
  consumer.join();
}
