#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "pickup/plugin/plugin_base.h"
#include "pickup/plugin/plugin_loader.h"

namespace pickup {
namespace plugin {

/**
 * @brief 插件管理器，负责插件的批量加载、查询和卸载
 *
 * 线程安全：所有公共方法均通过内部互斥锁保护。
 */
class PluginManager {
 public:
  PluginManager() = default;
  ~PluginManager();

  PluginManager(const PluginManager&) = delete;
  PluginManager& operator=(const PluginManager&) = delete;

  /**
   * @brief 加载单个插件
   * @param path 插件动态库路径
   * @return 成功返回 true
   */
  bool loadPlugin(const std::string& path);

  /**
   * @brief 加载目录下所有平台对应的插件文件（.so / .dll / .dylib）
   * @param dir 插件目录路径；目录不存在时返回 true（视为空目录）
   * @return 全部加载成功返回 true，任意失败返回 false
   */
  bool loadPluginsFromDirectory(const std::string& dir);

  /**
   * @brief 按注册逆序卸载所有插件
   */
  void unloadAllPlugins();

  /**
   * @brief 按名称查找插件
   * @return 找到返回插件指针，未找到返回 nullptr
   */
  PluginBase* getPlugin(const std::string& name) const;

  /** @brief 当前已加载的插件数量 */
  size_t pluginCount() const;

 private:
  // 有序列表用于按注册逆序卸载
  std::vector<std::unique_ptr<PluginLoader>> loaders_;
  // 名称索引用于 O(1) 查找
  std::unordered_map<std::string, PluginBase*> index_;
  mutable std::mutex mutex_;
};

}  // namespace plugin
}  // namespace pickup
