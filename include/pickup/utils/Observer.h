#pragma once

#include <memory>
#include <mutex>
#include <set>

namespace pickup {
namespace utils {

/**
 * @brief 传递给 Observer::update() 的参数和数据的基类
 */
struct ObserverArg {
  virtual ~ObserverArg() = default;
};

class Observer;

/**
 * @brief 可观察对象基类（被观察者）
 *
 * 实现观察者模式中的被观察者角色，管理 Observer 的注册与通知。
 */
class Observable {
 public:
  virtual ~Observable() = default;

  /**
   * @brief 添加观察者
   * @param o 要添加的观察者指针
   * @note 若 o 为空或已存在，直接返回
   */
  void addObserver(const std::shared_ptr<Observer>& o);

  /**
   * @brief 移除指定的观察者
   * @param o 要移除的观察者指针
   */
  void removeObserver(const std::shared_ptr<Observer>& o);

  /** @brief 移除所有观察者 */
  void removeAllObservers();

  /**
   * @brief 通知所有观察者（不传递数据）
   * @note 等效于 notifyObservers(nullptr)
   */
  void notifyObservers();

  /**
   * @brief 通知所有观察者，传递数据 arg
   * @param arg 传递给 Observer::update() 的参数
   * @note 仅当 changed_ 为 true 时才会触发通知
   */
  void notifyObservers(const ObserverArg* arg);

  /** @brief 获取观察者数量 */
  [[nodiscard]] size_t observerCount() const;

 protected:
  /** @brief 检查 Observable 状态是否已变更 */
  [[nodiscard]] bool hasChanged() const { return changed_; }

  /** @brief 设置 Observable 状态为已变更 */
  void setChanged() { changed_ = true; }

  /** @brief 清除 Observable 的已变更状态 */
  void clearChanged() { changed_ = false; }

  mutable std::mutex mutex_;                ///< 保护 observers_ 的互斥锁
  std::set<std::shared_ptr<Observer>> observers_;  ///< 观察者集合

 private:
  bool changed_ = false;  ///< 状态变更标记
};

/**
 * @brief 观察者基类
 *
 * 实现观察者模式中的观察者角色，Observable 状态变更时接收通知。
 */
class Observer {
 public:
  virtual ~Observer() = default;

  /**
   * @brief 更新此观察者
   * @param o   触发通知的 Observable 对象
   * @param arg 传递给通知的参数数据
   */
  virtual void update(const Observable* o, const ObserverArg* arg) = 0;
};

}  // namespace utils
}  // namespace pickup
