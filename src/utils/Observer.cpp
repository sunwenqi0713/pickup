#include "pickup/utils/Observer.h"

#include <set>

namespace pickup {
namespace utils {

void Observable::addObserver(const std::shared_ptr<Observer>& o) {
  if (o == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(mutex_);
  if (observers_.count(o) > 0) {
    return;
  }

  observers_.insert(o);
}

void Observable::removeObserver(const std::shared_ptr<Observer>& o) {
  std::lock_guard<std::mutex> lock(mutex_);
  observers_.erase(o);
}

void Observable::removeAllObservers() {
  std::lock_guard<std::mutex> lock(mutex_);
  observers_.clear();
}

size_t Observable::observerCount() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return observers_.size();
}

void Observable::notifyObservers() {
  notifyObservers(nullptr);
}

void Observable::notifyObservers(const ObserverArg* arg) {
  std::set<std::shared_ptr<Observer>> snapshot;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!changed_) {
      return;
    }

    snapshot = observers_;
    clearChanged();
  }

  for (auto& o : snapshot) {
    o->update(this, arg);
  }
}

}  // namespace utils
}  // namespace pickup
