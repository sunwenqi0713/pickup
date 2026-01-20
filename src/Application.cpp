#include "pickup/application/Application.h"

namespace pickup {
namespace application {

Application::Application(const std::string& name)
    : name_(name), signalHandler_(SignalHandler::getInstance()) {
  signalHandler_.onShutdown([this]() { this->quit(); });
}

Application::~Application() {
  stop();
}

bool Application::hasComponent(const std::string& name) const {
  std::lock_guard<std::mutex> lock(mutex_);
  return componentMap_.find(name) != componentMap_.end();
}

size_t Application::componentCount() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return components_.size();
}

bool Application::start() {
  if (started_) {
    return true;
  }

  if (!startComponents()) {
    return false;
  }

  started_ = true;
  running_ = true;
  return true;
}

void Application::stop() {
  if (!started_) {
    return;
  }

  running_ = false;
  shutdownCv_.notify_all();

  stopComponents();
  started_ = false;
}

int Application::run() {
  if (!start()) {
    return 1;
  }

  waitForShutdown();
  return 0;
}

void Application::quit() {
  running_ = false;
  shutdownCv_.notify_all();
}

bool Application::startComponents() {
  std::lock_guard<std::mutex> lock(mutex_);

  for (auto& component : components_) {
    if (!component->start()) {
      // 回滚：停止已启动的组件
      for (auto& comp : components_) {
        if (comp->isRunning()) {
          comp->stop();
          comp->setRunning(false);
        }
      }
      return false;
    }
    component->setRunning(true);
  }

  return true;
}

void Application::stopComponents() {
  std::lock_guard<std::mutex> lock(mutex_);

  // 反向停止组件（后启动的先停止）
  for (auto it = components_.rbegin(); it != components_.rend(); ++it) {
    auto& component = *it;
    if (component->isRunning()) {
      component->stop();
      component->setRunning(false);
    }
  }
}

void Application::waitForShutdown() {
  // 等待退出信号
  std::unique_lock<std::mutex> lock(mutex_);
  shutdownCv_.wait(lock, [this] { return !running_.load(); });

  // 确保组件已停止
  lock.unlock();
  stop();
}

}  // namespace application
}  // namespace pickup