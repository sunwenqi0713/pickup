#pragma once

#include <atomic>
#include <functional>

#ifdef _WIN32
#include <windows.h>
#endif

namespace pickup {
namespace application {

/**
 * @brief 信号处理器，处理系统退出信号（Ctrl+C、SIGTERM 等），支持优雅关闭。
 */
class SignalHandler {
 public:
  static SignalHandler& getInstance() {
    static SignalHandler instance;
    return instance;
  }

  SignalHandler(const SignalHandler&) = delete;
  SignalHandler& operator=(const SignalHandler&) = delete;

  /**
   * @brief 是否收到关机信号
   */
  bool shouldShutdown() const { return shutdown_.load(); }

  /**
   * @brief 设置优雅关机回调
   */
  void onShutdown(std::function<void()> callback) { shutdownCallback_ = std::move(callback); }

  /**
   * @brief 检查并处理信号
   */
  void checkSignals();

 private:
  SignalHandler();
  ~SignalHandler() = default;

  void initialize();

#ifdef _WIN32
  static BOOL WINAPI consoleHandler(DWORD signal);
#else
  static void signalHandler(int signo);
#endif

 private:
  std::atomic<bool> shutdown_{false};
  std::function<void()> shutdownCallback_;
};

}  // namespace application
}  // namespace pickup
