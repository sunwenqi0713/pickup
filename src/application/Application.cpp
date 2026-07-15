#include "pickup/application/Application.h"

#include <algorithm>
#include <chrono>

namespace pickup {
namespace application {

Application::Application(const std::string& name)
    : name_(name), signalHandler_(SignalHandler::getInstance()) {
  signalHandler_.onShutdown([this]() { this->quit(); });
}

Application::~Application() {
  stop();
}

Component::Ptr Application::findComponent(const std::string& name) const {
  auto it = std::find_if(components_.begin(), components_.end(),
                         [&name](const Component::Ptr& c) { return c->name() == name; });
  return it != components_.end() ? *it : nullptr;
}

bool Application::hasComponent(const std::string& name) const {
  std::lock_guard<std::mutex> lock(mutex_);
  return findComponent(name) != nullptr;
}

size_t Application::componentCount() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return components_.size();
}

bool Application::start() {
  // compare_exchange_strong 保证并发调用原子性，防止重复启动
  bool expected = false;
  if (!started_.compare_exchange_strong(expected, true)) {
    return true;  // 已启动，幂等返回
  }

  if (!startComponents()) {
    started_ = false;
    return false;
  }

  running_ = true;
  return true;
}

void Application::stop() {
  // compare_exchange_strong 保证只有一个调用者真正执行停止逻辑
  bool expected = true;
  if (!started_.compare_exchange_strong(expected, false)) {
    return;  // 未启动或已停止
  }

  {
    std::lock_guard<std::mutex> lock(shutdownMutex_);
    running_ = false;
    shutdownCv_.notify_all();
  }

  stopComponents();
}

int Application::run() {
  if (!start()) {
    return 1;
  }

  waitForShutdown();
  stop();  // 统一由 run() 负责调用 stop()
  return 0;
}

void Application::quit() {
  std::lock_guard<std::mutex> lock(shutdownMutex_);
  running_ = false;
  shutdownCv_.notify_all();
}

bool Application::startComponents() {
  // 仅在持锁时复制列表快照，组件 start() 在锁外调用，避免死锁
  std::vector<Component::Ptr> snapshot;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    snapshot = components_;
  }

  for (auto& component : snapshot) {
    if (!component->start()) {
      // 回滚：停止已启动组件
      for (auto& comp : snapshot) {
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
  // 仅在持锁时复制列表快照，组件 stop() 在锁外调用，避免死锁
  std::vector<Component::Ptr> snapshot;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    snapshot = components_;
  }

  // 反向停止组件（后启动先停止）
  for (auto it = snapshot.rbegin(); it != snapshot.rend(); ++it) {
    auto& component = *it;
    if (component->isRunning()) {
      component->stop();
      component->setRunning(false);
    }
  }
}

void Application::waitForShutdown() {
  // 使用独立 shutdownMutex_，不阻塞组件管理操作（mutex_）
  std::unique_lock<std::mutex> lock(shutdownMutex_);
  while (running_.load()) {
    // 每 100ms 检查一次系统信号（如 Ctrl+C）
    shutdownCv_.wait_for(lock, std::chrono::milliseconds(100));
    if (signalHandler_.shouldShutdown()) {
      running_ = false;
      break;
    }
  }
  // 不在此处调用 stop()，由 run() 统一负责
}

}  // namespace application
}  // namespace pickup
