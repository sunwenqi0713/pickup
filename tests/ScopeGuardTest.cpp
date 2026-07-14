#include <gtest/gtest.h>

#include "pickup/utils/ScopeGuard.hpp"

using namespace pickup::utils;

TEST(ScopeGuardTest, ExecuteOnDestruction) {
  int flag = 0;
  {
    auto guard = makeGuard([&]() { flag = 42; });
    EXPECT_EQ(flag, 0);
  }
  EXPECT_EQ(flag, 42);
}

TEST(ScopeGuardTest, ReleasePreventsExecution) {
  int flag = 0;
  {
    auto guard = makeGuard([&]() { flag = 42; });
    guard.release();
  }
  EXPECT_EQ(flag, 0);
}

TEST(ScopeGuardTest, MoveSemantics) {
  int flag = 0;
  {
    auto guard1 = makeGuard([&]() { flag = 42; });
    auto guard2 = std::move(guard1);
    EXPECT_EQ(flag, 0);
  }
  EXPECT_EQ(flag, 42);
}

TEST(ScopeGuardTest, MoveRelease) {
  int flag = 0;
  {
    auto guard1 = makeGuard([&]() { flag = 42; });
    auto guard2 = std::move(guard1);
    guard2.release();
  }
  EXPECT_EQ(flag, 0);
}

TEST(ScopeGuardTest, MultipleScopeGuards) {
  int a = 0, b = 0;
  {
    auto g1 = makeGuard([&]() { a = 1; });
    auto g2 = makeGuard([&]() { b = 2; });
  }
  EXPECT_EQ(a, 1);
  EXPECT_EQ(b, 2);
}

TEST(ScopeGuardTest, ScopeGuardMacro) {
  int flag = 0;
  {
    SCOPE_GUARD([&]() { flag = 99; });
    EXPECT_EQ(flag, 0);
  }
  EXPECT_EQ(flag, 99);
}

TEST(ScopeGuardTest, NamedScopeGuardMacro) {
  int flag = 0;
  {
    NAMED_SCOPE_GUARD(guard, [&]() { flag = 88; });
    EXPECT_EQ(flag, 0);
    guard.release();
  }
  EXPECT_EQ(flag, 0);
}

TEST(ScopeGuardTest, ExecuteOnExceptionPath) {
  int flag = 0;
  try {
    auto guard = makeGuard([&]() { flag = 77; });
    throw std::runtime_error("test");
  } catch (...) {
  }
  EXPECT_EQ(flag, 77);
}
