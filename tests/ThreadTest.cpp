#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <string>
#include <thread>

#include "pickup/thread/Thread.h"

using namespace pickup::thread;

TEST(ThreadTest, DefaultConstruct) {
  Thread t;
  EXPECT_FALSE(t.joinable());
}

TEST(ThreadTest, CreateAndJoin) {
  std::atomic<bool> executed{false};
  {
    Thread t("worker", [&] { executed.store(true); });
    t.join();
  }
  EXPECT_TRUE(executed.load());
}

TEST(ThreadTest, ThreadName) {
  Thread t("mythread", [] { std::this_thread::sleep_for(std::chrono::milliseconds(10)); });
  EXPECT_EQ(t.name(), "mythread");
  t.join();
}

TEST(ThreadTest, GetId) {
  Thread t("ident", [] { std::this_thread::sleep_for(std::chrono::milliseconds(10)); });
  EXPECT_NE(t.getId(), std::thread::id());
  t.join();
}

TEST(ThreadTest, Joinable) {
  Thread t("joinable", [] { std::this_thread::sleep_for(std::chrono::milliseconds(10)); });
  EXPECT_TRUE(t.joinable());
  t.join();
  EXPECT_FALSE(t.joinable());
}

TEST(ThreadTest, Detach) {
  std::atomic<bool> started{false};
  std::atomic<bool> finished{false};
  {
    Thread t("detach", [&] {
      started.store(true);
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      finished.store(true);
    });
    t.detach();
    EXPECT_FALSE(t.joinable());
  }
  // 等待分离的线程执行完毕
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_TRUE(started.load());
  EXPECT_TRUE(finished.load());
}

TEST(ThreadTest, Callbacks) {
  std::atomic<bool> started{false};
  std::atomic<bool> terminated{false};
  {
  std::function<void(const std::string&)> onStart =
      [&](const std::string& name) { started.store(true); EXPECT_EQ(name, "cb"); };
  std::function<void(const std::string&)> onTerm =
      [&](const std::string& name) { terminated.store(true); EXPECT_EQ(name, "cb"); };
  Thread t("cb", onStart, onTerm,
           [] { std::this_thread::sleep_for(std::chrono::milliseconds(10)); });
    t.join();
  }
  EXPECT_TRUE(started.load());
  EXPECT_TRUE(terminated.load());
}

TEST(ThreadTest, ThisThreadGetId) {
  long id = this_thread::getThreadId();
  EXPECT_GT(id, 0);
}

TEST(ThreadTest, ThisThreadGetName) {
  std::string name = this_thread::getName();
  // 主线程名可能是空字符串或非空
  EXPECT_NO_THROW(this_thread::getName());
}

TEST(ThreadTest, ThisThreadSetAndGetName) {
  this_thread::setName("test-thread");
  EXPECT_EQ(this_thread::getName(), "test-thread");
}

TEST(ThreadTest, MoveSemantics) {
  Thread t1("movable", [] { std::this_thread::sleep_for(std::chrono::milliseconds(10)); });
  EXPECT_TRUE(t1.joinable());
  Thread t2(std::move(t1));
  EXPECT_FALSE(t1.joinable());
  EXPECT_TRUE(t2.joinable());
  EXPECT_EQ(t2.name(), "movable");
  t2.join();
}

TEST(ThreadTest, Swap) {
  Thread t1("first", [] { std::this_thread::sleep_for(std::chrono::milliseconds(10)); });
  Thread t2("second", [] { std::this_thread::sleep_for(std::chrono::milliseconds(10)); });
  t1.swap(t2);
  EXPECT_EQ(t1.name(), "second");
  EXPECT_EQ(t2.name(), "first");
  t1.join();
  t2.join();
}

TEST(ThreadTest, HardwareConcurrency) {
  auto count = Thread::hardwareConcurrency();
  EXPECT_GE(count, 1);
}

TEST(ThreadTest, PassArgs) {
  std::atomic<int> result{0};
  Thread t("args", [&](int a, int b) { result.store(a + b); }, 3, 4);
  t.join();
  EXPECT_EQ(result.load(), 7);
}
