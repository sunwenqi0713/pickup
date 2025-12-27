#pragma once

#include <atomic>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>

// Windows信号处理
#ifdef _WIN32
#include <windows.h>

#include <csignal>
#else
// Linux/Unix信号处理
#include <csignal>
#include <cstring>
#endif

namespace pickup {
namespace application {

/**
 * @brief 信号处理器
 */
class SignalHandler {
 public:
  static SignalHandler& getInstance() {
    static SignalHandler instance;
    return instance;
  }

  /**
   * @brief 注册信号处理器
   */
  void registerSignal(int signo, std::function<void()> handler);

  /**
   * @brief 检查并处理信号
   */
  void checkSignals();

  /**
   * @brief 是否收到关机信号
   */
  bool shouldShutdown() const { return shutdown_; }

  /**
   * @brief 设置优雅关机回调
   */
  void onShutdown(std::function<void()> callback) { shutdownCallback_ = callback; }

 private:
  SignalHandler();
  ~SignalHandler();

  // 初始化信号处理
  void initialize();

  // 平台相关的信号处理函数
#ifdef _WIN32
  static BOOL WINAPI consoleHandler(DWORD signal);
#else
  static void signalHandler(int signo);
#endif

 private:
  std::mutex mutex_;
  std::map<int, std::function<void()>> handlers_;
  std::atomic<bool> shutdown_{false};
  std::function<void()> shutdownCallback_;
  bool initialized_{false};
};

}  // namespace application
}  // namespace pickup