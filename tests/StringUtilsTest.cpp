#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "pickup/utils/StringUtils.h"

using namespace pickup::utils;

TEST(StringUtilsTest, ToUpper) {
  EXPECT_EQ(toUpper("abc"), "ABC");
  EXPECT_EQ(toUpper("ABC"), "ABC");
  EXPECT_EQ(toUpper("aBc123"), "ABC123");
  EXPECT_EQ(toUpper(""), "");
}

TEST(StringUtilsTest, ToLower) {
  EXPECT_EQ(toLower("ABC"), "abc");
  EXPECT_EQ(toLower("abc"), "abc");
  EXPECT_EQ(toLower("AbC123"), "abc123");
  EXPECT_EQ(toLower(""), "");
}

TEST(StringUtilsTest, IsBlank) {
  EXPECT_TRUE(isBlank(""));
  EXPECT_TRUE(isBlank(" "));
  EXPECT_TRUE(isBlank("  \t\n\r"));
  EXPECT_FALSE(isBlank("a"));
  EXPECT_FALSE(isBlank(" a "));
}

TEST(StringUtilsTest, Contains) {
  EXPECT_TRUE(contains("hello world", "world"));
  EXPECT_TRUE(contains("hello world", "hello"));
  EXPECT_FALSE(contains("hello world", "xyz"));
  EXPECT_FALSE(contains("", "x"));
  EXPECT_TRUE(contains("abc", ""));
}

TEST(StringUtilsTest, StartsWithChar) {
  EXPECT_TRUE(startsWith("abc", 'a'));
  EXPECT_FALSE(startsWith("abc", 'b'));
  EXPECT_FALSE(startsWith("", 'a'));
}

TEST(StringUtilsTest, StartsWithString) {
  EXPECT_TRUE(startsWith("hello world", "hello"));
  EXPECT_FALSE(startsWith("hello world", "world"));
  EXPECT_TRUE(startsWith("abc", ""));
  EXPECT_FALSE(startsWith("", "abc"));
}

TEST(StringUtilsTest, EndsWithChar) {
  EXPECT_TRUE(endsWith("abc", 'c'));
  EXPECT_FALSE(endsWith("abc", 'a'));
  EXPECT_FALSE(endsWith("", 'c'));
}

TEST(StringUtilsTest, EndsWithString) {
  EXPECT_TRUE(endsWith("hello world", "world"));
  EXPECT_FALSE(endsWith("hello world", "hello"));
  EXPECT_TRUE(endsWith("abc", ""));
}

TEST(StringUtilsTest, CompareNoCaseChar) {
  EXPECT_TRUE(compareNoCase('a', 'A'));
  EXPECT_TRUE(compareNoCase('Z', 'z'));
  EXPECT_FALSE(compareNoCase('a', 'b'));
}

TEST(StringUtilsTest, CompareNoCaseString) {
  EXPECT_TRUE(compareNoCase("hello", "HELLO"));
  EXPECT_TRUE(compareNoCase("ABC", "abc"));
  EXPECT_FALSE(compareNoCase("hello", "world"));
}

TEST(StringUtilsTest, TrimLeftChar) {
  std::string s = "  hello";
  trimLeft(s);
  EXPECT_EQ(s, "hello");
  trimLeft(s, 'h');
  EXPECT_EQ(s, "ello");
}

TEST(StringUtilsTest, TrimRightChar) {
  std::string s = "hello  ";
  trimRight(s);
  EXPECT_EQ(s, "hello");
  trimRight(s, 'o');
  EXPECT_EQ(s, "hell");
}

TEST(StringUtilsTest, TrimChar) {
  std::string s = "  hello  ";
  trim(s);
  EXPECT_EQ(s, "hello");
}

TEST(StringUtilsTest, TrimLeftString) {
  std::string s = "\t\n hello";
  trimLeft(s, " \t\n");
  EXPECT_EQ(s, "hello");
}

TEST(StringUtilsTest, TrimRightString) {
  std::string s = "hello \t\n";
  trimRight(s, " \t\n");
  EXPECT_EQ(s, "hello");
}

TEST(StringUtilsTest, TrimString) {
  std::string s = " \t\n hello \t\n ";
  trim(s, " \t\n");
  EXPECT_EQ(s, "hello");
}

TEST(StringUtilsTest, ReplaceAll) {
  EXPECT_EQ(replaceAll("hello world", "world", "there"), "hello there");
  EXPECT_EQ(replaceAll("aaa", "a", "bb"), "bbbbbb");
  EXPECT_EQ(replaceAll("abc", "d", "x"), "abc");
  EXPECT_EQ(replaceAll("", "a", "b"), "");
  EXPECT_EQ(replaceAll("test", "", "x"), "test");
}

TEST(StringUtilsTest, StripPrefix) {
  EXPECT_EQ(stripPrefix("hello world", "hello "), "world");
  EXPECT_EQ(stripPrefix("hello world", "world"), "hello world");
  EXPECT_EQ(stripPrefix("abc", ""), "abc");
}

TEST(StringUtilsTest, StripSuffix) {
  EXPECT_EQ(stripSuffix("hello.cpp", ".cpp"), "hello");
  EXPECT_EQ(stripSuffix("hello.cpp", ".h"), "hello.cpp");
}

TEST(StringUtilsTest, PadLeft) {
  EXPECT_EQ(padLeft("abc", 5), "  abc");
  EXPECT_EQ(padLeft("abc", 5, '*'), "**abc");
  EXPECT_EQ(padLeft("abc", 3), "abc");
  EXPECT_EQ(padLeft("abc", 2), "abc");
  EXPECT_EQ(padLeft("", 3), "   ");
}

TEST(StringUtilsTest, PadRight) {
  EXPECT_EQ(padRight("abc", 5), "abc  ");
  EXPECT_EQ(padRight("abc", 5, '*'), "abc**");
  EXPECT_EQ(padRight("abc", 3), "abc");
}

TEST(StringUtilsTest, Repeat) {
  EXPECT_EQ(repeat("ab", 3), "ababab");
  EXPECT_EQ(repeat("x", 0), "");
  EXPECT_EQ(repeat("", 5), "");
}

TEST(StringUtilsTest, Split) {
  auto parts = split("a::b::c", "::");
  ASSERT_EQ(parts.size(), 3);
  EXPECT_EQ(parts[0], "a");
  EXPECT_EQ(parts[1], "b");
  EXPECT_EQ(parts[2], "c");
}

TEST(StringUtilsTest, SplitEmptyDelimiter) {
  auto parts = split("ab", "");
  ASSERT_EQ(parts.size(), 2);
  EXPECT_EQ(parts[0], "a");
  EXPECT_EQ(parts[1], "b");
}

TEST(StringUtilsTest, SplitNoMatch) {
  auto parts = split("abc", ",");
  ASSERT_EQ(parts.size(), 1);
  EXPECT_EQ(parts[0], "abc");
}

TEST(StringUtilsTest, SplitAnyOf) {
  auto parts = splitAnyOf("a,b;c", ",;");
  ASSERT_EQ(parts.size(), 3);
  EXPECT_EQ(parts[0], "a");
  EXPECT_EQ(parts[1], "b");
  EXPECT_EQ(parts[2], "c");
}

TEST(StringUtilsTest, SplitAnyOfNoDelimiters) {
  auto parts = splitAnyOf("abc", "");
  ASSERT_EQ(parts.size(), 1);
  EXPECT_EQ(parts[0], "abc");
}

TEST(StringUtilsTest, JoinChar) {
  std::vector<std::string> parts = {"a", "b", "c"};
  EXPECT_EQ(join(',', parts), "a,b,c");
}

TEST(StringUtilsTest, JoinEmpty) {
  std::vector<std::string> parts;
  EXPECT_EQ(join(',', parts), "");
}

TEST(StringUtilsTest, JoinSingle) {
  std::vector<std::string> parts = {"only"};
  EXPECT_EQ(join(',', parts), "only");
}

TEST(StringUtilsTest, JoinString) {
  std::vector<std::string> parts = {"a", "b", "c"};
  EXPECT_EQ(join(" -> ", parts), "a -> b -> c");
}
