#include "pickup/plugin/plugin_loader.h"

#include <iostream>

namespace pickup {
namespace plugin {

PluginLoader::~PluginLoader() { unload(); }

bool PluginLoader::load(const std::string& path) {
  if (library_.isLoaded()) {
    std::cerr << "[PluginLoader] already loaded: " << library_.path() << "\n";
    return false;
  }

  if (!library_.load(path)) {
    std::cerr << "[PluginLoader] failed to load '" << path << "': "
              << library_.lastError() << "\n";
    return false;
  }

  create_ = reinterpret_cast<CreateFunc>(library_.getSymbol("createPlugin"));
  if (!create_) {
    std::cerr << "[PluginLoader] symbol 'createPlugin' not found in '" << path
              << "': " << library_.lastError() << "\n";
    library_.unload();
    return false;
  }

  destroy_ = reinterpret_cast<DestroyFunc>(library_.getSymbol("destroyPlugin"));
  if (!destroy_) {
    std::cerr << "[PluginLoader] symbol 'destroyPlugin' not found in '" << path
              << "': " << library_.lastError() << "\n";
    library_.unload();
    return false;
  }

  plugin_.reset(create_());
  if (!plugin_) {
    std::cerr << "[PluginLoader] createPlugin() returned nullptr for '" << path << "'\n";
    library_.unload();
    return false;
  }

  if (!plugin_->initialize()) {
    std::cerr << "[PluginLoader] plugin '" << plugin_->name() << "' failed to initialize\n";
    destroy_(plugin_.release());
    library_.unload();
    return false;
  }

  return true;
}

void PluginLoader::unload() {
  if (plugin_) {
    plugin_->shutdown();
    destroy_(plugin_.release());
  }
  library_.unload();
  create_ = nullptr;
  destroy_ = nullptr;
}

}  // namespace plugin
}  // namespace pickup
