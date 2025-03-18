#pragma once
#include <chrono>
#include <iostream>

/*
// Usage:
StopWatch sw;
sw.start();
func();
std::cout << "Elapsed " << sw.elapsed() << std::endl;
*/

namespace pickup {
namespace utils {
  
  class StopWatch {
   public:
    StopWatch() : running_(false) {}
    ~StopWatch() = default;
  
    void start() {
      if (!running_) {
        start_time_ = std::chrono::high_resolution_clock::now();
        running_ = true;
      }
    }
  
    void stop() {
      if (running_) {
        auto now = std::chrono::high_resolution_clock::now();
        elapsed_ += now - start_time_;
        running_ = false;
      }
    }
  
    template <typename Duration = std::chrono::milliseconds>
    auto elapsed() const {
      if (running_) {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<Duration>(elapsed_ + (now - start_time_)).count();
      } else {
        return std::chrono::duration_cast<Duration>(elapsed_).count();
      }
    }
  
    void reset() {
      elapsed_ = std::chrono::nanoseconds(0);
      running_ = false;
    }
  
    void print(const std::string& msg = "") const {
      auto ns = elapsed<std::chrono::nanoseconds>();
      std::cout << msg << "Cost: " << ns << " ns (" << ns / 1000.0 << " μs, " << ns / 1000000.0 << " ms)\n";
    }
  
   private:
    std::chrono::high_resolution_clock::time_point start_time_;
    std::chrono::high_resolution_clock::duration elapsed_{};
    bool running_;
  };
  
}  // namespace utils
}  // namespace pickup