#include <iostream>
#include <string>

#include "pickup/utils/Factory.hpp"
struct Shape {
    virtual void draw() = 0;
    virtual ~Shape() = default;
};

struct Circle : Shape {
    Circle(int radius) : radius(radius) {}
    void draw() override { std::cout << "Circle radius=" << radius << std::endl; }
    int radius;
};

struct Square : Shape {
    Square(int side, int color) : side(side), color(color) {}
    void draw() override { std::cout << "Square side=" << side << " color=" << color << std::endl; }
    int side;
    int color;
};

int main() {
    pickup::utils::Factory<std::string, Shape> factory;
    
    factory.add("circle", [](int r) { return std::make_unique<Circle>(r); });
    factory.add("square", [](int s, int c) { return std::make_unique<Square>(s, c); });

    auto c = factory.create("circle", 5);
    auto s = factory.create("square", 10, 2);

    c->draw(); // Circle radius=5
    s->draw(); // Square side=10 color=2

    if (factory.contains("circle")) factory.remove("circle");
}
