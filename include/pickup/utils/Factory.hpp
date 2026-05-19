#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>

namespace pickup {
namespace utils {

/**
 * @brief 通用对象工厂模板类（支持可变参数构造）
 * @tparam IdentifierType 类型标识符类型（通常为字符串/枚举）
 * @tparam AbstractProduct 抽象产品类型（基类/接口）
 */
template <typename IdentifierType, typename AbstractProduct>
class Factory {
public:
    Factory() = default;

    /**
     * @brief 注册类型和创建方法
     * @tparam Creator 创建函数类型（lambda/std::function等）
     * @param id 类型标识符
     * @param creator 创建函数
     * @return 注册成功返回true，id已存在返回false
     */
    template <typename Creator>
    bool add(const IdentifierType& id, Creator&& creator) {
        auto wrapper = [creator = std::forward<Creator>(creator)](auto&&... args) -> std::unique_ptr<AbstractProduct> {
            return creator(std::forward<decltype(args)>(args)...);
        };
        return producers_.emplace(id, std::move(wrapper)).second;
    }

    /**
     * @brief 创建对象（可变参数）
     * @tparam Args 构造参数类型
     * @param id 类型标识符
     * @param args 构造参数
     * @return 成功返回对象指针，失败返回nullptr
     */
    template <typename... Args>
    std::unique_ptr<AbstractProduct> create(const IdentifierType& id, Args&&... args) {
        auto iter = producers_.find(id);
        if (iter != producers_.end()) {
            return iter->second(std::forward<Args>(args)...);
        } else {
            std::cerr << "Factory creation failed for type: " << id << std::endl;
            return nullptr;
        }
    }

    // 检查是否包含指定类型
    bool contains(const IdentifierType& id) const {
        return producers_.find(id) != producers_.end();
    }

    // 注销指定类型
    bool remove(const IdentifierType& id) {
        return producers_.erase(id) == 1;
    }

    // 清空所有注册项
    void clear() noexcept { producers_.clear(); }

    // 判断工厂是否为空
    bool empty() const noexcept { return producers_.empty(); }

private:
    std::map<IdentifierType, std::function<std::unique_ptr<AbstractProduct>(auto&&...)>> producers_;
};

}  // namespace utils
}  // namespace pickup
