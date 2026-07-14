#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

#include "pickup/thread/Channel.hpp"

using namespace pickup::thread;

TEST(ChannelTest, SendReceive) {
  Channel<int> ch;
  EXPECT_TRUE(ch.send(42));
  int val = 0;
  EXPECT_TRUE(ch.receive(val));
  EXPECT_EQ(val, 42);
}

TEST(ChannelTest, SendReceiveMove) {
  Channel<std::string> ch;
  std::string msg = "hello";
  EXPECT_TRUE(ch.send(std::move(msg)));
  std::string val;
  EXPECT_TRUE(ch.receive(val));
  EXPECT_EQ(val, "hello");
}

TEST(ChannelTest, SendMultipleReceiveFIFO) {
  Channel<int> ch;
  EXPECT_TRUE(ch.send(1));
  EXPECT_TRUE(ch.send(2));
  EXPECT_TRUE(ch.send(3));
  int val = 0;
  EXPECT_TRUE(ch.receive(val));
  EXPECT_EQ(val, 1);
  EXPECT_TRUE(ch.receive(val));
  EXPECT_EQ(val, 2);
  EXPECT_TRUE(ch.receive(val));
  EXPECT_EQ(val, 3);
}

TEST(ChannelTest, TryReceiveEmpty) {
  Channel<int> ch;
  int val = 0;
  EXPECT_FALSE(ch.tryReceive(val));
}

TEST(ChannelTest, TryReceiveSuccess) {
  Channel<int> ch;
  EXPECT_TRUE(ch.send(99));
  int val = 0;
  EXPECT_TRUE(ch.tryReceive(val));
  EXPECT_EQ(val, 99);
}

TEST(ChannelTest, TryReceiveWithTimeoutEmpty) {
  Channel<int> ch;
  int val = 0;
  EXPECT_FALSE(ch.tryReceive(val, 10));
}

TEST(ChannelTest, TryReceiveWithTimeoutSuccess) {
  Channel<int> ch;
  EXPECT_TRUE(ch.send(77));
  int val = 0;
  EXPECT_TRUE(ch.tryReceive(val, 100));
  EXPECT_EQ(val, 77);
}

TEST(ChannelTest, ClosePreventsSend) {
  Channel<int> ch;
  ch.close();
  EXPECT_FALSE(ch.send(42));
}

TEST(ChannelTest, CloseCausesReceiveToReturnFalse) {
  Channel<int> ch;
  ch.close();
  int val = 0;
  EXPECT_FALSE(ch.receive(val));
}

TEST(ChannelTest, CloseWhileReceivingBlocks) {
  Channel<int> ch;
  std::atomic<bool> received(false);
  std::thread t([&]() {
    int val = 0;
    bool ok = ch.receive(val);
    received.store(ok);
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  ch.close();
  t.join();
  EXPECT_FALSE(received.load());
}

TEST(ChannelTest, Clear) {
  Channel<int> ch;
  EXPECT_TRUE(ch.send(1));
  EXPECT_TRUE(ch.send(2));
  EXPECT_EQ(ch.size(), 2);
  ch.clear();
  EXPECT_EQ(ch.size(), 0);
  EXPECT_TRUE(ch.empty());
}

TEST(ChannelTest, EmptyAndSize) {
  Channel<int> ch;
  EXPECT_TRUE(ch.empty());
  EXPECT_EQ(ch.size(), 0);
  EXPECT_TRUE(ch.send(1));
  EXPECT_FALSE(ch.empty());
  EXPECT_EQ(ch.size(), 1);
  int val = 0;
  EXPECT_TRUE(ch.receive(val));
  EXPECT_TRUE(ch.empty());
  EXPECT_EQ(ch.size(), 0);
}

TEST(ChannelTest, ProducerConsumer) {
  Channel<int> ch;
  constexpr int kCount = 100;
  std::thread producer([&]() {
    for (int i = 0; i < kCount; ++i) {
      EXPECT_TRUE(ch.send(i));
    }
  });
  std::thread consumer([&]() {
    for (int i = 0; i < kCount; ++i) {
      int val = -1;
      EXPECT_TRUE(ch.receive(val));
      EXPECT_EQ(val, i);
    }
  });
  producer.join();
  consumer.join();
}

TEST(ChannelTest, CloseWithDataRemaining) {
  Channel<int> ch;
  EXPECT_TRUE(ch.send(1));
  EXPECT_TRUE(ch.send(2));
  ch.close();
  int val = 0;
  // receive() returns false immediately once closed
  EXPECT_FALSE(ch.receive(val));
}

TEST(ChannelTest, TryReceiveAfterClose) {
  Channel<int> ch;
  ch.close();
  int val = 0;
  EXPECT_FALSE(ch.tryReceive(val));
}

TEST(ChannelTest, TryReceiveWithTimeoutAfterClose) {
  Channel<int> ch;
  ch.close();
  int val = 0;
  EXPECT_FALSE(ch.tryReceive(val, 10));
}
