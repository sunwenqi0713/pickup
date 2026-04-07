#pragma once

#include <memory>
#include <string>

#include "pickup/plugin/plugin_base.h"
#include "pickup/utils/dynamic_library.h"

namespace pickup {
namespace plugin {

/**
 * @brief 负责单个插件的加载、初始化和卸载
 */
class PluginLoader {
 public:
  using CreateFunc = PluginBase* (*)();
  using DestroyFunc = void (*)(PluginBase*);

  PluginLoader() = default;
  ~PluginLoader();

  PluginLoader(const PluginLoader&) = delete;
  PluginLoader& operator=(const PluginLoader&) = delete;

  /** @brief 加载并初始化插件，失败返回 false */
  bool load(const std::string& path);

  /** @brief 关闭并卸载插件 */
  void unload();

  /** @brief 返回插件实例，未加载时返回 nullptr */
  PluginBase* plugin() const { return plugin_.get(); }

 private:
  utils::DynamicLibrary library_;
  std::unique_ptr<PluginBase> plugin_;
  CreateFunc create_ = nullptr;
  DestroyFunc destroy_ = nullptr;
};

}  // namespace plugin
}  // namespace pickup
