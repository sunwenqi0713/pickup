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

  // 初始化日志系统
  initializingLogger();

  // 解析命令行参数
  parseArguments(argc, argv);

  // 加载配置文件
  loadConfiguration("config.json");

  return true;
}

// TODO: run in loop
int Application::run() {
  if (!onStartUp()) {
    std::cerr << "Failed to start application" << std::endl;
    return 1;
  }

  try {
    initialize();
    main(args_);
    uninitialize();
  } catch (std::exception& exc) {
    std::cerr << exc.what() << std::endl;
  } catch (...) {
    std::cerr << "system exception" << std::endl;
  }

  onCleanUp();
  return 0;
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

void Application::initialize() {
  for (auto& pSub : subsystems_) {
    std::cout << "Initializing subsystem: " << pSub->name() << std::endl;
    pSub->initialize();
  }
  initialized_ = true;
}

void Application::uninitialize() {
  if (initialized_) {
    for (auto& pSub : subsystems_) {
      std::cout << "Uninitializing subsystem: " << pSub->name() << std::endl;
      pSub->uninitialize();
    }
    initialized_ = false;
  }
}

void Application::reinitialize() {
  for (auto& pSub : subsystems_) {
    std::cout << "Re-initializing subsystem: " << pSub->name() << std::endl;
    pSub->reinitialize();
  }
}

void Application::parseArguments(int argc, char** argv) {
  args_.reserve(argc);
  for (int i = 0; i < argc; ++i) {
    std::string arg(argv[i]);
    args_.emplace_back(arg);
  }
}

void Application::loadConfiguration(const std::string& path) {}

void Application::initializingLogger() {}

std::chrono::steady_clock::duration Application::uptime() const {
  auto uptime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start_time_);
  return uptime;
}

}  // namespace application
}  // namespace pickup