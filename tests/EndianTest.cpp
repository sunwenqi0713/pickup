#include <gtest/gtest.h>
#include <cstdint>

#include "pickup/utils/ByteSwap.hpp"
#include "pickup/utils/Endian.hpp"

using namespace pickup::utils;

// ---- ByteSwap ----

TEST(ByteSwapTest, Swap16) {
  EXPECT_EQ(byteswap16(0x1234), 0x3412);
}

TEST(ByteSwapTest, Swap32) {
  EXPECT_EQ(byteswap32(0x12345678), 0x78563412);
}

TEST(ByteSwapTest, Swap64) {
  EXPECT_EQ(byteswap64(0x0102030405060708ULL), 0x0807060504030201ULL);
}

TEST(ByteSwapTest, GenericSwap16) {
  uint16_t val = 0xABCD;
  EXPECT_EQ(byteswap(val), byteswap16(val));
}

TEST(ByteSwapTest, GenericSwap32) {
  uint32_t val = 0xDEADBEEF;
  EXPECT_EQ(byteswap(val), byteswap32(val));
}

TEST(ByteSwapTest, GenericSwap64) {
  uint64_t val = 0x1234567890ABCDEFULL;
  EXPECT_EQ(byteswap(val), byteswap64(val));
}

TEST(ByteSwapTest, SwapInt8NoChange) {
  int8_t val = 0x42;
  EXPECT_EQ(byteswap(val), val);
}

TEST(ByteSwapTest, SwapUint8NoChange) {
  uint8_t val = 0xAB;
  EXPECT_EQ(byteswap(val), val);
}

TEST(ByteSwapTest, SwapSigned32) {
  int32_t val = -12345678;
  auto swapped = byteswap(val);
  auto back = byteswap(swapped);
  EXPECT_EQ(back, val);
}

TEST(ByteSwapTest, SwapFloat) {
  float val = 3.14f;
  auto swapped = byteswap(val);
  auto back = byteswap(swapped);
  EXPECT_FLOAT_EQ(back, val);
}

TEST(ByteSwapTest, SwapDouble) {
  double val = 2.718281828;
  auto swapped = byteswap(val);
  auto back = byteswap(swapped);
  EXPECT_DOUBLE_EQ(back, val);
}

// ---- Endian ----

TEST(EndianTest, SystemEndianDetected) {
  auto endian = system_endian();
#if defined(_WIN32) || defined(__LITTLE_ENDIAN__)
  EXPECT_EQ(endian, Endian::Little);
#else
  EXPECT_TRUE(endian == Endian::Little || endian == Endian::Big);
#endif
}

TEST(EndianTest, ConvertEndianSameNoOp) {
  uint32_t val = 0x12345678;
  EXPECT_EQ(convert_endian(val, Endian::Little, Endian::Little), val);
}

TEST(EndianTest, ConvertEndianUnknownNoOp) {
  uint32_t val = 0x12345678;
  EXPECT_EQ(convert_endian(val, Endian::Little, Endian::Unknown), val);
}

TEST(EndianTest, ConvertEndianDifferent) {
  uint32_t val = 0x12345678;
  auto converted = convert_endian(val, Endian::Little, Endian::Big);
  EXPECT_EQ(converted, byteswap(val));
}

TEST(EndianTest, HostToNetwork) {
  uint32_t val = 0x12345678;
  auto net = host_to_network(val);
  auto back = network_to_host(net);
  EXPECT_EQ(back, val);
}

TEST(EndianTest, Htons) {
  uint16_t val = 0x1234;
  auto net = htons(val);
  EXPECT_EQ(ntohs(net), val);
}

TEST(EndianTest, Htonl) {
  uint32_t val = 0x12345678;
  EXPECT_EQ(ntohl(htonl(val)), val);
}

TEST(EndianTest, Htonll) {
  uint64_t val = 0x0102030405060708ULL;
  EXPECT_EQ(ntohll(htonll(val)), val);
}

TEST(EndianTest, ByteswapInplace) {
  uint32_t data[] = {0x12345678, 0xABCDEF01};
  byteswap_inplace(data, 2);
  EXPECT_EQ(data[0], byteswap32(0x12345678));
  EXPECT_EQ(data[1], byteswap32(0xABCDEF01));
}

TEST(EndianTest, ByteswapCopy) {
  uint32_t src[] = {0x12345678, 0xABCDEF01};
  uint32_t dst[2] = {};
  byteswap_copy(src, dst, 2);
  EXPECT_EQ(dst[0], byteswap32(src[0]));
  EXPECT_EQ(dst[1], byteswap32(src[1]));
}
