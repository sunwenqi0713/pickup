#pragma once

#include <csignal>
#include <functional>
#include <map>

namespace pickup {
namespace application {

class SignalHandler {
 public:
  using SignalCallback = std::function<void(int)>;

  static SignalHandler& getInstance() {
    static SignalHandler instance;
    return instance;
  }

  void registerHandler(int signal, SignalCallback callback) {
    handlers_[signal] = callback;
    std::signal(signal, SignalHandler::handleSignal);
  }

  static void handleSignal(int signal) {
    auto& instance = getInstance();
    if (instance.handlers_.find(signal) != instance.handlers_.end()) {
      instance.handlers_[signal](signal);
    }
  }

 private:
  SignalHandler() = default;
  ~SignalHandler() = default;

  SignalHandler(const SignalHandler&) = delete;
  SignalHandler& operator=(const SignalHandler&) = delete;

  std::map<int, SignalCallback> handlers_;
};

}  // namespace application
}  // namespace pickup