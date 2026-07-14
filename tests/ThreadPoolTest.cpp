#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <future>
#include <thread>

#include "pickup/thread/ThreadPool.h"

using namespace pickup::thread;

TEST(ThreadPoolTest, StartAndStop) {
  ThreadPool pool("test");
  pool.start(2);
  EXPECT_TRUE(pool.isRunning());
  pool.stop();
  EXPECT_FALSE(pool.isRunning());
}

TEST(ThreadPoolTest, AddTaskExecutes) {
  ThreadPool pool("exec");
  pool.start(1);
  std::atomic<bool> executed{false};
  pool.addTask([&] { executed.store(true); });
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  EXPECT_TRUE(executed.load());
  pool.stop();
}

TEST(ThreadPoolTest, SubmitReturnsValue) {
  ThreadPool pool("submit");
  pool.start(2);
  auto future = pool.submit([] { return 42; });
  EXPECT_EQ(future.get(), 42);
  pool.stop();
}

TEST(ThreadPoolTest, SubmitWithArgs) {
  ThreadPool pool("args");
  pool.start(2);
  auto future = pool.submit([](int a, int b) { return a + b; }, 10, 20);
  EXPECT_EQ(future.get(), 30);
  pool.stop();
}

TEST(ThreadPoolTest, SubmitException) {
  ThreadPool pool("except");
  pool.start(1);
  auto future = pool.submit([] { throw std::runtime_error("oops"); return 0; });
  EXPECT_THROW(future.get(), std::runtime_error);
  pool.stop();
}

TEST(ThreadPoolTest, TryAddTask) {
  ThreadPool pool("try");
  pool.start(1);
  std::atomic<bool> executed{false};
  bool added = pool.tryAddTask([&] { executed.store(true); });
  EXPECT_TRUE(added);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  EXPECT_TRUE(executed.load());
  pool.stop();
}

TEST(ThreadPoolTest, TryAddTaskFullQueue) {
  ThreadPool pool("full");
  pool.setMaxQueueSize(2);
  pool.start(1);
  std::atomic<int> count{0};
  // 占满队列（1 执行中 + maxQueueSize 排队）
  for (int i = 0; i < 3; ++i) {
    pool.addTask([&] {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      count.fetch_add(1);
    });
  }
  // 再提交应失败
  bool added = pool.tryAddTask([&] { count.fetch_add(1); });
  EXPECT_FALSE(added);
  pool.waitForAllDone();
  pool.stop();
}

TEST(ThreadPoolTest, TryAddTaskTimeout) {
  ThreadPool pool("timeout");
  pool.setMaxQueueSize(0);  // 无限制队列深度
  pool.start(1);
  // 先提交一个长时间任务占满工作线程，再提交一个填充队列
  std::atomic<bool> block{true};
  pool.addTask([&] {
    while (block.load()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  });
  // 给 worker 一点时间取出第一个任务
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  // 队列现在是空的（任务在执行中），再提交一个填充队列
  pool.setMaxQueueSize(1);
  pool.addTask([&] { std::this_thread::sleep_for(std::chrono::milliseconds(200)); });
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  // 队列满，超时应失败
  bool added = pool.tryAddTask([] {}, std::chrono::milliseconds(10));
  EXPECT_FALSE(added);
  block.store(false);
  pool.waitForAllDone();
  pool.stop();
}

TEST(ThreadPoolTest, WaitForAllDone) {
  ThreadPool pool("drain");
  pool.start(2);
  std::atomic<int> counter{0};
  for (int i = 0; i < 10; ++i) {
    pool.addTask([&] {
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
      counter.fetch_add(1);
    });
  }
  pool.waitForAllDone();
  EXPECT_EQ(counter.load(), 10);
  pool.stop();
}

TEST(ThreadPoolTest, MultipleTasks) {
  ThreadPool pool("multi");
  pool.start(4);
  std::atomic<int> counter{0};
  constexpr int kTaskCount = 100;
  for (int i = 0; i < kTaskCount; ++i) {
    pool.addTask([&] { counter.fetch_add(1); });
  }
  pool.waitForAllDone();
  EXPECT_EQ(counter.load(), kTaskCount);
  pool.stop();
}

TEST(ThreadPoolTest, Restart) {
  ThreadPool pool("restart");
  pool.start(1);
  pool.stop();
  pool.start(2);
  EXPECT_TRUE(pool.isRunning());
  std::atomic<bool> executed{false};
  pool.addTask([&] { executed.store(true); });
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  EXPECT_TRUE(executed.load());
  pool.stop();
}

TEST(ThreadPoolTest, AddTaskWithoutStart) {
  ThreadPool pool("nostart");
  std::atomic<bool> executed{false};
  pool.addTask([&] { executed.store(true); });
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  EXPECT_FALSE(executed.load());
}

TEST(ThreadPoolTest, Name) {
  ThreadPool pool("mypool");
  EXPECT_EQ(pool.name(), "mypool");
}

TEST(ThreadPoolTest, QueueSize) {
  ThreadPool pool("qsize");
  EXPECT_EQ(pool.queueSize(), 0);
  pool.start(1);
  EXPECT_EQ(pool.queueSize(), 0);
  pool.addTask([&] { std::this_thread::sleep_for(std::chrono::milliseconds(100)); });
  EXPECT_GE(pool.queueSize(), 0);
  pool.waitForAllDone();
  pool.stop();
}
