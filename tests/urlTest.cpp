#include <gtest/gtest.h>
#include <string>

#include "pickup/codec/url.h"

using namespace pickup::codec;

TEST(UrlTest, EncodeEmpty) {
  EXPECT_EQ(url::encode(""), "");
}

TEST(UrlTest, DecodeEmpty) {
  EXPECT_EQ(url::decode(""), "");
}

TEST(UrlTest, EncodeNormalChars) {
  EXPECT_EQ(url::encode("abc123"), "abc123");
}

TEST(UrlTest, EncodeSpace) {
  EXPECT_EQ(url::encode("a b"), "a+b");
}

TEST(UrlTest, EncodeSpecialChars) {
  std::string encoded = url::encode("a/b?c#d");
  EXPECT_NE(encoded, "a/b?c#d");
  EXPECT_EQ(encoded, "a%2Fb%3Fc%23d");
}

TEST(UrlTest, Roundtrip) {
  std::string input = "hello world!@#$%^&*()_+-=[]{}|;':\",./<>?";
  std::string encoded = url::encode(input);
  std::string decoded = url::decode(encoded);
  EXPECT_EQ(decoded, input);
}

TEST(UrlTest, DecodePlusAsSpace) {
  EXPECT_EQ(url::decode("a+b"), "a b");
}

TEST(UrlTest, DecodePercentEncoded) {
  EXPECT_EQ(url::decode("%20"), " ");
  EXPECT_EQ(url::decode("%41"), "A");
  EXPECT_EQ(url::decode("%7a"), "z");
}

TEST(UrlTest, DecodeInvalidPercent) {
  // hexToValue returns 0 for non-hex chars, so %XX decodes to null char
  std::string expected("\0", 1);
  EXPECT_EQ(url::decode("%XX"), expected);
}

TEST(UrlTest, DecodePartialPercent) {
  // hexToValue('G')=0, hexToValue('1')=1 => 0x01
  std::string expected("\x01", 1);
  EXPECT_EQ(url::decode("%G1"), expected);
}

TEST(UrlTest, CharsetRoundtrip) {
  const std::string inputs[] = {
      "",
      "a",
      "abc",
      "a b",
      "a+b",
      "a%b",
      "a&b=c?d/e#f",
      "hello@world.com",
      "~!@#$%^&*()_+`-=[]{}|;':\",./<>?",
      "1234567890",
  };
  for (const auto& input : inputs) {
    std::string encoded = url::encode(input);
    std::string decoded = url::decode(encoded);
    EXPECT_EQ(decoded, input) << "Failed roundtrip for input: " << input;
  }
}
