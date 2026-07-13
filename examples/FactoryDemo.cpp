#include <iostream>
#include <string>

#include "pickup/utils/Factory.hpp"

struct Shape {
  virtual void draw() = 0;
  virtual ~Shape() = default;
};

struct Circle : Shape {
  explicit Circle(int radius) : radius_(radius) {}
  void draw() override { std::cout << "Circle radius=" << radius_ << std::endl; }
  int radius_;
};

struct Square : Shape {
  Square(int side, int color) : side_(side), color_(color) {}
  void draw() override { std::cout << "Square side=" << side_ << " color=" << color_ << std::endl; }
  int side_;
  int color_;
};

int main() {
  pickup::utils::Factory<std::string, Shape> factory;

  // 绑定创建函数及其参数，创建时无需传参
  factory.add("circle", [](int r) { return std::make_unique<Circle>(r); }, 5);
  factory.add("square", [](int s, int c) { return std::make_unique<Square>(s, c); }, 10, 2);

  auto c = factory.create("circle");
  auto s = factory.create("square");

  c->draw();  // Circle radius=5
  s->draw();  // Square side=10 color=2

  if (factory.contains("circle")) factory.remove("circle");
}
