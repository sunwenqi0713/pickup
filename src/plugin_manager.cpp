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

PluginManager::~PluginManager() { destroyAll(); }

bool PluginManager::loadPlugin(const std::string& path) {
  using CreateFunc = PluginBase* (*)();
  using DestroyFunc = void (*)(PluginBase*);

  auto library = std::make_unique<utils::DynamicLibrary>();
  if (!library->load(path)) {
    std::cerr << "[PluginManager] failed to load '" << path << "': "
              << library->lastError() << "\n";
    return false;
  }

  auto createFn = reinterpret_cast<CreateFunc>(library->getSymbol("createPlugin"));
  if (!createFn) {
    std::cerr << "[PluginManager] symbol 'createPlugin' not found in '" << path << "'\n";
    return false;
  }

  auto destroyFn = reinterpret_cast<DestroyFunc>(library->getSymbol("destroyPlugin"));
  if (!destroyFn) {
    std::cerr << "[PluginManager] symbol 'destroyPlugin' not found in '" << path << "'\n";
    return false;
  }

  PluginBase* instance = createFn();
  if (!instance) {
    std::cerr << "[PluginManager] createPlugin() returned nullptr for '" << path << "'\n";
    return false;
  }

  std::string name = instance->name();

  std::lock_guard<std::mutex> lock(mutex_);
  if (index_.count(name)) {
    std::cerr << "[PluginManager] plugin '" << name << "' is already loaded\n";
    destroyFn(instance);
    return false;
  }

  index_.emplace(name, std::shared_ptr<PluginBase>(instance, destroyFn));
  plugins_.push_back({std::move(name), index_[name], std::move(library)});
  return true;
}

void PluginManager::addPlugin(const std::string& name, std::shared_ptr<PluginBase> plugin) {
  std::lock_guard<std::mutex> lock(mutex_);
  index_.emplace(name, plugin);
  plugins_.push_back({name, std::move(plugin), nullptr});
}

bool PluginManager::loadPluginsFromDirectory(const std::string& dir) {
  if (!std::filesystem::exists(dir)) return true;

  for (const auto& entry : std::filesystem::directory_iterator(dir)) {
    if (!entry.is_regular_file()) continue;
    if (entry.path().extension() != kPluginExtension) continue;

    if (!loadPlugin(entry.path().string())) {
      return false;
    }
  }
  return true;
}

bool PluginManager::initializePlugins() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (initialized_) {
    return true;
  }

  for (auto& entry : plugins_) {
    if (!entry.plugin->initialize()) {
      std::cerr << "[PluginManager] plugin '" << entry.name << "' failed to initialize\n";
      return false;
    }
  }

  initialized_ = true;
  return true;
}

void PluginManager::destroyAll() {
  std::lock_guard<std::mutex> lock(mutex_);
  initialized_ = false;
  index_.clear();
  // 按注册逆序销毁，依赖后加载的插件可能先被清理
  for (auto it = plugins_.rbegin(); it != plugins_.rend(); ++it) {
    it->plugin->shutdown();
  }
  plugins_.clear();
}

std::shared_ptr<PluginBase> PluginManager::getPlugin(const std::string& name) const {
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = index_.find(name);
  return it != index_.end() ? it->second : nullptr;
}

size_t PluginManager::pluginCount() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return plugins_.size();
}

}  // namespace plugin
}  // namespace pickup
