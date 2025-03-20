#pragma once

#include <functional>
#include <map>
#include <memory>
#include <utility>

namespace pickup {
namespace utils {

template <typename IdentifierType, class AbstractProduct>
class Factory {
  using CreateMethod = std::function<std::unique_ptr<AbstractProduct>()>;
  using MapContainer = std::map<IdentifierType, CreateMethod>;

 public:
  bool registerCreateMethod(const IdentifierType &id, CreateMethod creator) {
    auto id_iter = producers_.find(id);
    if (id_iter == producers_.end()) {
      producers_[id] = creator;
      return true;
    }
    return false;
  }

  bool contains(const IdentifierType &id) { return producers_.find(id) != producers_.end(); }

  bool unregister(const IdentifierType &id) { return producers_.erase(id) == 1; }

  void clear() { producers_.clear(); }

  bool empty() const { return producers_.empty(); }

  template <typename... Args>
  std::unique_ptr<AbstractProduct> createObjectOrNull(const IdentifierType &id, Args &&...args) {
    auto id_iter = producers_.find(id);
    if (id_iter != producers_.end()) {
      return std::unique_ptr<AbstractProduct>((id_iter->second)(std::forward<Args>(args)...));
    }
    return nullptr;
  }

  template <typename... Args>
  std::unique_ptr<AbstractProduct> create(const IdentifierType &id, Args &&...args) {
    auto result = createObjectOrNull(id, std::forward<Args>(args)...);
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