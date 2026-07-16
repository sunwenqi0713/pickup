#include <gtest/gtest.h>
#include <atomic>
#include <cstddef>
#include <thread>
#include <vector>

#include "pickup/thread/SPSCQueue.h"

using namespace pickup::thread;

TEST(SPSCQueueTest, PushPop) {
  SPSCQueue<int> queue(16);
  EXPECT_TRUE(queue.tryPush(42));
  int val = 0;
  EXPECT_TRUE(queue.tryPop(val));
  EXPECT_EQ(val, 42);
}

TEST(SPSCQueueTest, PushPopMove) {
  SPSCQueue<std::string> queue(16);
  std::string msg = "hello";
  EXPECT_TRUE(queue.tryPush(std::move(msg)));
  std::string val;
  EXPECT_TRUE(queue.tryPop(val));
  EXPECT_EQ(val, "hello");
}

TEST(SPSCQueueTest, PopEmpty) {
  SPSCQueue<int> queue(16);
  int val = 0;
  EXPECT_FALSE(queue.tryPop(val));
}

TEST(SPSCQueueTest, PushFull) {
  SPSCQueue<int> queue(4);
  EXPECT_TRUE(queue.tryPush(1));
  EXPECT_TRUE(queue.tryPush(2));
  EXPECT_TRUE(queue.tryPush(3));
  EXPECT_TRUE(queue.tryPush(4));
  EXPECT_FALSE(queue.tryPush(5));
}

TEST(SPSCQueueTest, PushAfterPop) {
  SPSCQueue<int> queue(4);
  EXPECT_TRUE(queue.tryPush(1));
  EXPECT_TRUE(queue.tryPush(2));
  int val = 0;
  EXPECT_TRUE(queue.tryPop(val));
  EXPECT_EQ(val, 1);
  EXPECT_TRUE(queue.tryPush(3));
  EXPECT_TRUE(queue.tryPush(4));
}

TEST(SPSCQueueTest, Emplace) {
  struct Point {
    Point(int x, int y) : x_(x), y_(y) {}
    int x_, y_;
  };
  SPSCQueue<Point> queue(16);
  EXPECT_TRUE(queue.emplace(10, 20));
  Point p(0, 0);
  EXPECT_TRUE(queue.tryPop(p));
  EXPECT_EQ(p.x_, 10);
  EXPECT_EQ(p.y_, 20);
}

TEST(SPSCQueueTest, Size) {
  SPSCQueue<int> queue(8);
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

TEST(SPSCQueueTest, Capacity) {
  SPSCQueue<int> queue(10);
  EXPECT_EQ(queue.capacity(), 16);
}

TEST(SPSCQueueTest, CapacityExactPowerOfTwo) {
  SPSCQueue<int> queue(16);
  EXPECT_EQ(queue.capacity(), 16);
}

TEST(SPSCQueueTest, CapacityMinOne) {
  SPSCQueue<int> queue(0);
  EXPECT_EQ(queue.capacity(), 1);
}

TEST(SPSCQueueTest, CapacityOne) {
  SPSCQueue<int> queue(1);
  EXPECT_EQ(queue.capacity(), 1);
  EXPECT_TRUE(queue.tryPush(1));
  int val = 0;
  EXPECT_TRUE(queue.tryPop(val));
  EXPECT_EQ(val, 1);
}

TEST(SPSCQueueTest, FIFO) {
  SPSCQueue<int> queue(16);
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

TEST(SPSCQueueTest, DestructorWithElements) {
  auto queue = std::make_unique<SPSCQueue<int>>(4);
  EXPECT_TRUE(queue->tryPush(1));
  EXPECT_TRUE(queue->tryPush(2));
  EXPECT_TRUE(queue->tryPush(3));
  EXPECT_NO_THROW(queue.reset());
}

TEST(SPSCQueueTest, WrapAround) {
  SPSCQueue<int> queue(4);
  for (int round = 0; round < 10; ++round) {
    EXPECT_TRUE(queue.tryPush(round));
    int val = 0;
    EXPECT_TRUE(queue.tryPop(val));
    EXPECT_EQ(val, round);
  }
}

TEST(SPSCQueueTest, SingleProducerSingleConsumer) {
  SPSCQueue<int> queue(256);
  constexpr int kItems = 10000;
  std::atomic<bool> done{false};

  std::thread producer([&]() {
    for (int i = 0; i < kItems; ++i) {
      while (!queue.tryPush(i)) {
        std::this_thread::yield();
      }
    }
  });

  std::thread consumer([&]() {
    int expected = 0;
    int val = 0;
    while (expected < kItems) {
      if (queue.tryPop(val)) {
        EXPECT_EQ(val, expected);
        ++expected;
      }
    }
    done = true;
  });

  producer.join();
  consumer.join();
  EXPECT_TRUE(done);
}

TEST(SPSCQueueTest, SequentialPushPopStress) {
  SPSCQueue<int> queue(8);

  auto producer = [&](int start, int count) {
    for (int i = 0; i < count; ++i) {
      while (!queue.tryPush(start + i)) {
        std::this_thread::yield();
      }
    }
  };

  auto consumer = [&](int count) {
    int val = 0;
    for (int i = 0; i < count; ++i) {
      while (!queue.tryPop(val)) {
        std::this_thread::yield();
      }
    }
    return val;
  };

  std::thread t1(producer, 0, 500);
  t1.join();

  int last = consumer(500);
  EXPECT_EQ(last, 499);
  EXPECT_TRUE(queue.empty());
}

TEST(SPSCQueueTest, ProducerConsumerInterleaved) {
  SPSCQueue<int> queue(4);
  constexpr int kIterations = 1000;

  std::thread producer([&]() {
    for (int i = 0; i < kIterations; ++i) {
      while (!queue.tryPush(i)) {
        std::this_thread::yield();
      }
    }
  });

  std::thread consumer([&]() {
    int expected = 0;
    int val = 0;
    while (expected < kIterations) {
      if (queue.tryPop(val)) {
        EXPECT_EQ(val, expected);
        ++expected;
      }
    }
  });

  producer.join();
  consumer.join();
}
