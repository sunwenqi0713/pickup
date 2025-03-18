#pragma once

#include <functional>
#include <map>
#include <memory>
#include <utility>

/*
// Usage:
class Base {
 public:
  virtual std::string Name() const { return "base"; }
};

class Derived : public Base {
 public:
  virtual std::string Name() const { return "derived"; }
};

Factory<std::string, Base> factory;
factory.Register("derived_class", []() { return std::make_unique<Derived>(); });
auto derived_ptr = factory.Create("derived_class");
std::cout << "class name: " << derived_ptr->Name() << std::endl;
*/

namespace pickup {
namespace utils {

template <typename IdentifierType, class AbstractProduct>
class Factory {
  using CreateMethod = std::function<std::unique_ptr<AbstractProduct>()>;
  using MapContainer = std::map<IdentifierType, CreateMethod>;

 public:
  bool Register(const IdentifierType &id, CreateMethod creator) {
    auto id_iter = producers_.find(id);
    if (id_iter == producers_.end()) {
      producers_[id] = creator;
      return true;
    }
    return false;
  }

  bool Contains(const IdentifierType &id) { return producers_.find(id) != producers_.end(); }

  bool Unregister(const IdentifierType &id) { return producers_.erase(id) == 1; }

  void Clear() { producers_.clear(); }

  bool Empty() const { return producers_.empty(); }

  template <typename... Args>
  std::unique_ptr<AbstractProduct> CreateObjectOrNull(const IdentifierType &id, Args &&...args) {
    auto id_iter = producers_.find(id);
    if (id_iter != producers_.end()) {
      return std::unique_ptr<AbstractProduct>((id_iter->second)(std::forward<Args>(args)...));
    }
    return nullptr;
  }

  template <typename... Args>
  std::unique_ptr<AbstractProduct> Create(const IdentifierType &id, Args &&...args) {
    auto result = CreateObjectOrNull(id, std::forward<Args>(args)...);
    if (result == nullptr) {
      std::cout << "Factory could not create Object of type : " << id << std::endl;
    }
    return result;
  }

 private:
  MapContainer producers_;
};

}  // namespace utils
}  // namespace pickup