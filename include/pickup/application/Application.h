#pragma once

#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "pickup/application/Component.h"
#include "pickup/application/SignalHandler.h"

namespace pickup {
namespace application {

/**
 * @brief 应用程序框架
 */
class Application {
 public:
  Application(const std::string& name = "App");
  ~Application();

  /**
   * @brief 添加组件
   */
  template <typename T, typename... Args>
  std::shared_ptr<T> addComponent(const std::string& name, Args&&... args) {
    static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");

    std::lock_guard<std::mutex> lock(mutex_);

    // 检查名称是否重复
    if (componentMap_.find(name) != componentMap_.end()) {
      std::cerr << "Component '" << name << "' already exists" << std::endl;
      return nullptr;
    }

    auto component = std::make_shared<T>(name, std::forward<Args>(args)...);

    if (!component->initialize()) {
      std::cerr << "Failed to initialize component: " << name << std::endl;
      return nullptr;
    }

    components_.push_back(component);
    componentMap_[name] = component;

    return component;
  }

  /**
   * @brief 获取组件
   */
  template <typename T>
  std::shared_ptr<T> getComponent(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = componentMap_.find(name);
    if (it == componentMap_.end()) {
      return nullptr;
    }
    return std::dynamic_pointer_cast<T>(it->second);
  }

  /**
   * @brief 启动应用
   */
  bool start();

  /**
   * @brief 停止应用
   */
  void stop();

  /**
   * @brief 运行应用（阻塞）
   */
  int run();

  /**
   * @brief 异步运行
   */
  void runAsync();

  /**
   * @brief 等待应用结束
   */
  void wait();

  /**
   * @brief 退出应用
   */
  void quit() { running_ = false; }

  /**
   * @brief 获取应用状态
   */
  bool isRunning() const { return running_; }

  /**
   * @brief 获取应用名称
   */
  std::string getName() const { return name_; }

 private:
  // 启动所有组件
  bool startComponents();

  // 停止所有组件
  void stopComponents();

  // 主循环
  void mainLoop();

 private:
  std::string name_;
  std::atomic<bool> running_{false};
  std::atomic<bool> started_{false};

  std::vector<Component::Ptr> components_;
  std::unordered_map<std::string, Component::Ptr> componentMap_;
  mutable std::mutex mutex_;

  std::thread mainThread_;
  SignalHandler& signalHandler_;
};

}  // namespace application
}  // namespace pickup