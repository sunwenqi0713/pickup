#include "pickup/thread/Thread.h"

namespace pickup {
namespace thread {

std::thread::id Thread::get_id() const noexcept { return thread_.get_id(); }

bool Thread::joinable() const noexcept { return thread_.joinable(); }

void Thread::join() { thread_.join(); }

size_t Thread::hardware_concurrency() noexcept {
  const auto hc = std::thread::hardware_concurrency();
  return (hc != 0) ? hc : kDefaultNumberOfCores;
}

}  // namespace thread
}  // namespace pickup
