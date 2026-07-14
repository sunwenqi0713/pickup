#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "pickup/codec/hex.h"

using namespace pickup::codec;

TEST(HexTest, EncodeEmpty) {
  EXPECT_EQ(hex::encode(nullptr, 0), "");
}

TEST(HexTest, EncodeSingleByte) {
  uint8_t data[] = {0xFF};
  EXPECT_EQ(hex::encode(data, 1), "FF");
  EXPECT_EQ(hex::encode(data, 1, false), "ff");
}

TEST(HexTest, EncodeMultipleBytes) {
  uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};
  EXPECT_EQ(hex::encode(data, 4), "DEADBEEF");
  EXPECT_EQ(hex::encode(data, 4, false), "deadbeef");
}

TEST(HexTest, EncodeVector) {
  std::vector<uint8_t> data = {0x01, 0x02, 0xFF};
  EXPECT_EQ(hex::encode(data), "0102FF");
  EXPECT_EQ(hex::encode(data, false), "0102ff");
}

TEST(HexTest, EncodeUint8) {
  EXPECT_EQ(hex::encode(uint8_t{0x0F}), "0F");
  EXPECT_EQ(hex::encode(uint8_t{0xAB}), "AB");
  EXPECT_EQ(hex::encode(uint8_t{0x0F}, false), "0f");
}

TEST(HexTest, EncodeUint16) {
  EXPECT_EQ(hex::encode(uint16_t{0xABCD}), "ABCD");
  EXPECT_EQ(hex::encode(uint16_t{0xABCD}, false), "abcd");
}

TEST(HexTest, EncodeUint32) {
  EXPECT_EQ(hex::encode(uint32_t{0xDEADBEEF}), "DEADBEEF");
  EXPECT_EQ(hex::encode(uint32_t{0xDEADBEEF}, false), "deadbeef");
}

TEST(HexTest, EncodeUint64) {
  EXPECT_EQ(hex::encode(uint64_t{0x0123456789ABCDEF}), "0123456789ABCDEF");
}

TEST(HexTest, EncodeWithSeparator) {
  uint8_t data[] = {0xAB, 0xCD, 0xEF};
  EXPECT_EQ(hex::encodeWithSeparator(data, 3), "AB CD EF");
  EXPECT_EQ(hex::encodeWithSeparator(data, 3, true, ':'), "AB:CD:EF");
  EXPECT_EQ(hex::encodeWithSeparator(data, 3, false), "ab cd ef");
}

TEST(HexTest, EncodeWithSeparatorSingle) {
  uint8_t data[] = {0x12};
  EXPECT_EQ(hex::encodeWithSeparator(data, 1), "12");
}

TEST(HexTest, Decode) {
  auto result = hex::decode("DEADBEEF");
  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->size(), 4);
  EXPECT_EQ((*result)[0], 0xDE);
  EXPECT_EQ((*result)[3], 0xEF);
}

TEST(HexTest, DecodeLowercase) {
  auto result = hex::decode("deadbeef");
  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->size(), 4);
  EXPECT_EQ((*result)[0], 0xDE);
}

TEST(HexTest, DecodeEmpty) {
  auto result = hex::decode("");
  ASSERT_TRUE(result.has_value());
  EXPECT_TRUE(result->empty());
}

TEST(HexTest, DecodeInvalidOddLength) {
  auto result = hex::decode("ABC");
  EXPECT_FALSE(result.has_value());
}

TEST(HexTest, DecodeInvalidChars) {
  auto result = hex::decode("XYZ");
  EXPECT_FALSE(result.has_value());
}

TEST(HexTest, DecodeWithSeparator) {
  auto result = hex::decodeWithSeparator("AB CD EF");
  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->size(), 3);
  EXPECT_EQ((*result)[0], 0xAB);
  EXPECT_EQ((*result)[2], 0xEF);
}

TEST(HexTest, DecodeWithSeparatorCustom) {
  auto result = hex::decodeWithSeparator("AB:CD:EF", ':');
  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result->size(), 3);
}

TEST(HexTest, Roundtrip) {
  std::vector<uint8_t> original = {0x00, 0xFF, 0x12, 0x34, 0xAB, 0xCD};
  auto encoded = hex::encode(original);
  auto decoded = hex::decode(encoded);
  ASSERT_TRUE(decoded.has_value());
  EXPECT_EQ(*decoded, original);
}

TEST(HexTest, RoundtripWithSeparator) {
  std::vector<uint8_t> original = {0x01, 0x23, 0x45, 0x67};
  auto encoded = hex::encodeWithSeparator(original, true, '-');
  auto decoded = hex::decodeWithSeparator(encoded, '-');
  ASSERT_TRUE(decoded.has_value());
  EXPECT_EQ(*decoded, original);
}
