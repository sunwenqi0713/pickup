#pragma once

#include <memory>
#include <string>

namespace pickup {
namespace application {

/**
 * @brief 应用组件基类
 *
 * 组件是应用程序的基本构建块，具有独立的生命周期。
 * 继承此类并实现生命周期方法来创建自定义组件。
 *
 * 生命周期：initialize() → start() → [运行中] → stop()
 *
 * @code
 * class MyComponent : public Component {
 *  public:
 *   using Component::Component;
 *
 *   bool initialize() override {
 *     // 初始化资源
 *     return true;
 *   }
 *
 *   bool start() override {
 *     // 启动工作线程、事件循环等
 *     worker_ = std::thread([this] { run(); });
 *     return true;
 *   }
 *
 *   bool stop() override {
 *     // 停止工作线程
 *     running_ = false;
 *     if (worker_.joinable()) worker_.join();
 *     return true;
 *   }
 * };
 * @endcode
 */
class Component {
 public:
  using Ptr = std::shared_ptr<Component>;

  explicit Component(const std::string& name);
  virtual ~Component() = default;

  Component(const Component&) = delete;
  Component& operator=(const Component&) = delete;

  /**
   * @brief 初始化组件
   * @return 成功返回 true，失败返回 false
   * @note 在 start() 之前调用，用于分配资源
   */
  virtual bool initialize() { return true; }

  /**
   * @brief 启动组件
   * @return 成功返回 true，失败返回 false
   * @note 在此启动工作线程、事件循环等
   */
  virtual bool start() { return true; }

  /**
   * @brief 停止组件
   * @return 成功返回 true，失败返回 false
   * @note 在此停止工作线程、释放资源
   */
  virtual bool stop() { return true; }

  /// 获取组件名称
  const std::string& name() const { return name_; }

  /// 检查组件是否正在运行
  bool isRunning() const { return running_; }

 protected:
  friend class Application;
  void setRunning(bool running) { running_ = running; }

 private:
  std::string name_;
  bool running_{false};
};

}  // namespace application
}  // namespace pickup