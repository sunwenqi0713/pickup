#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "pickup/application/Component.h"
#include "pickup/application/SignalHandler.h"

namespace pickup {
namespace application {

/**
 * @brief 轻量级应用程序框架
 *
 * 提供组件化的应用生命周期管理，支持优雅关闭。
 * 采用事件驱动模式，组件自行管理各自的事件循环。
 *
 * @code
 * Application app("MyApp");
 * app.addComponent<MyComponent>("comp1");
 * return app.run();  // 阻塞直到收到退出信号
 * @endcode
 */
class Application {
 public:
  explicit Application(const std::string& name = "App");
  ~Application();

  Application(const Application&) = delete;
  Application& operator=(const Application&) = delete;

  /**
   * @brief 添加组件
   * @tparam T 组件类型（必须继承自 Component）
   * @tparam Args 构造函数参数类型
   * @param name 组件名称（唯一）
   * @param args 传递给组件构造函数的参数
   * @return 成功返回组件指针，失败返回 nullptr
   */
  template <typename T, typename... Args>
  std::shared_ptr<T> addComponent(const std::string& name, Args&&... args) {
    static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");

    std::lock_guard<std::mutex> lock(mutex_);

    if (componentMap_.find(name) != componentMap_.end()) {
      return nullptr;  // 名称已存在
    }

    auto component = std::make_shared<T>(name, std::forward<Args>(args)...);

    if (!component->initialize()) {
      return nullptr;  // 初始化失败
    }

    components_.push_back(component);
    componentMap_[name] = component;

    return component;
  }

  /**
   * @brief 获取组件
   * @tparam T 组件类型
   * @param name 组件名称
   * @return 成功返回组件指针，未找到返回 nullptr
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
   * @brief 检查组件是否存在
   * @param name 组件名称
   * @return 存在返回 true，否则返回 false
   */
  bool hasComponent(const std::string& name) const;

  /**
   * @brief 获取组件数量
   * @return 组件数量
   */
  size_t componentCount() const;

  /**
   * @brief 启动应用
   * @return 成功返回 true，失败返回 false
   */
  bool start();

  /**
   * @brief 停止应用
   */
  void stop();

  /**
   * @brief 运行应用（阻塞直到退出）
   * @return 退出码（0 表示正常退出）
   */
  int run();

  /**
   * @brief 请求退出应用
   */
  void quit();

  /// 获取运行状态
  bool isRunning() const { return running_; }

  /// 获取应用名称
  const std::string& name() const { return name_; }

 private:
  bool startComponents();
  void stopComponents();
  void waitForShutdown();

 private:
  std::string name_;
  std::atomic<bool> running_{false};
  std::atomic<bool> started_{false};

  std::vector<Component::Ptr> components_;
  std::unordered_map<std::string, Component::Ptr> componentMap_;

  mutable std::mutex mutex_;
  std::condition_variable shutdownCv_;

  SignalHandler& signalHandler_;
};

}  // namespace application
}  // namespace pickup