#pragma once

/*
  int a = 0;
  float b = 0.0f;
  char c[] = "test";
  PICKUP_UNUSED(a, b, c); // No warning: PICKUP_UNUSED variable 'a', 'b', 'c'.

  int d = 0;
  PICKUP_UNUSED(d); // No warning: PICKUP_UNUSED variable 'd'.
*/

namespace pickup {
namespace utils {

// Function with varying number of arguments to avoid "unused variable" warnings.
template <typename... Args>
#if defined(__cpp_constexpr) && __cpp_constexpr >= 201304L
constexpr
#else
inline
#endif
    void
    unused(const Args &...) noexcept {
}

}  // namespace utils
}  // namespace pickup

#if defined(_MSC_VER)
// Macro with varying number of arguments to avoid "unused variable" warnings.
#define PICKUP_UNUSED(...) ((void)(__VA_ARGS__))
#else
// Macro with varying number of arguments to avoid "unused variable" warnings.
#define PICKUP_UNUSED(...) (decltype(pickup::utils::unused(__VA_ARGS__))())
#endif
