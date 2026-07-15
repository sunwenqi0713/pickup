#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "pickup/plugin/Plugin.h"
#include "pickup/utils/DynamicLibrary.h"

namespace pickup {
namespace plugin {

/**
 * @brief 插件管理器，负责插件的加载、注册、初始化与销毁
 *
 * - 支持从动态库加载或直接注册已有插件实例
 * - 两阶段初始化：先加载创建，后统一初始化
 * - 线程安全：所有公共方法均通过内部互斥锁保护
 */
class PluginManager {
 public:
  PluginManager() = default;
  ~PluginManager();

  PluginManager(const PluginManager&) = delete;
  PluginManager& operator=(const PluginManager&) = delete;

  /**
   * @brief 从动态库加载并创建插件
   * @param path 动态库路径
   * @return 成功返回 true
   * @note 此阶段仅创建插件实例，不调用 initialize()
   */
  [[nodiscard]] bool loadPlugin(const std::string& path);

  /**
   * @brief 直接注册一个已有的插件实例
   * @param name   插件名称
   * @param plugin 插件实例
   * @note 适用于非动态库的插件，如静态注册或测试
   */
  void addPlugin(const std::string& name, std::shared_ptr<Plugin> plugin);

  /**
   * @brief 加载目录下所有平台对应的插件文件
   * @param dir 插件目录；目录不存在时返回 true（视为空目录）
   * @return 全部加载成功返回 true，任意失败返回 false
   */
  [[nodiscard]] bool loadPluginsFromDirectory(const std::string& dir);

  /**
   * @brief 初始化所有已加载的插件
   * @return 全部初始化成功返回 true；失败时已初始化的插件不会被回滚
   * @note 重复调用无效（幂等）
   */
  [[nodiscard]] bool initializePlugins();

  /**
   * @brief 销毁所有插件并卸载动态库（按注册逆序）
   */
  void destroyAll();

  /**
   * @brief 按名称查找插件
   * @return 找到返回插件共享指针，未找到返回 nullptr
   */
  [[nodiscard]] std::shared_ptr<Plugin> getPlugin(const std::string& name) const;

  /** @brief 当前已加载的插件数量 */
  [[nodiscard]] size_t pluginCount() const;

 private:
  struct PluginEntry {
    std::string name;
    std::shared_ptr<Plugin> plugin;
    std::unique_ptr<utils::DynamicLibrary> library;  ///< 外部注册的插件为 nullptr
  };

  std::vector<PluginEntry> plugins_;                                   ///< 按注册顺序排列
  std::unordered_map<std::string, std::shared_ptr<Plugin>> index_;  ///< 名称索引
  mutable std::mutex mutex_;
  bool initialized_{false};
};

}  // namespace plugin
}  // namespace pickup
