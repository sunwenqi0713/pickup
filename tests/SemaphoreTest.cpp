#include <gtest/gtest.h>
#include <atomic>
#include <thread>
#include <vector>

#include "pickup/thread/Semaphore.h"

using namespace pickup::thread;

TEST(SemaphoreTest, DefaultCountZero) {
  Semaphore sem;
  std::atomic<bool> acquired{false};
  std::thread t([&]() {
    sem.acquire();
    acquired.store(true);
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  EXPECT_FALSE(acquired.load());
  sem.release();
  t.join();
  EXPECT_TRUE(acquired.load());
}

TEST(SemaphoreTest, InitialCountNonZero) {
  Semaphore sem(2);
  sem.acquire();
  sem.acquire();
  std::atomic<bool> blocked{false};
  std::thread t([&]() {
    blocked.store(true);
    sem.acquire();
    blocked.store(false);
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  EXPECT_TRUE(blocked.load());
  sem.release();
  t.join();
}

TEST(SemaphoreTest, ReleaseMultiple) {
  Semaphore sem;
  sem.release();
  sem.release();
  sem.acquire();
  sem.acquire();
  std::atomic<bool> blocked{false};
  std::thread t([&]() {
    blocked.store(true);
    sem.acquire();
    blocked.store(false);
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  EXPECT_TRUE(blocked.load());
  sem.release();
  t.join();
  EXPECT_FALSE(blocked.load());
}

TEST(SemaphoreTest, ProducerConsumer) {
  Semaphore sem;
  std::atomic<int> counter{0};
  std::thread consumer([&]() {
    sem.acquire();
    counter.store(42);
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  EXPECT_EQ(counter.load(), 0);
  sem.release();
  consumer.join();
  EXPECT_EQ(counter.load(), 42);
}

TEST(SemaphoreTest, MultipleConsumers) {
  Semaphore sem;
  std::atomic<int> count{0};
  std::vector<std::thread> consumers;
  for (int i = 0; i < 3; ++i) {
    consumers.emplace_back([&]() {
      sem.acquire();
      count.fetch_add(1);
    });
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  EXPECT_EQ(count.load(), 0);
  sem.release();
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  EXPECT_EQ(count.load(), 1);
  sem.release();
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  EXPECT_EQ(count.load(), 2);
  sem.release();
  for (auto& t : consumers) t.join();
  EXPECT_EQ(count.load(), 3);
}
