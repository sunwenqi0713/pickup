#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "pickup/utils/Factory.hpp"

using namespace pickup::utils;

namespace {

struct ProductBase {
  virtual ~ProductBase() = default;
  virtual std::string type() const = 0;
};

struct ProductA : ProductBase {
  std::string type() const override { return "A"; }
};

struct ProductB : ProductBase {
  std::string type() const override { return "B"; }
};

struct ProductWithParam : ProductBase {
  explicit ProductWithParam(int v) : value(v) {}
  std::string type() const override { return "Param:" + std::to_string(value); }
  int value = 0;
};

struct ProductWithMultiParam : ProductBase {
  ProductWithMultiParam(int a, std::string b) : sum(a), label(b) {}
  std::string type() const override {
    return label + ":" + std::to_string(sum);
  }
  int sum = 0;
  std::string label;
};

}  // namespace

TEST(FactoryTest, CreateRegisteredType) {
  Factory<std::string, ProductBase> factory;
  factory.add("A", []() { return std::make_unique<ProductA>(); });
  auto obj = factory.create("A");
  ASSERT_NE(obj, nullptr);
  EXPECT_EQ(obj->type(), "A");
}

TEST(FactoryTest, CreateNonexistentType) {
  Factory<std::string, ProductBase> factory;
  auto obj = factory.create("nonexistent");
  EXPECT_EQ(obj, nullptr);
}

TEST(FactoryTest, Contains) {
  Factory<std::string, ProductBase> factory;
  factory.add("A", []() { return std::make_unique<ProductA>(); });
  EXPECT_TRUE(factory.contains("A"));
  EXPECT_FALSE(factory.contains("B"));
}

TEST(FactoryTest, Remove) {
  Factory<std::string, ProductBase> factory;
  factory.add("A", []() { return std::make_unique<ProductA>(); });
  EXPECT_TRUE(factory.contains("A"));
  EXPECT_TRUE(factory.remove("A"));
  EXPECT_FALSE(factory.contains("A"));
  EXPECT_FALSE(factory.remove("A"));
}

TEST(FactoryTest, Clear) {
  Factory<std::string, ProductBase> factory;
  factory.add("A", []() { return std::make_unique<ProductA>(); });
  factory.add("B", []() { return std::make_unique<ProductB>(); });
  EXPECT_FALSE(factory.empty());
  factory.clear();
  EXPECT_TRUE(factory.empty());
  EXPECT_EQ(factory.create("A"), nullptr);
}

TEST(FactoryTest, Empty) {
  Factory<std::string, ProductBase> factory;
  EXPECT_TRUE(factory.empty());
}

TEST(FactoryTest, AddDuplicateReturnsFalse) {
  Factory<std::string, ProductBase> factory;
  EXPECT_TRUE(factory.add("A", []() { return std::make_unique<ProductA>(); }));
  EXPECT_FALSE(factory.add("A", []() { return std::make_unique<ProductB>(); }));
}

TEST(FactoryTest, FactoryWithIntKeys) {
  Factory<int, ProductBase> factory;
  factory.add(1, []() { return std::make_unique<ProductA>(); });
  factory.add(2, []() { return std::make_unique<ProductB>(); });
  EXPECT_EQ(factory.create(1)->type(), "A");
  EXPECT_EQ(factory.create(2)->type(), "B");
  EXPECT_EQ(factory.create(3), nullptr);
}

TEST(FactoryTest, CreateWithBoundParameters) {
  Factory<std::string, ProductBase> factory;
  factory.add("param", [](int v) { return std::make_unique<ProductWithParam>(v); }, 42);
  auto obj = factory.create("param");
  ASSERT_NE(obj, nullptr);
  EXPECT_EQ(obj->type(), "Param:42");
}

TEST(FactoryTest, CreateWithMultiBoundParameters) {
  Factory<std::string, ProductBase> factory;
  factory.add("multi", [](int a, std::string b) { return std::make_unique<ProductWithMultiParam>(a, b); },
              100, "hello");
  auto obj = factory.create("multi");
  ASSERT_NE(obj, nullptr);
  EXPECT_EQ(obj->type(), "hello:100");
}
