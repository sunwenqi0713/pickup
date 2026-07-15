#include <gtest/gtest.h>
#include "pickup/utils/BitOperator.hpp"

using namespace pickup::utils;

TEST(BitOperatorTest, DefaultConstructor) {
  BitOperator<uint32_t> bits;
  uint32_t v = bits.getValue();
  EXPECT_EQ(v, 0);
}

TEST(BitOperatorTest, InitialValueConstructor) {
  BitOperator<uint32_t> bits(0x5A);
  uint32_t v = bits.getValue();
  EXPECT_EQ(v, 0x5A);
}

TEST(BitOperatorTest, SetBit) {
  BitOperator<uint32_t> bits;
  bits.setBit(3);
  uint32_t v = bits.getValue();
  EXPECT_EQ(v, uint32_t(0x00000008));
  bits.setBit(0);
  v = bits.getValue();
  EXPECT_EQ(v, uint32_t(0x00000009));
}

TEST(BitOperatorTest, ClearBit) {
  BitOperator<uint32_t> bits(0x0000000F);
  bits.clearBit(1);
  uint32_t v = bits.getValue();
  EXPECT_EQ(v, uint32_t(0x0000000D));
  bits.clearBit(3);
  v = bits.getValue();
  EXPECT_EQ(v, uint32_t(0x00000005));
}

TEST(BitOperatorTest, ToggleBit) {
  BitOperator<uint32_t> bits(0x00000005);
  bits.toggleBit(1);
  uint32_t v = bits.getValue();
  EXPECT_EQ(v, uint32_t(0x00000007));
  bits.toggleBit(0);
  v = bits.getValue();
  EXPECT_EQ(v, uint32_t(0x00000006));
}

TEST(BitOperatorTest, CheckBit) {
  BitOperator<uint32_t> bits(0x0000000A);
  bool b = bits.checkBit(3);
  EXPECT_TRUE(b);
  b = bits.checkBit(2);
  EXPECT_FALSE(b);
  b = bits.checkBit(1);
  EXPECT_TRUE(b);
  b = bits.checkBit(0);
  EXPECT_FALSE(b);
}

TEST(BitOperatorTest, SetAndGetValue) {
  BitOperator<uint32_t> bits;
  bits.setValue(0x12345678);
  uint32_t v = bits.getValue();
  EXPECT_EQ(v, 0x12345678);
}

TEST(BitOperatorTest, HighestBitOperation) {
  BitOperator<uint32_t> bits;
  const uint32_t highest_bit = 1 << 31;
  bits.setBit(31);
  uint32_t v = bits.getValue();
  EXPECT_EQ(v, highest_bit);
  bool b = bits.checkBit(31);
  EXPECT_TRUE(b);
  bits.clearBit(31);
  v = bits.getValue();
  EXPECT_EQ(v, 0);
}

TEST(BitOperatorTest, CombinedOperations) {
  BitOperator<uint32_t> bits;
  bits.setBit(2);
  bits.toggleBit(5);
  bits.setBit(5);
  bits.clearBit(2);
  uint32_t v = bits.getValue();
  EXPECT_EQ(v, uint32_t(0x00000020));
  bool b = bits.checkBit(2);
  EXPECT_FALSE(b);
  b = bits.checkBit(5);
  EXPECT_TRUE(b);
}

TEST(BitOperatorTest, BoundaryConditions) {
  BitOperator<uint8_t> bits;
  bits.setBit(7);
  uint8_t v = bits.getValue();
  EXPECT_EQ(v, uint8_t(0x80));
  bool b = bits.checkBit(7);
  EXPECT_TRUE(b);
  bits.toggleBit(7);
  v = bits.getValue();
  EXPECT_EQ(v, uint8_t(0));
}

TEST(BitOperatorTest, SignedTypeTest) {
  BitOperator<int> bits;
  bits.setBit(31);
  bool b = bits.checkBit(31);
  EXPECT_TRUE(b);
  int v = bits.getValue();
  EXPECT_LT(v, 0);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
