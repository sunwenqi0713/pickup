#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <utility>

namespace pickup {
namespace utils {

/**
 * @brief 通用对象工厂模板类
 *
 * 注册时绑定创建函数及其参数；创建时不再需要传参。
 *
 * @tparam IdentifierType  类型标识符类型（通常为字符串/枚举）
 * @tparam AbstractProduct 抽象产品类型（基类/接口）
 */
template <typename IdentifierType, typename AbstractProduct>
class Factory {
 public:
  Factory() = default;

  /**
   * @brief 注册类型、创建函数及参数
   * @tparam Fn    创建函数类型（lambda / 可调用对象）
   * @tparam Args  创建函数所需的参数类型
   * @param id     类型标识符
   * @param fn     创建函数
   * @param args   创建函数参数
   * @return 注册成功返回 true，id 已存在返回 false
   */
  template <typename Fn, typename... Args>
  bool add(const IdentifierType& id, Fn&& fn, Args&&... args) {
    return producers_
        .emplace(id,
                 [fn = std::forward<Fn>(fn),
                  args = std::make_tuple(std::forward<Args>(args)...)]() -> std::unique_ptr<AbstractProduct> {
                   return std::apply(fn, args);
                 })
        .second;
  }

  /**
   * @brief 创建对象
   * @param id 类型标识符
   * @return 成功返回对象指针，失败返回 nullptr
   */
  std::unique_ptr<AbstractProduct> create(const IdentifierType& id) {
    auto iter = producers_.find(id);
    if (iter != producers_.end()) {
      return iter->second();
    }
    std::cerr << "Factory creation failed for type: " << id << std::endl;
    return nullptr;
  }

  /** @brief 检查是否包含指定类型 */
  bool contains(const IdentifierType& id) const { return producers_.find(id) != producers_.end(); }

  /** @brief 注销指定类型，成功返回 true */
  bool remove(const IdentifierType& id) { return producers_.erase(id) == 1; }

  /** @brief 清空所有注册项 */
  void clear() noexcept { producers_.clear(); }

  /** @brief 判断工厂是否为空 */
  bool empty() const noexcept { return producers_.empty(); }

 private:
  std::map<IdentifierType, std::function<std::unique_ptr<AbstractProduct>()>> producers_;
};

}  // namespace utils
}  // namespace pickup
