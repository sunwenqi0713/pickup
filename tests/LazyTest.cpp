#include <gtest/gtest.h>
#include <string>

#include "pickup/utils/Lazy.hpp"

using namespace pickup::utils;

TEST(LazyTest, DefaultConstructor) {
  Lazy<int> lazy;
  EXPECT_FALSE(lazy.hasValue());
}

TEST(LazyTest, GetOrCreateCreates) {
  Lazy<int> lazy([]() { return 42; });
  EXPECT_FALSE(lazy.hasValue());
  const int& val = lazy.getOrCreate();
  EXPECT_EQ(val, 42);
  EXPECT_TRUE(lazy.hasValue());
}

TEST(LazyTest, GetOrCreateReturnsCached) {
  int callCount = 0;
  Lazy<int> lazy([&]() {
    ++callCount;
    return 42;
  });
  EXPECT_EQ(lazy.getOrCreate(), 42);
  EXPECT_EQ(lazy.getOrCreate(), 42);
  EXPECT_EQ(lazy.getOrCreate(), 42);
  EXPECT_EQ(callCount, 1);
}

TEST(LazyTest, GetIfCreatedNoValue) {
  Lazy<int> lazy;
  EXPECT_FALSE(lazy.getIfCreated().has_value());
}

TEST(LazyTest, GetIfCreatedHasValue) {
  Lazy<std::string> lazy([]() { return std::string("hello"); });
  lazy.getOrCreate();
  auto opt = lazy.getIfCreated();
  ASSERT_TRUE(opt.has_value());
  EXPECT_EQ(*opt, "hello");
}

TEST(LazyTest, SetFactoryBeforeCreate) {
  Lazy<int> lazy;
  EXPECT_FALSE(lazy.hasValue());
  lazy.setFactory([]() { return 99; });
  EXPECT_EQ(lazy.getOrCreate(), 99);
}

TEST(LazyTest, SetFactoryAfterCreateDoesNothing) {
  int callCount = 0;
  Lazy<int> lazy([&]() {
    ++callCount;
    return 42;
  });
  lazy.getOrCreate();
  lazy.setFactory([]() { return 100; });
  EXPECT_EQ(lazy.getOrCreate(), 42);
  EXPECT_EQ(callCount, 1);
}

TEST(LazyTest, LazyString) {
  Lazy<std::string> lazy([]() { return "expensive"; });
  EXPECT_EQ(lazy.getOrCreate(), "expensive");
}
