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

  Subsystem(std::string name = "");
  virtual ~Subsystem();

  /**
   * @brief 初始化子系统
   */
  virtual void initialize() = 0;

  /**
   * @brief 反初始化子系统
   */
  virtual void uninitialize() = 0;

  /**
   * @brief 重新初始化子系统
   * The default implementation just calls uninitialize() followed by initialize(). Actual implementations might want to
   * use a less radical and possibly more performant approach.
   */
  virtual void reinitialize();
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

  /**
   * @brief 子系统是否正在运行
   *
   * @return true
   * @return false
   */
  virtual bool isRunning() const = 0;

  std::string name() const { return name_; }

 private:
  std::string name_;
};

}  // namespace application
}  // namespace pickup