#include <iostream>
#include <string>

#include "pickup/utils/Factory.hpp"

class Base {
 public:
  virtual std::string Name() const { return "base"; }
};

class Derived : public Base {
 public:
  virtual std::string Name() const { return "derived"; }
};

int main() {
  using namespace pickup::utils;
  Factory<std::string, Base> factory;
  factory.registerCreateMethod("derived_class", []() { return std::make_unique<Derived>(); });
  auto derived_ptr = factory.create("derived_class");
  std::cout << "class name: " << derived_ptr->Name() << std::endl;
  return 0;
}