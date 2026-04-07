#include "pickup/plugin/plugin_manager.h"

#include <filesystem>
#include <iostream>

namespace pickup {
namespace plugin {

#ifdef _WIN32
static constexpr const char* kPluginExtension = ".dll";
#elif defined(__APPLE__)
static constexpr const char* kPluginExtension = ".dylib";
#else
static constexpr const char* kPluginExtension = ".so";
#endif

PluginManager::~PluginManager() { unloadAllPlugins(); }

bool PluginManager::loadPlugin(const std::string& path) {
  auto loader = std::make_unique<PluginLoader>();
  if (!loader->load(path)) return false;

  PluginBase* plugin = loader->plugin();
  std::lock_guard<std::mutex> lock(mutex_);
  if (index_.count(plugin->name())) {
    std::cerr << "[PluginManager] plugin '" << plugin->name() << "' is already loaded\n";
    return false;
  }
  index_.emplace(plugin->name(), plugin);
  loaders_.push_back(std::move(loader));
  return true;
}

bool PluginManager::loadPluginsFromDirectory(const std::string& dir) {
  if (!std::filesystem::exists(dir)) return true;

  for (const auto& entry : std::filesystem::directory_iterator(dir)) {
    if (!entry.is_regular_file()) continue;
    if (entry.path().extension() != kPluginExtension) continue;

    const std::string path = entry.path().string();
    if (!loadPlugin(path)) {
      std::cerr << "[PluginManager] failed to load plugin: " << path << "\n";
      return false;
    }
  }
  return true;
}

void PluginManager::unloadAllPlugins() {
  std::lock_guard<std::mutex> lock(mutex_);
  index_.clear();
  for (auto it = loaders_.rbegin(); it != loaders_.rend(); ++it) {
    (*it)->unload();
  }
  loaders_.clear();
}

PluginBase* PluginManager::getPlugin(const std::string& name) const {
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = index_.find(name);
  return it != index_.end() ? it->second : nullptr;
}

size_t PluginManager::pluginCount() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return loaders_.size();
}

}  // namespace plugin
}  // namespace pickup
