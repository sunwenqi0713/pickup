#include "pickup/utils/unused.hpp"

using namespace pickup::utils;

int main() {
  int a = 0;
  float b = 0.0f;
  char c[] = "test";
  PICKUP_UNUSED(a, b, c); // No warning: PICKUP_UNUSED variable 'a', 'b', 'c'.

  int d = 0;
  PICKUP_UNUSED(d); // No warning: PICKUP_UNUSED variable 'd'.

  return 0;
}