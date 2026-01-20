#pragma once

#if __GNUC__
#define PICKUP_LIKELY(expr) (__builtin_expect(!!(expr), 1))
#define PICKUP_UNLIKELY(expr) (__builtin_expect(!!(expr), 0))
#else
#define PICKUP_LIKELY(expr) (expr)
#define PICKUP_UNLIKELY(expr) (expr)
#endif