#include <gtest/gtest.h>
#include <atomic>
#include <thread>
#include <vector>

#include "pickup/thread/CounterLatch.h"

using namespace pickup::thread;

TEST(CounterLatchTest, InitialCountZero) {
  CounterLatch latch;
  EXPECT_EQ(latch.getCount(), 0);
}

TEST(CounterLatchTest, CountUpAndDown) {
  CounterLatch latch;
  EXPECT_TRUE(latch.countUp());
  EXPECT_EQ(latch.getCount(), 1);
  latch.countDown();
  EXPECT_EQ(latch.getCount(), 0);
}

TEST(CounterLatchTest, WaitUnblocksWhenCountReachesZero) {
  CounterLatch latch;
  latch.countUp();
  std::atomic<bool> waited{false};
  std::thread t([&]() {
    latch.countDown();
    waited.store(true);
  });
  latch.wait();
  EXPECT_TRUE(waited.load());
  t.join();
}

TEST(CounterLatchTest, WaitBlocksUntilCountDown) {
  CounterLatch latch;
  latch.countUp();
  std::atomic<bool> passed{false};
  std::thread t([&]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    latch.countDown();
  });
  latch.wait();
  passed.store(true);
  t.join();
  EXPECT_TRUE(passed.load());
}

TEST(CounterLatchTest, WaitWithZeroCountDoesNotBlock) {
  CounterLatch latch;
  latch.wait();
  SUCCEED();
}

TEST(CounterLatchTest, CountUpReturnsFalseAfterWait) {
  CounterLatch latch;
  latch.wait();
  EXPECT_FALSE(latch.countUp());
}

TEST(CounterLatchTest, CountDownAfterZeroDoesNothing) {
  CounterLatch latch;
  EXPECT_EQ(latch.getCount(), 0);
  latch.countDown();
  EXPECT_EQ(latch.getCount(), 0);
}

TEST(CounterLatchTest, MultipleCountUp) {
  CounterLatch latch;
  EXPECT_TRUE(latch.countUp());
  EXPECT_TRUE(latch.countUp());
  EXPECT_TRUE(latch.countUp());
  EXPECT_EQ(latch.getCount(), 3);
  latch.countDown();
  EXPECT_EQ(latch.getCount(), 2);
  latch.countDown();
  EXPECT_EQ(latch.getCount(), 1);
  latch.countDown();
  EXPECT_EQ(latch.getCount(), 0);
}

TEST(CounterLatchTest, WaitThrowsIfCalledTwice) {
  CounterLatch latch;
  latch.wait();
  EXPECT_THROW(latch.wait(), std::runtime_error);
}

TEST(CounterLatchTest, ConcurrentCountUpAndWait) {
  CounterLatch latch;
  constexpr int kThreads = 5;
  std::vector<std::thread> threads;
  for (int i = 0; i < kThreads; ++i) {
    threads.emplace_back([&]() {
      latch.countUp();
    });
  }
  for (auto& t : threads) t.join();
  EXPECT_EQ(latch.getCount(), kThreads);
  for (int i = 0; i < kThreads; ++i) {
    latch.countDown();
  }
  EXPECT_EQ(latch.getCount(), 0);
}
