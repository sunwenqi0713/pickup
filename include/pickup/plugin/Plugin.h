#pragma once

#include <string>

namespace pickup {
namespace plugin {

/**
 * @brief 插件基类，所有插件必须继承此类并实现生命周期接口
 */
class Plugin {
 public:
  /**
   * @brief 构造插件
   * @param name    插件名称
   * @param version 插件版本
   */
  Plugin(std::string name = "", std::string version = "")
      : name_(std::move(name)), version_(std::move(version)) {}
  virtual ~Plugin() = default;

  Plugin(const Plugin&) = delete;
  Plugin& operator=(const Plugin&) = delete;

  /** @brief 获取插件名称 */
  const std::string& name() const { return name_; }

  /** @brief 获取插件版本 */
  const std::string& version() const { return version_; }

  /**
   * @brief 初始化插件
   * @return 成功返回 true，失败返回 false
   */
  virtual bool initialize() = 0;

  /**
   * @brief 关闭插件并释放资源
   */
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
  PLUGIN_EXPORT pickup::plugin::Plugin* createPlugin() {                 \
    return new PluginClass();                                             \
  }                                                                       \
  PLUGIN_EXPORT void destroyPlugin(pickup::plugin::Plugin* plugin) {     \
    delete plugin;                                                        \
  }                                                                       \
  }
