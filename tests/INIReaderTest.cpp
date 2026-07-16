#include <gtest/gtest.h>
#include <sstream>
#include <string>

#include "pickup/config/INIReader.h"

using namespace pickup::config;

TEST(INIReaderTest, FileNotFound) {
  INIReader reader("/nonexistent/file.ini");
  auto err = reader.error();
  ASSERT_TRUE(err.has_value());
  EXPECT_EQ(err->kind, INIReader::ParseError::Kind::FileOpen);
}

TEST(INIReaderTest, EmptyStream) {
  std::istringstream ss("");
  INIReader reader(ss);
  EXPECT_FALSE(reader.error().has_value());
  EXPECT_TRUE(reader.sections().empty());
}

TEST(INIReaderTest, SimpleKeyValue) {
  std::istringstream ss("name=hello");
  INIReader reader(ss);
  EXPECT_FALSE(reader.error().has_value());
  EXPECT_EQ(reader.get("", "name", ""), "hello");
}

TEST(INIReaderTest, ColonSeparator) {
  std::istringstream ss("name:hello");
  INIReader reader(ss);
  EXPECT_FALSE(reader.error().has_value());
  EXPECT_EQ(reader.get("", "name", ""), "hello");
}

TEST(INIReaderTest, SectionHeader) {
  std::istringstream ss("[section1]\nkey=value");
  INIReader reader(ss);
  EXPECT_FALSE(reader.error().has_value());
  EXPECT_TRUE(reader.hasSection("section1"));
  EXPECT_EQ(reader.get("section1", "key", ""), "value");
}

TEST(INIReaderTest, MultipleSections) {
  std::istringstream ss(
      "[db]\nhost=localhost\nport=5432\n\n[cache]\nttl=3600\n");
  INIReader reader(ss);
  EXPECT_FALSE(reader.error().has_value());
  EXPECT_TRUE(reader.hasSection("db"));
  EXPECT_TRUE(reader.hasSection("cache"));
  EXPECT_EQ(reader.get("db", "host", ""), "localhost");
  EXPECT_EQ(reader.get("db", "port", ""), "5432");
  EXPECT_EQ(reader.get("cache", "ttl", ""), "3600");
}

TEST(INIReaderTest, WhitespaceTrimming) {
  std::istringstream ss("  key =  value  ");
  INIReader reader(ss);
  EXPECT_FALSE(reader.error().has_value());
  EXPECT_EQ(reader.get("", "key", ""), "value");
}

TEST(INIReaderTest, BlankLinesIgnored) {
  std::istringstream ss("\n\n\nkey=val\n\n\n");
  INIReader reader(ss);
  EXPECT_FALSE(reader.error().has_value());
  EXPECT_EQ(reader.get("", "key", ""), "val");
}

TEST(INIReaderTest, SemicolonComment) {
  std::istringstream ss("; this is a comment\nkey=val");
  INIReader reader(ss);
  EXPECT_FALSE(reader.error().has_value());
  EXPECT_EQ(reader.get("", "key", ""), "val");
}

TEST(INIReaderTest, HashComment) {
  std::istringstream ss("# this is a comment\nkey=val");
  INIReader reader(ss);
  EXPECT_FALSE(reader.error().has_value());
  EXPECT_EQ(reader.get("", "key", ""), "val");
}

TEST(INIReaderTest, InlineComment) {
  std::istringstream ss("key=val ; trailing comment");
  INIReader reader(ss);
  EXPECT_FALSE(reader.error().has_value());
  EXPECT_EQ(reader.get("", "key", ""), "val");
}

TEST(INIReaderTest, CaseInsensitiveKey) {
  std::istringstream ss("[Section]\nKEY=value");
  INIReader reader(ss);
  EXPECT_FALSE(reader.error().has_value());
  EXPECT_TRUE(reader.hasValue("section", "key"));
  EXPECT_TRUE(reader.hasValue("SECTION", "KEY"));
  EXPECT_TRUE(reader.hasValue("Section", "Key"));
  EXPECT_EQ(reader.get("section", "key", ""), "value");
}

TEST(INIReaderTest, MultilineValue) {
  std::istringstream ss("key=line1\n  line2\n  line3");
  INIReader reader(ss);
  EXPECT_FALSE(reader.error().has_value());
  EXPECT_EQ(reader.get("", "key", ""), "line1\nline2\nline3");
}

TEST(INIReaderTest, IntegerDecimal) {
  std::istringstream ss("val=1234");
  INIReader reader(ss);
  EXPECT_EQ(reader.getInteger("", "val", 0), 1234);
}

TEST(INIReaderTest, IntegerHex) {
  std::istringstream ss("val=0x4d2");
  INIReader reader(ss);
  EXPECT_EQ(reader.getInteger("", "val", 0), 1234);
}

TEST(INIReaderTest, IntegerNegative) {
  std::istringstream ss("val=-42");
  INIReader reader(ss);
  EXPECT_EQ(reader.getInteger("", "val", 0), -42);
}

TEST(INIReaderTest, IntegerInvalidReturnsDefault) {
  std::istringstream ss("val=notanumber");
  INIReader reader(ss);
  EXPECT_EQ(reader.getInteger("", "val", 99), 99);
}

TEST(INIReaderTest, IntegerEmptyReturnsDefault) {
  std::istringstream ss("val=");
  INIReader reader(ss);
  EXPECT_EQ(reader.getInteger("", "val", 42), 42);
}

TEST(INIReaderTest, RealDouble) {
  std::istringstream ss("val=3.14");
  INIReader reader(ss);
  EXPECT_DOUBLE_EQ(reader.getReal("", "val", 0.0), 3.14);
}

TEST(INIReaderTest, RealInvalidReturnsDefault) {
  std::istringstream ss("val=abc");
  INIReader reader(ss);
  EXPECT_DOUBLE_EQ(reader.getReal("", "val", 1.5), 1.5);
}

TEST(INIReaderTest, Float) {
  std::istringstream ss("val=2.5");
  INIReader reader(ss);
  EXPECT_FLOAT_EQ(reader.getFloat("", "val", 0.0f), 2.5f);
}

TEST(INIReaderTest, BooleanTrue) {
  std::istringstream ss("v=true\nw=yes\nx=on\ny=1\n");
  INIReader reader(ss);
  EXPECT_TRUE(reader.getBoolean("", "v", false));
  EXPECT_TRUE(reader.getBoolean("", "w", false));
  EXPECT_TRUE(reader.getBoolean("", "x", false));
  EXPECT_TRUE(reader.getBoolean("", "y", false));
}

TEST(INIReaderTest, BooleanFalse) {
  std::istringstream ss("v=false\nw=no\nx=off\ny=0\n");
  INIReader reader(ss);
  EXPECT_FALSE(reader.getBoolean("", "v", true));
  EXPECT_FALSE(reader.getBoolean("", "w", true));
  EXPECT_FALSE(reader.getBoolean("", "x", true));
  EXPECT_FALSE(reader.getBoolean("", "y", true));
}

TEST(INIReaderTest, BooleanDefault) {
  std::istringstream ss("key=unknown");
  INIReader reader(ss);
  EXPECT_TRUE(reader.getBoolean("", "key", true));
  EXPECT_FALSE(reader.getBoolean("", "key", false));
}

TEST(INIReaderTest, MissingKeyReturnsDefault) {
  std::istringstream ss("other=val");
  INIReader reader(ss);
  EXPECT_EQ(reader.get("", "nonexistent", "default"), "default");
  EXPECT_EQ(reader.getInteger("", "nonexistent", 42), 42);
  EXPECT_DOUBLE_EQ(reader.getReal("", "nonexistent", 1.5), 1.5);
}

TEST(INIReaderTest, HasValue) {
  std::istringstream ss("key=val");
  INIReader reader(ss);
  EXPECT_TRUE(reader.hasValue("", "key"));
  EXPECT_FALSE(reader.hasValue("", "missing"));
}

TEST(INIReaderTest, HasSection) {
  std::istringstream ss("[sec1]\nk=v\n[sec2]\nk=v\n");
  INIReader reader(ss);
  EXPECT_TRUE(reader.hasSection("sec1"));
  EXPECT_TRUE(reader.hasSection("sec2"));
  EXPECT_FALSE(reader.hasSection("sec3"));
}

TEST(INIReaderTest, Sections) {
  std::istringstream ss("[sec1]\nk=v\n[sec2]\nk=v\n");
  INIReader reader(ss);
  auto secs = reader.sections();
  EXPECT_EQ(secs.size(), 2);
  EXPECT_TRUE(secs.count("sec1") > 0);
  EXPECT_TRUE(secs.count("sec2") > 0);
}

TEST(INIReaderTest, DuplicateKeyConcatenates) {
  std::istringstream ss("key=first\nkey=second");
  INIReader reader(ss);
  EXPECT_EQ(reader.get("", "key", ""), "first\nsecond");
}

TEST(INIReaderTest, UTF8BOM) {
  // BOM: EF BB BF
  std::string bom = "\xEF\xBB\xBF";
  std::istringstream ss(bom + "key=value");
  INIReader reader(ss);
  EXPECT_FALSE(reader.error().has_value());
  EXPECT_EQ(reader.get("", "key", ""), "value");
}

TEST(INIReaderTest, SyntaxErrorMissingBracket) {
  std::istringstream ss("[section\nkey=val");
  INIReader reader(ss);
  auto err = reader.error();
  ASSERT_TRUE(err.has_value());
  EXPECT_EQ(err->kind, INIReader::ParseError::Kind::Syntax);
  EXPECT_EQ(err->line, 1);
}

TEST(INIReaderTest, SyntaxErrorMalformedLine) {
  std::istringstream ss("no_separator_here\n");
  INIReader reader(ss);
  auto err = reader.error();
  ASSERT_TRUE(err.has_value());
  EXPECT_EQ(err->kind, INIReader::ParseError::Kind::Syntax);
}

TEST(INIReaderTest, ValueContainsEquals) {
  std::istringstream ss("key=foo=bar");
  INIReader reader(ss);
  EXPECT_FALSE(reader.error().has_value());
  EXPECT_EQ(reader.get("", "key", ""), "foo=bar");
}

TEST(INIReaderTest, EmptyValue) {
  std::istringstream ss("key=");
  INIReader reader(ss);
  EXPECT_EQ(reader.get("", "key", "default"), "");
}

TEST(INIReaderTest, GlobalKeysBeforeFirstSection) {
  std::istringstream ss("global=val\n[sec]\nk=v\n");
  INIReader reader(ss);
  EXPECT_TRUE(reader.hasValue("", "global"));
  EXPECT_EQ(reader.get("", "global", ""), "val");
  EXPECT_TRUE(reader.hasValue("sec", "k"));
  EXPECT_EQ(reader.get("sec", "k", ""), "v");
}

TEST(INIReaderTest, CRLFLineEndings) {
  std::istringstream ss("key=val\r\n[sec]\r\nk=v\r\n");
  INIReader reader(ss);
  EXPECT_FALSE(reader.error().has_value());
  EXPECT_EQ(reader.get("", "key", ""), "val");
  EXPECT_EQ(reader.get("sec", "k", ""), "v");
}

TEST(INIReaderTest, GetBooleanCaseInsensitive) {
  std::istringstream ss("a=True\nb=YES\nc=On\n");
  INIReader reader(ss);
  EXPECT_TRUE(reader.getBoolean("", "a", false));
  EXPECT_TRUE(reader.getBoolean("", "b", false));
  EXPECT_TRUE(reader.getBoolean("", "c", false));
}
