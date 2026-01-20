#include "pickup/application/SignalHandler.h"

#ifndef _WIN32
#include <csignal>
#include <cstring>
#endif

namespace pickup {
namespace application {

SignalHandler::SignalHandler() { initialize(); }

void SignalHandler::initialize() {
#ifdef _WIN32
  SetConsoleCtrlHandler(consoleHandler, TRUE);
#else
  struct sigaction action;
  std::memset(&action, 0, sizeof(action));
  action.sa_handler = signalHandler;
  sigemptyset(&action.sa_mask);

  sigaction(SIGINT, &action, nullptr);   // Ctrl+C
  sigaction(SIGTERM, &action, nullptr);  // kill
  sigaction(SIGQUIT, &action, nullptr);  // Ctrl+'\'
#endif
}

void SignalHandler::checkSignals() {
  if (shutdown_.load()) {
    if (shutdownCallback_) {
      shutdownCallback_();
    }
    shutdown_.store(false);
  }
}

#ifdef _WIN32
BOOL WINAPI SignalHandler::consoleHandler(DWORD signal) {
  switch (signal) {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
      getInstance().shutdown_.store(true);
      return TRUE;
    default:
      return FALSE;
  }
}
#else
void SignalHandler::signalHandler(int signo) {
  (void)signo;
  // 设置标志
  getInstance().shutdown_.store(true);
}
#endif

}  // namespace application
}  // namespace pickup