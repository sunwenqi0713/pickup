#include <gtest/gtest.h>
#include <atomic>
#include <cstddef>
#include <thread>
#include <vector>

#include "pickup/thread/MPSCQueue.h"

using namespace pickup::thread;

TEST(MPSCQueueTest, PushPop) {
  MPSCQueue<int> queue(16);
  EXPECT_TRUE(queue.tryPush(42));
  int val = 0;
  EXPECT_TRUE(queue.tryPop(val));
  EXPECT_EQ(val, 42);
}

TEST(MPSCQueueTest, PushPopMove) {
  MPSCQueue<std::string> queue(16);
  std::string msg = "hello";
  EXPECT_TRUE(queue.tryPush(std::move(msg)));
  std::string val;
  EXPECT_TRUE(queue.tryPop(val));
  EXPECT_EQ(val, "hello");
}

TEST(MPSCQueueTest, PopEmpty) {
  MPSCQueue<int> queue(16);
  int val = 0;
  EXPECT_FALSE(queue.tryPop(val));
}

TEST(MPSCQueueTest, PushFull) {
  MPSCQueue<int> queue(4);
  EXPECT_TRUE(queue.tryPush(1));
  EXPECT_TRUE(queue.tryPush(2));
  EXPECT_TRUE(queue.tryPush(3));
  EXPECT_TRUE(queue.tryPush(4));
  EXPECT_FALSE(queue.tryPush(5));
}

TEST(MPSCQueueTest, PushAfterPop) {
  MPSCQueue<int> queue(4);
  EXPECT_TRUE(queue.tryPush(1));
  EXPECT_TRUE(queue.tryPush(2));
  int val = 0;
  EXPECT_TRUE(queue.tryPop(val));
  EXPECT_EQ(val, 1);
  EXPECT_TRUE(queue.tryPush(3));
  EXPECT_TRUE(queue.tryPush(4));
}

TEST(MPSCQueueTest, Emplace) {
  struct Point {
    Point(int x, int y) : x_(x), y_(y) {}
    int x_, y_;
  };
  MPSCQueue<Point> queue(16);
  EXPECT_TRUE(queue.emplace(10, 20));
  Point p(0, 0);
  EXPECT_TRUE(queue.tryPop(p));
  EXPECT_EQ(p.x_, 10);
  EXPECT_EQ(p.y_, 20);
}

TEST(MPSCQueueTest, Size) {
  MPSCQueue<int> queue(8);
  EXPECT_TRUE(queue.empty());
  EXPECT_EQ(queue.size(), 0);
  EXPECT_TRUE(queue.tryPush(1));
  EXPECT_EQ(queue.size(), 1);
  EXPECT_TRUE(queue.tryPush(2));
  EXPECT_EQ(queue.size(), 2);
  int val = 0;
  EXPECT_TRUE(queue.tryPop(val));
  EXPECT_EQ(queue.size(), 1);
}

TEST(MPSCQueueTest, Capacity) {
  MPSCQueue<int> queue(10);
  EXPECT_EQ(queue.capacity(), 16);
}

TEST(MPSCQueueTest, CapacityExactPowerOfTwo) {
  MPSCQueue<int> queue(16);
  EXPECT_EQ(queue.capacity(), 16);
}

TEST(MPSCQueueTest, CapacityMinOne) {
  MPSCQueue<int> queue(0);
  EXPECT_EQ(queue.capacity(), 1);
}

TEST(MPSCQueueTest, CapacityOne) {
  MPSCQueue<int> queue(1);
  EXPECT_EQ(queue.capacity(), 1);
  EXPECT_TRUE(queue.tryPush(1));
  int val = 0;
  EXPECT_TRUE(queue.tryPop(val));
  EXPECT_EQ(val, 1);
}

TEST(MPSCQueueTest, FIFO) {
  MPSCQueue<int> queue(16);
  EXPECT_TRUE(queue.tryPush(10));
  EXPECT_TRUE(queue.tryPush(20));
  EXPECT_TRUE(queue.tryPush(30));
  int val = 0;
  EXPECT_TRUE(queue.tryPop(val));
  EXPECT_EQ(val, 10);
  EXPECT_TRUE(queue.tryPop(val));
  EXPECT_EQ(val, 20);
  EXPECT_TRUE(queue.tryPop(val));
  EXPECT_EQ(val, 30);
}

TEST(MPSCQueueTest, MultipleProducers) {
  MPSCQueue<int> queue(1024);
  std::atomic<int> produced{0};
  std::vector<std::thread> producers;
  constexpr int kProducers = 4;
  constexpr int kItemsPerProducer = 250;

  for (int t = 0; t < kProducers; ++t) {
    producers.emplace_back([&, t]() {
      for (int i = 0; i < kItemsPerProducer; ++i) {
        while (!queue.tryPush(t * kItemsPerProducer + i)) {
          std::this_thread::yield();
        }
        produced.fetch_add(1, std::memory_order_relaxed);
      }
    });
  }

  std::vector<int> received;
  while (static_cast<int>(received.size()) < kProducers * kItemsPerProducer) {
    int val = 0;
    if (queue.tryPop(val)) {
      received.push_back(val);
    }
  }

  for (auto& t : producers) {
    t.join();
  }

  EXPECT_EQ(received.size(), kProducers * kItemsPerProducer);
  EXPECT_EQ(produced.load(), kProducers * kItemsPerProducer);
}

TEST(MPSCQueueTest, DestructorWithElements) {
  auto queue = std::make_unique<MPSCQueue<int>>(4);
  EXPECT_TRUE(queue->tryPush(1));
  EXPECT_TRUE(queue->tryPush(2));
  EXPECT_TRUE(queue->tryPush(3));
  EXPECT_NO_THROW(queue.reset());
}

TEST(MPSCQueueTest, WrapAround) {
  MPSCQueue<int> queue(4);
  for (int round = 0; round < 10; ++round) {
    EXPECT_TRUE(queue.tryPush(round));
    int val = 0;
    EXPECT_TRUE(queue.tryPop(val));
    EXPECT_EQ(val, round);
  }
}
