#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
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

    auto component = std::make_shared<T>(name, std::forward<Args>(args)...);

    /** @brief initialize() 在锁外调用，避免持锁执行耗时操作或回调导致死锁 */
    if (!component->initialize()) {
      return nullptr;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (findComponent(name) != nullptr) {
      return nullptr;  // 名称已存在
    }
    components_.push_back(component);
    return component;
  }

  /**
   * @brief 获取组件
   * @tparam T 组件类型
   * @param name 组件名称
   * @return 成功返回组件指针，未找到返回 nullptr
   */
  template <typename T>
  std::shared_ptr<T> getComponent(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return std::dynamic_pointer_cast<T>(findComponent(name));
  }

  /**
   * @brief 检查组件是否存在
   */
  bool hasComponent(const std::string& name) const;

  /**
   * @brief 获取组件数量
   */
  size_t componentCount() const;

  /**
   * @brief 启动应用
   * @return 成功返回 true，失败返回 false
   */
  bool start();

  /**
   * @brief 停止应用并释放所有组件资源（幂等，可安全多次调用）
   *
   * 反向停止所有组件，等同于完整的清理流程。
   * 析构函数会自动调用本函数。
   */
  void stop();

  /**
   * @brief 运行应用（阻塞直到退出）
   * @return 退出码（0 表示正常退出）
   *
   * 等价于 start() + 等待退出信号 + stop()。
   * 推荐在 main() 中使用此函数。
   */
  int run();

  /**
   * @brief 发送退出信号（异步，不阻塞）
   *
   * 仅通知 run() 中的等待循环退出，不直接停止组件。
   * 实际清理由 run() 在收到信号后调用 stop() 完成。
   * 可安全地从信号处理函数或其他线程调用。
   *
   * @note 若未通过 run() 运行应用，请直接调用 stop()。
   */
  void quit();

  /** @brief 获取运行状态 */
  bool isRunning() const { return running_.load(); }

  /** @brief 获取应用名称 */
  const std::string& name() const { return name_; }

 private:
  bool startComponents();
  void stopComponents();
  void waitForShutdown();

  /** @brief 按名称查找组件（调用方须持有 mutex_） */
  Component::Ptr findComponent(const std::string& name) const;

 private:
  std::string name_;
  std::atomic<bool> running_{false};
  std::atomic<bool> started_{false};

  std::vector<Component::Ptr> components_;

  mutable std::mutex mutex_;           ///< 保护 components_ 列表
  mutable std::mutex shutdownMutex_;   ///< 保护 shutdown 信号，与 mutex_ 分离
  std::condition_variable shutdownCv_; ///< 与 shutdownMutex_ 配合使用

  SignalHandler& signalHandler_;
};

}  // namespace application
}  // namespace pickup
