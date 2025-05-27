#include "pickup/application/Application.h"

#include <iostream>
#include <thread>

#include "pickup/application/SignalHandler.h"

namespace pickup {
namespace application {

Application::Application() { start_time_ = std::chrono::steady_clock::now(); }

Application::~Application() {}

bool Application::init(int argc, char** argv) {
  // 注册信号处理函数
  SignalHandler::getInstance().registerHandler(SIGINT, [this](int signal) {
    std::cout << "Received signal: " << signal << std::endl;
    // 中断信号，通常由键盘 Ctrl+C 触发
    should_exit_.store(true);
  });

  SignalHandler::getInstance().registerHandler(SIGTERM, [this](int signal) {
    std::cout << "Received signal: " << signal << std::endl;
    // 终止信号，通常由 kill 命令发送
    should_exit_.store(true);
  });

  // 解析命令行参数
  parseArguments(argc, argv);

  if (!preInitialize()) {
    std::cerr << "Pre-initialization failed." << std::endl;
    return false;
  }

  if (!initialize()) {
    std::cerr << "Initialization failed." << std::endl;
    return false;
  }

  if (!postInitialize()) {
    std::cerr << "Post-initialization failed." << std::endl;
    return false;
  }

  initialized_ = true;
  return true;
}

// TODO: run in loop
int Application::run() {
  if (!initialized_) {
    std::cerr << "Application not initialized." << std::endl;
    return -1;
  }

  int ret = main(args_);
  shutdown();
  return ret;
}

int Application::main(const ArgVec& args) {
  while (!should_exit_.load()) {
    // 主业务逻辑循环
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Application is running..." << std::endl;
  }

  return 0;
}

void Application::addSubsystem(Subsystem::Ptr pSubsystem) { subsystems_.emplace_back(pSubsystem); }

bool Application::preInitialize() {
  // 预初始化阶段，加载配置文件和初始化日志系统
  if (!loadConfiguration("config.yaml")) {
    std::cerr << "Failed to load configuration." << std::endl;
    return false;
  }
  std::cout << "Configuration loaded successfully." << std::endl;

  if (!initializingLogger()) {
    std::cerr << "Failed to initialize logger." << std::endl;
    return false;
  }
  std::cout << "Logger initialized successfully." << std::endl;

  return true;
}

bool Application::postInitialize() {
  for (auto& pSub : subsystems_) {
    std::cout << "Initializing subsystem: " << pSub->name() << std::endl;
    if (!pSub->initialize()) {
      std::cerr << "Failed to initialize subsystem: " << pSub->name() << std::endl;
      return false;
    }
    std::cout << "Subsystem initialized: " << pSub->name() << std::endl;
  }

  return true;
}

void Application::shutdown() {
  for (auto it = subsystems_.rbegin(); it != subsystems_.rend(); ++it) {
    auto& pSub = *it;
    std::cout << "Stopping subsystem: " << pSub->name() << std::endl;
    pSub->stop();
    std::cout << "Subsystem stopped: " << pSub->name() << std::endl;
  }

  for (auto it = subsystems_.rbegin(); it != subsystems_.rend(); ++it) {
    auto& pSub = *it;
    std::cout << "Uninitializing subsystem: " << pSub->name() << std::endl;
    pSub->uninitialize();
    std::cout << "Subsystem uninitialized: " << pSub->name() << std::endl;
  }
}

void Application::parseArguments(int argc, char** argv) {
  args_.reserve(argc);
  for (int i = 0; i < argc; ++i) {
    std::string arg(argv[i]);
    args_.emplace_back(arg);
  }
}

bool Application::initializingLogger() { return true; }

bool Application::loadConfiguration(const std::string& path) { return true; }

std::chrono::steady_clock::duration Application::uptime() const {
  auto uptime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start_time_);
  return uptime;
}

}  // namespace application
}  // namespace pickup