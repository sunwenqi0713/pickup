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

#elif defined(PICKUP_UNIX_OS)

void Thread::set_name(const std::string& name) noexcept { ::pthread_setname_np(::pthread_self(), name.data()); }

#elif defined(PICKUP_MAC_OS)

void Thread::set_name(const std::string& name) noexcept { ::pthread_setname_np(name.data()); }

#endif

}  // namespace thread
}  // namespace pickup
