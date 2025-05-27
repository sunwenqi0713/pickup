#pragma once

#include <memory>
#include <string>

namespace pickup {
namespace application {
/**
 * @brief 子系统基类
 *
 */
class Subsystem {
 public:
  using Ptr = std::shared_ptr<Subsystem>;

  Subsystem(std::string name = "") : name_(std::move(name)) {};
  virtual ~Subsystem() = default;

  /**
   * @brief 初始化子系统
   */
  virtual bool initialize() = 0;

  /**
   * @brief 反初始化子系统
   */
  virtual void uninitialize() = 0;

  /**
   * @brief 启动子系统
   *
   */
  virtual void start() = 0;

  /**
   * @brief 停止子系统
   *
   */
  virtual void stop() = 0;

  std::string name() const { return name_; }

 private:
  std::string name_;
};

}  // namespace application
}  // namespace pickup