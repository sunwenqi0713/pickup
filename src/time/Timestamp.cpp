#include "pickup/time/Timestamp.h"

#include <cstdint>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#if defined(_MSC_VER)
#include <intrin.h>
#endif
#elif defined(__APPLE__)
#include <mach/mach_time.h>
#include <sys/time.h>
#include <time.h>
#else
#include <time.h>
#endif

namespace pickup {
namespace time {

namespace {

#if defined(_WIN32) || defined(_WIN64)
// FILETIME 以 100ns 为单位、自 1601-01-01 起算；转换为自 1970 起的纳秒
constexpr int64_t kFiletimeEpochDiff100ns = 116444736000000000ll;  // 1601→1970 的 100ns 数

int64_t filetimeToNanos(const FILETIME& ft) noexcept {
  ULARGE_INTEGER u;
  u.LowPart = ft.dwLowDateTime;
  u.HighPart = ft.dwHighDateTime;
  return (static_cast<int64_t>(u.QuadPart) - kFiletimeEpochDiff100ns) * 100;
}
#endif

}  // namespace

int64_t Timestamp::utc() {
#if defined(_WIN32) || defined(_WIN64)
  FILETIME ft;
  GetSystemTimePreciseAsFileTime(&ft);
  return filetimeToNanos(ft);
#else
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return static_cast<int64_t>(ts.tv_sec) * 1000000000ll + ts.tv_nsec;
#endif
}

int64_t Timestamp::local() {
#if defined(_WIN32) || defined(_WIN64)
  FILETIME ft, ftLocal;
  GetSystemTimePreciseAsFileTime(&ft);
  if (!FileTimeToLocalFileTime(&ft, &ftLocal)) return filetimeToNanos(ft);
  return filetimeToNanos(ftLocal);
#else
  int64_t nanos = utc();
  time_t seconds = static_cast<time_t>(nanos / 1000000000ll);
  struct tm local_tm;
  if (localtime_r(&seconds, &local_tm) != &local_tm) return nanos;
  return nanos + static_cast<int64_t>(local_tm.tm_gmtoff) * 1000000000ll;
#endif
}

int64_t Timestamp::nano() {
#if defined(_WIN32) || defined(_WIN64)
  static LARGE_INTEGER frequency = [] {
    LARGE_INTEGER f;
    QueryPerformanceFrequency(&f);
    return f;
  }();
  LARGE_INTEGER counter;
  QueryPerformanceCounter(&counter);
  const int64_t ticks = counter.QuadPart;
  const int64_t freq = frequency.QuadPart;
  // 拆分为整秒 + 余数，避免 ticks * 1e9 溢出
  return (ticks / freq) * 1000000000ll + ((ticks % freq) * 1000000000ll) / freq;
#elif defined(__APPLE__)
  static mach_timebase_info_data_t info = [] {
    mach_timebase_info_data_t i;
    mach_timebase_info(&i);
    return i;
  }();
  return static_cast<int64_t>(mach_absolute_time() * info.numer / info.denom);
#else
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return static_cast<int64_t>(ts.tv_sec) * 1000000000ll + ts.tv_nsec;
#endif
}

int64_t Timestamp::rdts() {
#if defined(_MSC_VER)
  return static_cast<int64_t>(__rdtsc());
#elif defined(__x86_64__) || defined(__amd64__)
  uint32_t low, high;
  __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
  return static_cast<int64_t>((static_cast<uint64_t>(high) << 32) | low);
#elif defined(__i386__)
  uint64_t ret;
  __asm__ volatile("rdtsc" : "=A"(ret));
  return static_cast<int64_t>(ret);
#elif defined(__aarch64__)
  int64_t value;
  __asm__ volatile("mrs %0, cntvct_el0" : "=r"(value));
  return value;
#else
  // 无 TSC 的平台回退到单调时钟
  return nano();
#endif
}

}  // namespace time
}  // namespace pickup