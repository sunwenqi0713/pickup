#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <memory>

namespace pickup {
namespace utils {

/**
 * @brief 通用对象工厂模板类
 * @tparam IdentifierType 类型标识符类型（通常为字符串/枚举）
 * @tparam AbstractProduct 抽象产品类型（基类/接口）
 *
 * @note 典型用法：
 * @code
 * Factory<std::string, Shape> shapeFactory;
 * shapeFactory.registerCreator("circle", []{ return std::make_unique<Circle>(); });
 * auto circle = shapeFactory.create("circle");
 * @endcode
 */
template <typename IdentifierType, class AbstractProduct>
class Factory {
  // 产品创建函数类型
  using CreateMethod = std::function<std::unique_ptr<AbstractProduct>()>;
  // 注册表容器类型
  using MapContainer = std::map<IdentifierType, CreateMethod>;

 public:
    Factory() = default;
    Factory(const Factory&) = delete;
    Factory& operator=(const Factory&) = delete;
    Factory(Factory&&) = delete;
    Factory& operator=(Factory&&) = delete;

    // Register a derived type with default constructor
    template <typename Derived>
    bool registerType(const IdentifierType& id) {
        static_assert(std::is_base_of_v<AbstractProduct, Derived>, "Derived must inherit from AbstractProduct");
        return registerCreator(id, [] { return std::make_unique<Derived>(); });
    }

  /**
   * @brief 注册对象创建方法
   * @param id 类型标识符
   * @param creator 创建函数（需返回AbstractProduct派生类对象）
   * @return 注册成功返回true，id已存在返回false
   */
  bool registerCreator(const IdentifierType& id, CreateMethod creator) {
    return producers_.emplace(id, creator).second;
  }

  // 检查是否包含指定id的创建方法
  bool contains(const IdentifierType& id) const { return producers_.find(id) != producers_.end(); }

  // 注销指定id的创建方法
  bool unregister(const IdentifierType& id) { return producers_.erase(id) == 1; }

  // 清空所有注册项
  void clear() noexcept { producers_.clear(); }

  // 判断工厂是否为空
  bool empty() const noexcept { return producers_.empty(); }

  /**
   * @brief 创建对象（安全版本）
   * @return 成功返回对象指针，失败返回nullptr
   */
  std::unique_ptr<AbstractProduct> createObjectOrNull(const IdentifierType& id) {
    auto iter = producers_.find(id);
    return (iter != producers_.end()) ? iter->second() : nullptr;
  }

  /**
   * @brief 创建对象
   * @return 成功返回对象指针，失败输出日志并返回nullptr
   * @note 后续改为抛出异常或自定义错误处理策略
   */
  std::unique_ptr<AbstractProduct> create(const IdentifierType& id) {
    auto obj = createObjectOrNull(id);
    if (!obj) {
      std::cerr << "Factory creation failed for type: " << id << std::endl;
    }
    return obj;
  }

 private:
  MapContainer producers_;  ///< 类型标识符到创建方法的映射
};

}  // namespace utils
}  // namespace pickup
