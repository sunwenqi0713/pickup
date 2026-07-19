#include <gtest/gtest.h>

#include "pickup/utils/flags.hpp"

using namespace pickup::utils;

enum class MyFlags {
  None = 0x0,
  One = 0x1,
  Two = 0x2,
  Three = 0x4,
  Four = 0x8,
  All = 0xF,
};

ENUM_FLAGS(MyFlags)

enum class Uint8Flags : uint8_t {
  None = 0,
  A = 0x1,
  B = 0x2,
  C = 0x4,
};

ENUM_FLAGS(Uint8Flags)

TEST(FlagsTest, DefaultConstructor) {
  Flags<MyFlags> flags;
  EXPECT_FALSE(flags);
  EXPECT_EQ(flags.underlying(), 0u);
}

TEST(FlagsTest, ConstructFromEnum) {
  Flags<MyFlags> flags(MyFlags::One);
  EXPECT_TRUE(flags);
  EXPECT_EQ(flags.underlying(), 1u);
}

TEST(FlagsTest, ConstructFromUnderlying) {
  using type = unsigned int;
  Flags<MyFlags> flags(type{5});
  EXPECT_TRUE(flags);
  EXPECT_EQ(flags.underlying(), 5u);
}

TEST(FlagsTest, CopyConstruction) {
  Flags<MyFlags> original(MyFlags::Three);
  Flags<MyFlags> copy(original);
  EXPECT_EQ(copy.underlying(), 4u);
}

TEST(FlagsTest, MoveConstruction) {
  Flags<MyFlags> original(MyFlags::Three);
  Flags<MyFlags> moved(std::move(original));
  EXPECT_EQ(moved.underlying(), 4u);
}

TEST(FlagsTest, CopyAssignment) {
  Flags<MyFlags> original(MyFlags::Two);
  Flags<MyFlags> copy;
  copy = original;
  EXPECT_EQ(copy.underlying(), 2u);
}

TEST(FlagsTest, MoveAssignment) {
  Flags<MyFlags> original(MyFlags::Two);
  Flags<MyFlags> moved;
  moved = std::move(original);
  EXPECT_EQ(moved.underlying(), 2u);
}

TEST(FlagsTest, AssignEnum) {
  Flags<MyFlags> flags;
  flags = MyFlags::Four;
  EXPECT_TRUE(flags);
  EXPECT_EQ(flags.underlying(), 8u);
}

TEST(FlagsTest, AssignUnderlying) {
  Flags<MyFlags> flags;
  flags = (unsigned int)(3);
  EXPECT_TRUE(flags);
  EXPECT_EQ(flags.underlying(), 3u);
}

TEST(FlagsTest, OperatorBool) {
  Flags<MyFlags> empty;
  EXPECT_FALSE(empty);

  Flags<MyFlags> nonEmpty(MyFlags::One);
  EXPECT_TRUE(nonEmpty);
}

TEST(FlagsTest, OperatorNot) {
  Flags<MyFlags> empty;
  EXPECT_TRUE(!empty);

  Flags<MyFlags> nonEmpty(MyFlags::One);
  EXPECT_FALSE(!nonEmpty);
}

TEST(FlagsTest, OperatorBitwiseNot) {
  Flags<MyFlags> flags(MyFlags::None);
  Flags<MyFlags> inverted = ~flags;
  EXPECT_NE(inverted.underlying(), 0u);
}

TEST(FlagsTest, OperatorAndAssign) {
  Flags<MyFlags> flags(MyFlags::All);
  flags &= Flags<MyFlags>(MyFlags::One | MyFlags::Two);
  EXPECT_TRUE(flags.isset(MyFlags::One));
  EXPECT_TRUE(flags.isset(MyFlags::Two));
  EXPECT_FALSE(flags.isset(MyFlags::Three));
}

TEST(FlagsTest, OperatorOrAssign) {
  Flags<MyFlags> flags(MyFlags::One);
  flags |= Flags<MyFlags>(MyFlags::Four);
  EXPECT_TRUE(flags.isset(MyFlags::One));
  EXPECT_TRUE(flags.isset(MyFlags::Four));
}

TEST(FlagsTest, OperatorXorAssign) {
  Flags<MyFlags> flags(MyFlags::One | MyFlags::Two);
  flags ^= Flags<MyFlags>(MyFlags::One);
  EXPECT_FALSE(flags.isset(MyFlags::One));
  EXPECT_TRUE(flags.isset(MyFlags::Two));
}

TEST(FlagsTest, OperatorAnd) {
  auto result = Flags<MyFlags>(MyFlags::All) & Flags<MyFlags>(MyFlags::One | MyFlags::Three);
  EXPECT_TRUE(result.isset(MyFlags::One));
  EXPECT_TRUE(result.isset(MyFlags::Three));
  EXPECT_FALSE(result.isset(MyFlags::Two));
}

TEST(FlagsTest, OperatorOr) {
  auto result = Flags<MyFlags>(MyFlags::One) | Flags<MyFlags>(MyFlags::Four);
  EXPECT_TRUE(result.isset(MyFlags::One));
  EXPECT_TRUE(result.isset(MyFlags::Four));
}

TEST(FlagsTest, OperatorXor) {
  auto result = Flags<MyFlags>(MyFlags::One | MyFlags::Two) ^ Flags<MyFlags>(MyFlags::Two);
  EXPECT_TRUE(result.isset(MyFlags::One));
  EXPECT_FALSE(result.isset(MyFlags::Two));
}

TEST(FlagsTest, OperatorEqual) {
  Flags<MyFlags> a(MyFlags::One | MyFlags::Two);
  Flags<MyFlags> b(MyFlags::One | MyFlags::Two);
  EXPECT_TRUE(a == b);
}

TEST(FlagsTest, OperatorNotEqual) {
  Flags<MyFlags> a(MyFlags::One);
  Flags<MyFlags> b(MyFlags::Two);
  EXPECT_TRUE(a != b);
}

TEST(FlagsTest, ConvertToEnum) {
  Flags<MyFlags> flags(MyFlags::Three);
  MyFlags val = (MyFlags)flags;
  EXPECT_EQ(val, MyFlags::Three);
}

TEST(FlagsTest, IssetNoArgs) {
  Flags<MyFlags> empty;
  EXPECT_FALSE(empty.isset());

  Flags<MyFlags> nonEmpty(MyFlags::One);
  EXPECT_TRUE(nonEmpty.isset());
}

TEST(FlagsTest, IssetWithType) {
  Flags<MyFlags> flags(MyFlags::One | MyFlags::Two);
  EXPECT_TRUE(flags.isset((unsigned int)MyFlags::One));
  EXPECT_TRUE(flags.isset((unsigned int)MyFlags::Two));
  EXPECT_FALSE(flags.isset((unsigned int)MyFlags::Four));
}

TEST(FlagsTest, IssetWithEnum) {
  Flags<MyFlags> flags(MyFlags::Two | MyFlags::Three);
  EXPECT_TRUE(flags.isset(MyFlags::Two));
  EXPECT_TRUE(flags.isset(MyFlags::Three));
  EXPECT_FALSE(flags.isset(MyFlags::One));
}

TEST(FlagsTest, Value) {
  Flags<MyFlags> flags(MyFlags::Two | MyFlags::Four);
  MyFlags val = flags.value();
  EXPECT_EQ(static_cast<int>(val), static_cast<int>(MyFlags::Two) | static_cast<int>(MyFlags::Four));
}

TEST(FlagsTest, Underlying) {
  Flags<MyFlags> flags(MyFlags::One | MyFlags::Three);
  EXPECT_EQ(flags.underlying(), 5u);
}

TEST(FlagsTest, Bitset) {
  Flags<MyFlags> flags(MyFlags::One | MyFlags::Three);
  auto bs = flags.bitset();
  EXPECT_TRUE(bs.test(0));
  EXPECT_FALSE(bs.test(1));
  EXPECT_TRUE(bs.test(2));
}

TEST(FlagsTest, MemberSwap) {
  Flags<MyFlags> a(MyFlags::One);
  Flags<MyFlags> b(MyFlags::Two);
  a.swap(b);
  EXPECT_EQ(a.underlying(), 2u);
  EXPECT_EQ(b.underlying(), 1u);
}

TEST(FlagsTest, FreeSwap) {
  Flags<MyFlags> a(MyFlags::One);
  Flags<MyFlags> b(MyFlags::Two);
  swap(a, b);
  EXPECT_EQ(a.underlying(), 2u);
  EXPECT_EQ(b.underlying(), 1u);
}

TEST(FlagsTest, EnumOperatorOr) {
  auto mask = MyFlags::One | MyFlags::Three;
  EXPECT_TRUE(mask.isset(MyFlags::One));
  EXPECT_TRUE(mask.isset(MyFlags::Three));
  EXPECT_FALSE(mask.isset(MyFlags::Two));
}

TEST(FlagsTest, EnumOperatorAnd) {
  auto mask = MyFlags::All & MyFlags::One;
  EXPECT_TRUE(mask.isset(MyFlags::One));
  EXPECT_FALSE(mask.isset(MyFlags::Two));
}

TEST(FlagsTest, EnumOperatorXor) {
  auto mask = MyFlags::All ^ MyFlags::One;
  EXPECT_FALSE(mask.isset(MyFlags::One));
  EXPECT_TRUE(mask.isset(MyFlags::Two));
}

TEST(FlagsTest, FlagsChain) {
  auto mask = MyFlags::One | MyFlags::Two;
  mask &= MyFlags::All;
  EXPECT_TRUE(mask.isset(MyFlags::One));
  EXPECT_TRUE(mask.isset(MyFlags::Two));
}

TEST(FlagsTest, Uint8Flags) {
  Flags<Uint8Flags> flags(Uint8Flags::A | Uint8Flags::C);
  EXPECT_EQ(flags.underlying(), static_cast<uint8_t>(5));
  EXPECT_TRUE(flags.isset(Uint8Flags::A));
  EXPECT_FALSE(flags.isset(Uint8Flags::B));
  EXPECT_TRUE(flags.isset(Uint8Flags::C));
}
