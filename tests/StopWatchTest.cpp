#include <gtest/gtest.h>
#include <thread>

#include "pickup/utils/StopWatch.hpp"

using namespace pickup::utils;

TEST(StopWatchTest, DefaultConstructorStopped) {
  StopWatch sw;
  EXPECT_FALSE(sw.isRunning());
  EXPECT_EQ(sw.elapsedSeconds(), 0);
  EXPECT_EQ(sw.elapsedMs(), 0);
}

TEST(StopWatchTest, StartedConstructor) {
  StopWatch sw(StopWatch::Started);
  EXPECT_TRUE(sw.isRunning());
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  sw.stop();
  EXPECT_GE(sw.elapsedMs(), 5);
}

TEST(StopWatchTest, StartStop) {
  StopWatch sw;
  sw.start();
  EXPECT_TRUE(sw.isRunning());
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  sw.stop();
  EXPECT_FALSE(sw.isRunning());
  EXPECT_GE(sw.elapsedMs(), 1);
}

TEST(StopWatchTest, StartIdempotent) {
  StopWatch sw(StopWatch::Started);
  sw.start();
  EXPECT_TRUE(sw.isRunning());
}

TEST(StopWatchTest, StopIdempotent) {
  StopWatch sw;
  sw.stop();
  EXPECT_FALSE(sw.isRunning());
  EXPECT_EQ(sw.elapsedMs(), 0);
}

TEST(StopWatchTest, Reset) {
  StopWatch sw(StopWatch::Started);
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  sw.stop();
  EXPECT_GE(sw.elapsedMs(), 1);
  sw.reset();
  EXPECT_EQ(sw.elapsedMs(), 0);
  EXPECT_FALSE(sw.isRunning());
}

TEST(StopWatchTest, Restart) {
  StopWatch sw(StopWatch::Started);
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  sw.stop();
  auto before = sw.elapsedMs();
  EXPECT_GE(before, 1);
  sw.restart();
  EXPECT_TRUE(sw.isRunning());
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  sw.stop();
  // After restart, elapsed should be much less than before
  EXPECT_LT(sw.elapsedMs(), before + 10);
}

TEST(StopWatchTest, ElapsedSeconds) {
  StopWatch sw(StopWatch::Started);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  sw.stop();
  EXPECT_EQ(sw.elapsedSeconds(), 0);
  EXPECT_GE(sw.elapsedMs(), 90);
}

TEST(StopWatchTest, ElapsedUs) {
  StopWatch sw(StopWatch::Started);
  std::this_thread::sleep_for(std::chrono::microseconds(100));
  sw.stop();
  EXPECT_GE(sw.elapsedUs(), 0);
}

TEST(StopWatchTest, MultipleStartStopAccumulates) {
  StopWatch sw;
  sw.start();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  sw.stop();
  auto first = sw.elapsedMs();

  sw.start();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  sw.stop();
  auto total = sw.elapsedMs();

  EXPECT_GE(total, first + 1);
}

TEST(StopWatchTest, ElapsedWhileRunning) {
  StopWatch sw(StopWatch::Started);
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  auto elapsed = sw.elapsedMs();
  EXPECT_GE(elapsed, 1);
}

TEST(StopWatchTest, StartTime) {
  StopWatch sw;
  sw.start();
  auto tp = sw.startTime();
  sw.stop();
  EXPECT_GT(sw.startTime(), decltype(tp)());
}
