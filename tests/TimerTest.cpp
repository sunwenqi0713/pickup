#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <memory>
#include <thread>

#include "pickup/timer/Timer.h"

using namespace pickup::timer;

TEST(TimerTest, ScheduleAndExecute) {
  Timer timer;
  std::atomic<bool> executed{false};
  timer.schedule([&] { executed.store(true); }, std::chrono::milliseconds(5));
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  EXPECT_TRUE(executed.load());
}

TEST(TimerTest, ScheduleZeroDelay) {
  Timer timer;
  std::atomic<bool> executed{false};
  timer.schedule([&] { executed.store(true); }, std::chrono::milliseconds(0));
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  EXPECT_TRUE(executed.load());
}

TEST(TimerTest, ScheduleOrder) {
  Timer timer;
  std::vector<int> executionOrder;
  std::mutex orderMutex;
  // 延迟短的先调度，应先执行
  timer.schedule([&] {
    std::lock_guard<std::mutex> lock(orderMutex);
    executionOrder.push_back(1);
  }, std::chrono::milliseconds(20));
  timer.schedule([&] {
    std::lock_guard<std::mutex> lock(orderMutex);
    executionOrder.push_back(2);
  }, std::chrono::milliseconds(5));
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  ASSERT_EQ(executionOrder.size(), 2);
  EXPECT_EQ(executionOrder[0], 2);  // 5ms task executed first
  EXPECT_EQ(executionOrder[1], 1);  // 20ms task executed second
}

TEST(TimerTest, ScheduleMultiple) {
  Timer timer;
  std::atomic<int> counter{0};
  for (int i = 0; i < 5; ++i) {
    timer.schedule([&] { counter.fetch_add(1); }, std::chrono::milliseconds(5));
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(counter.load(), 5);
}

TEST(TimerTest, ScheduleRepeated) {
  Timer timer;
  std::atomic<int> counter{0};
  auto task = std::make_shared<TimerTaskAdapter>([&counter] {
    counter.fetch_add(1);
  });
  timer.scheduleRepeated(task, std::chrono::milliseconds(5));
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  EXPECT_GE(counter.load(), 2);
}

TEST(TimerTest, CancelBeforeExecution) {
  Timer timer;
  std::atomic<bool> executed{false};
  auto task = std::make_shared<TimerTaskAdapter>([&] { executed.store(true); });
  timer.schedule(task, std::chrono::milliseconds(50));
  bool cancelled = timer.cancel(task);
  EXPECT_TRUE(cancelled);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_FALSE(executed.load());
}

TEST(TimerTest, CancelNonExistent) {
  Timer timer;
  auto task = std::make_shared<TimerTaskAdapter>([] {});
  bool cancelled = timer.cancel(task);
  EXPECT_FALSE(cancelled);
}

TEST(TimerTest, IsScheduled) {
  Timer timer;
  auto task = std::make_shared<TimerTaskAdapter>([] {});
  EXPECT_FALSE(timer.isScheduled(task));
  timer.schedule(task, std::chrono::milliseconds(50));
  EXPECT_TRUE(timer.isScheduled(task));
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_FALSE(timer.isScheduled(task));
}

TEST(TimerTest, NegativeDelayThrows) {
  Timer timer;
  EXPECT_THROW(
      timer.schedule([] {}, std::chrono::milliseconds(-1)),
      std::invalid_argument);
}

TEST(TimerTest, DoubleScheduleThrows) {
  Timer timer;
  auto task = std::make_shared<TimerTaskAdapter>([] {});
  timer.schedule(task, std::chrono::milliseconds(50));
  EXPECT_THROW(
      timer.schedule(task, std::chrono::milliseconds(50)),
      std::invalid_argument);
}

TEST(TimerTest, Reschedule) {
  Timer timer;
  std::atomic<bool> executed{false};
  auto task = std::make_shared<TimerTaskAdapter>([&] { executed.store(true); });
  timer.schedule(task, std::chrono::milliseconds(100));
  timer.reschedule(task, std::chrono::milliseconds(5));
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  EXPECT_TRUE(executed.load());
}

TEST(TimerTest, StopTimer) {
  Timer timer;
  std::atomic<bool> executed{false};
  timer.schedule([&] { executed.store(true); }, std::chrono::milliseconds(5));
  timer.stop();
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  // stop 后剩余任务可能已执行或丢弃
  EXPECT_NO_THROW(timer.stop());
}
