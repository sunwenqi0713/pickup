#include <gtest/gtest.h>
#include <optional>
#include <string>

#include "pickup/utils/LexicalCast.hpp"

using namespace pickup::utils;

TEST(LexicalCastTest, StringToInt) {
  auto r = lexicalCast<int>(std::string_view("123"));
  ASSERT_TRUE(r.has_value());
  EXPECT_EQ(*r, 123);
}

TEST(LexicalCastTest, StringToNegativeInt) {
  auto r = lexicalCast<int>(std::string_view("-42"));
  ASSERT_TRUE(r.has_value());
  EXPECT_EQ(*r, -42);
}

TEST(LexicalCastTest, StringToUnsigned) {
  auto r = lexicalCast<unsigned>(std::string_view("255"));
  ASSERT_TRUE(r.has_value());
  EXPECT_EQ(*r, 255u);
}

TEST(LexicalCastTest, StringToLong) {
  auto r = lexicalCast<long>(std::string_view("1000000"));
  ASSERT_TRUE(r.has_value());
  EXPECT_EQ(*r, 1000000L);
}

TEST(LexicalCastTest, StringToFloat) {
  auto r = lexicalCast<float>(std::string_view("3.14"));
  ASSERT_TRUE(r.has_value());
  EXPECT_FLOAT_EQ(*r, 3.14f);
}

TEST(LexicalCastTest, StringToDouble) {
  auto r = lexicalCast<double>(std::string_view("2.71828"));
  ASSERT_TRUE(r.has_value());
  EXPECT_DOUBLE_EQ(*r, 2.71828);
}

TEST(LexicalCastTest, StringToBoolTrue) {
  auto r = lexicalCast<bool>(std::string_view("true"));
  ASSERT_TRUE(r.has_value());
  EXPECT_TRUE(*r);
}

TEST(LexicalCastTest, StringToBoolFalse) {
  auto r = lexicalCast<bool>(std::string_view("false"));
  ASSERT_TRUE(r.has_value());
  EXPECT_FALSE(*r);
}

TEST(LexicalCastTest, StringToBoolCaseInsensitive) {
  EXPECT_TRUE(*lexicalCast<bool>(std::string_view("True")));
  EXPECT_TRUE(*lexicalCast<bool>(std::string_view("TRUE")));
  EXPECT_FALSE(*lexicalCast<bool>(std::string_view("False")));
  EXPECT_FALSE(*lexicalCast<bool>(std::string_view("FALSE")));
}

TEST(LexicalCastTest, StringToBoolInvalid) {
  EXPECT_FALSE(lexicalCast<bool>(std::string_view("yes")).has_value());
  EXPECT_FALSE(lexicalCast<bool>(std::string_view("no")).has_value());
  EXPECT_FALSE(lexicalCast<bool>(std::string_view("1")).has_value());
}

TEST(LexicalCastTest, InvalidStringToInt) {
  EXPECT_FALSE(lexicalCast<int>(std::string_view("abc")).has_value());
}

TEST(LexicalCastTest, EmptyStringToInt) {
  EXPECT_FALSE(lexicalCast<int>(std::string_view("")).has_value());
}

TEST(LexicalCastTest, IntToString) {
  auto r = lexicalCast<std::string>(42);
  ASSERT_TRUE(r.has_value());
  EXPECT_EQ(*r, "42");
}

TEST(LexicalCastTest, FloatToString) {
  auto r = lexicalCast<std::string>(3.14f);
  ASSERT_TRUE(r.has_value());
  EXPECT_FALSE(r->empty());
}

TEST(LexicalCastTest, BoolToString) {
  auto r = lexicalCast<std::string>(true);
  ASSERT_TRUE(r.has_value());
  EXPECT_EQ(*r, "1");
}

TEST(LexicalCastTest, IntToBool) {
  EXPECT_TRUE(*lexicalCast<bool>(1));
  EXPECT_TRUE(*lexicalCast<bool>(-1));
  EXPECT_FALSE(*lexicalCast<bool>(0));
}

TEST(LexicalCastTest, StringViewToInt) {
  std::string_view sv = "789";
  auto r = lexicalCast<int>(sv);
  ASSERT_TRUE(r.has_value());
  EXPECT_EQ(*r, 789);
}

TEST(LexicalCastTest, StringToString) {
  auto r = lexicalCast<std::string>(std::string("hello"));
  ASSERT_TRUE(r.has_value());
  EXPECT_EQ(*r, "hello");
}

TEST(LexicalCastTest, StringViewToString) {
  std::string_view sv = "world";
  auto r = lexicalCast<std::string>(sv);
  ASSERT_TRUE(r.has_value());
  EXPECT_EQ(*r, "world");
}

TEST(LexicalCastTest, SameType) {
  auto r = lexicalCast<int>(100);
  ASSERT_TRUE(r.has_value());
  EXPECT_EQ(*r, 100);
}
