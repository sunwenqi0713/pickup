#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <thread>

#include "pickup/thread/Event.h"

using namespace pickup::thread;

TEST(EventTest, AutoResetInitiallyNotSet) {
  Event ev(false);
  EXPECT_FALSE(ev.wait(0));
}

TEST(EventTest, AutoResetSetAndWait) {
  Event ev(false);
  ev.set();
  EXPECT_TRUE(ev.wait(100));
}

TEST(EventTest, AutoResetAutoResets) {
  Event ev(false);
  ev.set();
  EXPECT_TRUE(ev.wait(100));   // first waiter gets it
  EXPECT_FALSE(ev.wait(0));     // auto-reset: not set anymore
}

TEST(EventTest, AutoResetMultiThread) {
  Event ev(false);
  std::atomic<bool> ready{false};
  std::thread t([&] {
    ready.store(true);
    EXPECT_TRUE(ev.wait(2000));
  });
  while (!ready.load()) {
    std::this_thread::yield();
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  ev.set();
  t.join();
}

TEST(EventTest, AutoResetOnlyWakesOne) {
  Event ev(false);
  std::atomic<int> woken{0};
  std::thread t1([&] { if (ev.wait(500)) woken.fetch_add(1); });
  std::thread t2([&] { if (ev.wait(500)) woken.fetch_add(1); });
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  ev.set();
  t1.join();
  t2.join();
  // 自动重置只会唤醒一个等待线程
  EXPECT_EQ(woken.load(), 1);
}

TEST(EventTest, ManualResetInitiallyNotSet) {
  Event ev(true);
  EXPECT_FALSE(ev.wait(0));
}

TEST(EventTest, ManualResetSetAndWait) {
  Event ev(true);
  ev.set();
  EXPECT_TRUE(ev.wait(0));
}

TEST(EventTest, ManualResetStaysSet) {
  Event ev(true);
  ev.set();
  EXPECT_TRUE(ev.wait(0));
  EXPECT_TRUE(ev.wait(0));  // 手动重置保持触发
}

TEST(EventTest, ManualResetReset) {
  Event ev(true);
  ev.set();
  EXPECT_TRUE(ev.wait(0));
  ev.reset();
  EXPECT_FALSE(ev.wait(0));
}

TEST(EventTest, ManualResetWakesAll) {
  Event ev(true);
  std::atomic<int> woken{0};
  std::thread t1([&] { if (ev.wait(500)) woken.fetch_add(1); });
  std::thread t2([&] { if (ev.wait(500)) woken.fetch_add(1); });
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  ev.set();
  t1.join();
  t2.join();
  // 手动重置唤醒所有等待线程
  EXPECT_EQ(woken.load(), 2);
}

TEST(EventTest, WaitTimeout) {
  Event ev(false);
  EXPECT_FALSE(ev.wait(50));
}

TEST(EventTest, ImmediateTimeout) {
  Event ev(false);
  EXPECT_FALSE(ev.wait(Event::TIMEOUT_IMMEDIATE));
}

TEST(EventTest, SetBeforeWait) {
  Event ev(false);
  ev.set();
  // 事件已触发，即时检查应成功
  EXPECT_TRUE(ev.wait(Event::TIMEOUT_IMMEDIATE));
}

TEST(EventTest, MultipleSet) {
  Event ev(false);
  ev.set();
  ev.set();  // 重复 set 应安全
  EXPECT_TRUE(ev.wait(0));
}
