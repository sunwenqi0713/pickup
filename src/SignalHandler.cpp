#include "pickup/application/SignalHandler.h"

namespace pickup {
namespace application {

SignalHandler::SignalHandler() { initialize(); }

SignalHandler::~SignalHandler() {
  // 清理资源
}

void SignalHandler::initialize() {
  if (initialized_) return;

#ifdef _WIN32
  // Windows平台使用控制台处理器
  if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
    std::cerr << "Failed to set console handler" << std::endl;
    return;
  }
#else
    // Linux/Unix平台使用信号处理器
    struct sigaction action;
    std::memset(&action, 0, sizeof(action));
    action.sa_handler = signalHandler;
    sigemptyset(&action.sa_mask);
    
    // 注册常见信号
    sigaction(SIGINT, &action, nullptr);   // Ctrl+C
    sigaction(SIGTERM, &action, nullptr);  // kill
    sigaction(SIGQUIT, &action, nullptr);
#endif
    
    // 设置默认处理器
    registerSignal(1, [this]() {
        std::cout << "\nShutdown signal received" << std::endl;
        shutdown_ = true;
    });
    
    registerSignal(2, [this]() {
        std::cout << "\nTermination signal received" << std::endl;
        shutdown_ = true;
    });
    
    initialized_ = true;
}

void SignalHandler::registerSignal(int signo, std::function<void()> handler) {
    std::lock_guard<std::mutex> lock(mutex_);
    handlers_[signo] = handler;
}

void SignalHandler::checkSignals() {
    if (shutdown_) {
        if (shutdownCallback_) {
            shutdownCallback_();
        }
        shutdown_ = false;
    }
}

#ifdef _WIN32
BOOL WINAPI SignalHandler::consoleHandler(DWORD signal) {
    auto& instance = getInstance();
    
    switch (signal) {
        case CTRL_C_EVENT:
            std::cout << "\nCtrl+C received" << std::endl;
            instance.shutdown_ = true;
            return TRUE;
            
        case CTRL_CLOSE_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
            std::cout << "\nSystem shutdown event" << std::endl;
            instance.shutdown_ = true;
            return TRUE;
            
        default:
            return FALSE;
    }
}
#else
void SignalHandler::signalHandler(int signo) {
    auto& instance = getInstance();
    
    // 调用注册的处理器
    std::lock_guard<std::mutex> lock(instance.mutex_);
    auto it = instance.handlers_.find(signo);
    if (it != instance.handlers_.end() && it->second) {
        it->second();
    } else {
        // 默认处理
        std::cout << "\nReceived signal: " << signo << std::endl;
        instance.shutdown_ = true;
    }
}
#endif

} // namespace application
} // namespace pickup