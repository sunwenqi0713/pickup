#include "pickup/thread/Event.h"

namespace pickup {
namespace thread {

Event::Event(bool manualReset) noexcept : manualReset_(manualReset) {}

bool Event::wait(int64_t timeoutMs) {
  std::unique_lock<std::mutex> lock(mutex_);

  if (!triggered_) {
    if (timeoutMs == Event::TIMEOUT_IMMEDIATE) {
      cv_.wait(lock, [this] { return triggered_ == true; });
    } else {
      std::chrono::milliseconds period(timeoutMs);
      if (!cv_.wait_for(lock, period, [this] { return triggered_ == true; })) {
        return false;
      }
    }
  }

  if (!manualReset_) {
    reset();
  }

  return true;
}

void Event::signal() {
  std::lock_guard<std::mutex> lock(mutex_);

  triggered_ = true;
  cv_.notify_all();
}

void Event::reset() { triggered_ = false; }

}  // namespace thread
}  // namespace pickup
