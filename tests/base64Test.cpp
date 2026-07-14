#include <gtest/gtest.h>
#include <string>

#include "pickup/codec/base64.h"

using namespace pickup::codec;

TEST(Base64Test, EncodeEmpty) {
  EXPECT_EQ(base64::encode(""), "");
}

TEST(Base64Test, DecodeEmpty) {
  EXPECT_EQ(base64::decode(""), "");
}

TEST(Base64Test, RoundtripShort) {
  const std::string input = "Hello";
  auto encoded = base64::encode(input);
  EXPECT_EQ(encoded, "SGVsbG8=");
  EXPECT_EQ(base64::decode(encoded), input);
}

TEST(Base64Test, RoundtripThreeBytes) {
  const std::string input = "Man";
  auto encoded = base64::encode(input);
  EXPECT_EQ(encoded, "TWFu");
  EXPECT_EQ(base64::decode(encoded), input);
}

TEST(Base64Test, RoundtripBinary) {
  const unsigned char raw[] = {0x00, 0xFF, 0xAB, 0xCD, 0x12, 0x34};
  auto encoded = base64::encode(raw, sizeof(raw));
  auto decoded = base64::decode(encoded);
  EXPECT_EQ(decoded.size(), sizeof(raw));
  EXPECT_EQ(memcmp(decoded.data(), raw, sizeof(raw)), 0);
}

TEST(Base64Test, RoundtripLong) {
  std::string input;
  for (int i = 0; i < 256; ++i) input += static_cast<char>(i);
  auto encoded = base64::encode(input);
  EXPECT_EQ(base64::decode(encoded), input);
}

TEST(Base64Test, Padding) {
  EXPECT_EQ(base64::encode("f"), "Zg==");
  EXPECT_EQ(base64::encode("fo"), "Zm8=");
  EXPECT_EQ(base64::encode("foo"), "Zm9v");
  EXPECT_EQ(base64::encode("foob"), "Zm9vYg==");
  EXPECT_EQ(base64::encode("fooba"), "Zm9vYmE=");
  EXPECT_EQ(base64::encode("foobar"), "Zm9vYmFy");
}

TEST(Base64Test, UrlSafe) {
  // 选择编码后包含 '+' 和 '/' 的输入
  std::string std_encoded = base64::encode("\xFF\xFB");
  EXPECT_EQ(std_encoded, "//s=");  // contains '/'
  auto url_safe = base64::toUrlSafe(std_encoded);
  // toUrlSafe replaces '+' with '-' and '/' with '_'
  EXPECT_EQ(url_safe.find('/'), std::string::npos);
  EXPECT_EQ(url_safe, "__s");
  // Roundtrip: fromUrlSafe(toUrlSafe(x)) == x
  auto back = base64::fromUrlSafe(url_safe);
  EXPECT_EQ(back, std_encoded);
}

TEST(Base64Test, RoundtripUrlSafe) {
  const std::string input = "\xFF\xFE\xFD\xFC\xFB\xFA";
  auto encoded = base64::encode(input);
  auto url_safe = base64::toUrlSafe(encoded);
  auto from_url = base64::fromUrlSafe(url_safe);
  EXPECT_EQ(from_url, encoded);
  auto decoded = base64::decode(from_url);
  EXPECT_EQ(decoded, input);
}
