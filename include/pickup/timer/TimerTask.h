#pragma once

#include <memory>

namespace pickup {
namespace timer {

class TimerTask {
 public:
  virtual ~TimerTask();
  virtual void run() = 0;
};

using TimerTaskPtr = std::shared_ptr<TimerTask>;

}  // namespace timer
}  // namespace pickup
