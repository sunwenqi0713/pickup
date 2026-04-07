#pragma once

#include <string>

namespace pickup {
namespace plugin {

/**
 * @brief 插件基类，所有插件必须继承此类并实现生命周期接口
 */
class PluginBase {
 public:
  PluginBase(std::string name = "", std::string version = "")
      : name_(std::move(name)), version_(std::move(version)) {}
  virtual ~PluginBase() = default;

  PluginBase(const PluginBase&) = delete;
  PluginBase& operator=(const PluginBase&) = delete;

  const std::string& name() const { return name_; }
  const std::string& version() const { return version_; }

  virtual bool initialize() = 0;
  virtual void shutdown() = 0;

 private:
  std::string name_;
  std::string version_;
};

}  // namespace plugin
}  // namespace pickup

// =============================================================================
// 插件导出宏
// =============================================================================

#ifdef _WIN32
#define PLUGIN_EXPORT __declspec(dllexport)
#else
#define PLUGIN_EXPORT __attribute__((visibility("default")))
#endif

#define REGISTER_PLUGIN(PluginClass)                                      \
  extern "C" {                                                            \
  PLUGIN_EXPORT pickup::plugin::PluginBase* createPlugin() {             \
    return new PluginClass();                                             \
  }                                                                       \
  PLUGIN_EXPORT void destroyPlugin(pickup::plugin::PluginBase* plugin) { \
    delete plugin;                                                        \
  }                                                                       \
  }
