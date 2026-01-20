#include <gtest/gtest.h>
#include "pickup/utils/BitOperator.hpp"

using namespace pickup::utils;

TEST(BitOperatorTest, DefaultConstructor) {
  BitOperator<uint32_t> bits;
  EXPECT_EQ(bits.getValue(), 0);
}

TEST(BitOperatorTest, InitialValueConstructor) {
  BitOperator<uint32_t> bits(0x5A);
  EXPECT_EQ(bits.getValue(), 0x5A);
}

TEST(BitOperatorTest, SetBit) {
  BitOperator<uint32_t> bits;
  
  bits.setBit(3);
  EXPECT_EQ(bits.getValue(), 0x00000008);
  
  bits.setBit(0);
  EXPECT_EQ(bits.getValue(), 0x00000009);
}

TEST(BitOperatorTest, ClearBit) {
  BitOperator<uint32_t> bits(0x0000000F);
  
  bits.clearBit(1);
  EXPECT_EQ(bits.getValue(), 0x0000000D);
  
  bits.clearBit(3);
  EXPECT_EQ(bits.getValue(), 0x00000005);
}

TEST(BitOperatorTest, ToggleBit) {
  BitOperator<uint32_t> bits(0x00000005);
  
  bits.toggleBit(1);
  EXPECT_EQ(bits.getValue(), 0x00000007);
  
  bits.toggleBit(0);
  EXPECT_EQ(bits.getValue(), 0x00000006);
}

TEST(BitOperatorTest, CheckBit) {
  BitOperator<uint32_t> bits(0x0000000A);  // 二进制 1010
  
  EXPECT_TRUE(bits.checkBit(3));
  EXPECT_FALSE(bits.checkBit(2));
  EXPECT_TRUE(bits.checkBit(1));
  EXPECT_FALSE(bits.checkBit(0));
}

TEST(BitOperatorTest, SetAndGetValue) {
  BitOperator<uint32_t> bits;
  bits.setValue(0x12345678);
  EXPECT_EQ(bits.getValue(), 0x12345678);
}

TEST(BitOperatorTest, HighestBitOperation) {
  BitOperator<uint32_t> bits;
  const uint32_t highest_bit = 1 << 31;
  
  bits.setBit(31);
  EXPECT_EQ(bits.getValue(), highest_bit);
  EXPECT_TRUE(bits.checkBit(31));
  
  bits.clearBit(31);
  EXPECT_EQ(bits.getValue(), 0);
}

TEST(BitOperatorTest, CombinedOperations) {
  BitOperator<uint32_t> bits;
  
  bits.setBit(2);
  bits.toggleBit(5);
  bits.setBit(5);       // 重复设置应不影响结果
  bits.clearBit(2);
  
  EXPECT_EQ(bits.getValue(), 0x00000020);
  EXPECT_FALSE(bits.checkBit(2));
  EXPECT_TRUE(bits.checkBit(5));
}

TEST(BitOperatorTest, BoundaryConditions) {
  BitOperator<uint8_t> bits;  // 测试8位类型
  
  bits.setBit(7);  // 设置最高位
  EXPECT_EQ(bits.getValue(), 0x80);
  EXPECT_TRUE(bits.checkBit(7));
  
  bits.toggleBit(7);
  EXPECT_EQ(bits.getValue(), 0);
}

TEST(BitOperatorTest, SignedTypeTest) {
  BitOperator<int> bits;  // 测试有符号类型
  
  bits.setBit(31);  // 在32位int上设置符号位
  EXPECT_TRUE(bits.checkBit(31));
  EXPECT_LT(bits.getValue(), 0);  // 验证结果确实为负数
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
