#pragma once

#include <functional>
#include <memory>
#include <string>

namespace pickup {
namespace application {

/**
 * @brief 组件基类
 */
class Component {
 public:
  using Ptr = std::shared_ptr<Component>;

  Component(const std::string& name);
  virtual ~Component() = default;

  // 基础生命周期方法
  virtual bool initialize() { return true; }
  virtual bool start() { return true; }
  virtual bool stop() { return true; }
  virtual bool update() { return true; }

  std::string getName() const { return name_; }
  bool isRunning() const { return running_; }
  void setRunning(bool running) { running_ = running; }

 private:
  std::string name_;
  bool running_{false};
};

}  // namespace application
}  // namespace pickup