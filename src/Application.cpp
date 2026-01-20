#include "pickup/application/Application.h"

namespace pickup {
namespace application {

Application::Application(const std::string& name) : name_(name), signalHandler_(SignalHandler::getInstance()) {
  // 设置默认信号处理器
  signalHandler_.onShutdown([this]() {
    std::cout << "\nShutdown requested, stopping application..." << std::endl;
    this->stop();
  });

  std::cout << "Application '" << name_ << "' created" << std::endl;
}

Application::~Application() {
  stop();

  if (mainThread_.joinable()) {
    mainThread_.join();
  }

  std::cout << "Application '" << name_ << "' destroyed" << std::endl;
}

bool Application::start() {
  if (started_) {
    std::cout << "Application already started" << std::endl;
    return true;
  }

  std::cout << "Starting application: " << name_ << std::endl;

  // 启动所有组件
  if (!startComponents()) {
    std::cerr << "Failed to start components" << std::endl;
    return false;
  }

  started_ = true;
  running_ = true;

  std::cout << "Application started successfully" << std::endl;
  return true;
}

void Application::stop() {
  if (!started_) {
    return;
  }

  running_ = false;
  std::cout << "Stopping application: " << name_ << std::endl;

  // 停止所有组件
  stopComponents();

  started_ = false;
  std::cout << "Application stopped" << std::endl;
}

int Application::run() {
  if (!start()) {
    return 1;
  }

  mainLoop();
  return 0;
}

void Application::runAsync() {
  if (!start()) {
    return;
  }

  mainThread_ = std::thread(&Application::mainLoop, this);
}

void Application::wait() {
  if (mainThread_.joinable()) {
    mainThread_.join();
  }
}

bool Application::startComponents() {
  std::lock_guard<std::mutex> lock(mutex_);

  std::cout << "Starting " << components_.size() << " components..." << std::endl;
  for (auto& component : components_) {
    std::cout << "  Starting component: " << component->getName() << std::endl;
    if (!component->start()) {
      std::cerr << "Failed to start component: " << component->getName() << std::endl;
      // 回滚：停止已启动的组件
      for (auto& comp : components_) {
        if (comp->isRunning()) {
          std::cout << "  Rolling back component: " << comp->getName() << std::endl;
          comp->stop();
        }
      }
      return false;
    }
    component->setRunning(true);
  }

  std::cout << "All components started successfully" << std::endl;
  return true;
}

void Application::stopComponents() {
  std::lock_guard<std::mutex> lock(mutex_);

  std::cout << "Stopping " << components_.size() << " components..." << std::endl;
  // 反向停止组件
  for (auto it = components_.rbegin(); it != components_.rend(); ++it) {
    auto& component = *it;
    if (component->isRunning()) {
      std::cout << "  Stopping component: " << component->getName() << std::endl;
      component->stop();
      component->setRunning(false);
    }
  }
  std::cout << "All components stopped" << std::endl;
}

void Application::mainLoop() {
  std::cout << "Application running. Press Ctrl+C to stop." << std::endl;

  auto lastUpdate = std::chrono::steady_clock::now();
  int updateCount = 0;

  while (running_) {
    // 检查信号
    signalHandler_.checkSignals();

    // 检查是否应该关闭
    if (signalHandler_.shouldShutdown()) {
      stop();
      break;
    }

    // 计算时间间隔
    auto now = std::chrono::steady_clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate).count();
    lastUpdate = now;

    // 更新组件
    bool allOk = true;
    {
      std::lock_guard<std::mutex> lock(mutex_);
      for (auto& component : components_) {
        if (component->isRunning()) {
          if (!component->update()) {
            std::cerr << "Component update failed: " << component->getName() << std::endl;
            allOk = false;
          }
        }
      }
    }

    // 如果有组件更新失败，停止应用
    if (!allOk) {
      std::cerr << "Component failure detected, stopping application" << std::endl;
      stop();
      break;
    }

    updateCount++;

    // 每10次更新显示一次状态
    if (updateCount % 10 == 0) {
      std::cout << "Application update #" << updateCount << std::endl;
    }

    // 限制更新频率（大约每秒10次）
    if (delta < 100) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100 - delta));
    }
  }

  std::cout << "Application main loop ended after " << updateCount << " updates" << std::endl;
}

}  // namespace application
}  // namespace pickup