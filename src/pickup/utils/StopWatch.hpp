#pragma once

#include <chrono>

/*
// Usage:
StopWatch sw;
std::cout << "Elapsed " << std::chrono::duration_cast<std::chrono::milliseconds>(sw.elapsed()) << std::endl;
*/

namespace pickup {
namespace utils {

class StopWatch {
  using clock = std::chrono::steady_clock;
  std::chrono::time_point<clock> start_tp_;

 public:
  StopWatch() : start_tp_{clock::now()} {}

  std::chrono::duration<double> elapsed() const { return std::chrono::duration<double>(clock::now() - start_tp_); }

  void reset() { start_tp_ = clock::now(); }
};

}  // namespace utils
}  // namespace pickup