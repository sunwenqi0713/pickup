#include "pickup/thread/Thread.h"

#include "pickup/utils/platform.h"
#ifdef PICKUP_WIN_OS
#include <Windows.h>
#else
#include <pthread.h>
#endif

namespace pickup {
namespace thread {

std::thread::id Thread::get_id() const noexcept { return thread_.get_id(); }

bool Thread::joinable() const noexcept { return thread_.joinable(); }

void Thread::join() { thread_.join(); }

size_t Thread::hardware_concurrency() noexcept {
  const auto hc = std::thread::hardware_concurrency();
  return (hc != 0) ? hc : kDefaultNumberOfCores;
}

#ifdef PICKUP_WIN_OS

void Thread::set_name(const std::string& name) noexcept {
  const std::wstring utf16_name(name.begin(), name.end());
  ::SetThreadDescription(::GetCurrentThread(), utf16_name.data());
}

void Thread::set_thread_priority(int priority) noexcept {
  // Windows 优先级等级
  const int priorities[] = {
      THREAD_PRIORITY_IDLE,          // 空闲优先级
      THREAD_PRIORITY_LOWEST,        // 最低优先级
      THREAD_PRIORITY_BELOW_NORMAL,  // 低于正常的优先级
      THREAD_PRIORITY_NORMAL,        // 正常优先级（默认）
      THREAD_PRIORITY_ABOVE_NORMAL,  // 高于正常的优先级
      THREAD_PRIORITY_HIGHEST,       // 最高优先级
      THREAD_PRIORITY_TIME_CRITICAL  // 时间关键优先级
  };

  if (priority >= 0 && priority <= 6) {
    SetThreadPriority(::GetCurrentThread(), priorities[priority]);
  }
}

#elif defined(PICKUP_UNIX_OS)

void Thread::set_name(const std::string& name) noexcept { ::pthread_setname_np(::pthread_self(), name.data()); }

void Thread::set_thread_priority(int priority) noexcept {
  // 调度策略参数
  sched_param param;
  param.sched_priority = priority;  // 通常范围 1-99（实时策略）

  // 设置调度策略（需要root权限）
  ::pthread_setschedparam(::pthread_self(), SCHED_RR, &param);
}

#elif defined(PICKUP_MAC_OS)

void Thread::set_name(const std::string& name) noexcept { ::pthread_setname_np(name.data()); }

void Thread::set_thread_priority(int priority) noexcept {
  // 调度策略参数
  sched_param param;
  param.sched_priority = priority;  // 通常范围 1-99（实时策略）

  // 设置调度策略（需要root权限）
  ::pthread_setschedparam(::pthread_self(), SCHED_RR, &param);
}

#endif

}  // namespace thread
}  // namespace pickup
