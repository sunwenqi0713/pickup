#include <gtest/gtest.h>
#include <memory>
#include <string>

#include "pickup/plugin/Plugin.h"
#include "pickup/plugin/PluginManager.h"

using namespace pickup::plugin;

namespace {

class MockPlugin : public Plugin {
 public:
  explicit MockPlugin(const std::string& name,
                      const std::string& version = "1.0")
      : Plugin(name, version) {}

  bool initialize() override {
    if (failInit_) return false;
    initialized_ = true;
    return true;
  }

  void shutdown() override {
    shutdown_ = true;
    initialized_ = false;
  }

  bool initialized_ = false;
  bool shutdown_ = false;
  bool failInit_ = false;
};

}  // namespace

TEST(PluginManagerTest, InitiallyEmpty) {
  PluginManager mgr;
  EXPECT_EQ(mgr.pluginCount(), 0);
  EXPECT_EQ(mgr.getPlugin("nonexistent"), nullptr);
}

TEST(PluginManagerTest, AddPlugin) {
  PluginManager mgr;
  auto plugin = std::make_shared<MockPlugin>("test");
  mgr.addPlugin("test", plugin);
  EXPECT_EQ(mgr.pluginCount(), 1);
}

TEST(PluginManagerTest, GetPlugin) {
  PluginManager mgr;
  auto plugin = std::make_shared<MockPlugin>("test");
  mgr.addPlugin("test", plugin);
  auto retrieved = mgr.getPlugin("test");
  ASSERT_NE(retrieved, nullptr);
  EXPECT_EQ(retrieved->name(), "test");
}

TEST(PluginManagerTest, GetPluginNonexistent) {
  PluginManager mgr;
  EXPECT_EQ(mgr.getPlugin("missing"), nullptr);
}

TEST(PluginManagerTest, InitializePlugins) {
  PluginManager mgr;
  auto plugin = std::make_shared<MockPlugin>("test");
  mgr.addPlugin("test", plugin);
  EXPECT_TRUE(mgr.initializePlugins());
  EXPECT_TRUE(plugin->initialized_);
}

TEST(PluginManagerTest, InitializePluginsIdempotent) {
  PluginManager mgr;
  auto plugin = std::make_shared<MockPlugin>("test");
  mgr.addPlugin("test", plugin);
  EXPECT_TRUE(mgr.initializePlugins());
  EXPECT_TRUE(plugin->initialized_);
  EXPECT_TRUE(mgr.initializePlugins());
}

TEST(PluginManagerTest, InitializePluginsFailure) {
  PluginManager mgr;
  auto plugin = std::make_shared<MockPlugin>("test");
  plugin->failInit_ = true;
  mgr.addPlugin("test", plugin);
  EXPECT_FALSE(mgr.initializePlugins());
}

TEST(PluginManagerTest, DestroyAll) {
  PluginManager mgr;
  auto plugin = std::make_shared<MockPlugin>("test");
  mgr.addPlugin("test", plugin);
  EXPECT_TRUE(mgr.initializePlugins());
  EXPECT_TRUE(plugin->initialized_);
  mgr.destroyAll();
  EXPECT_TRUE(plugin->shutdown_);
  EXPECT_EQ(mgr.pluginCount(), 0);
}

TEST(PluginManagerTest, DestroyAllWithNoPlugins) {
  PluginManager mgr;
  EXPECT_NO_THROW(mgr.destroyAll());
}

TEST(PluginManagerTest, MultiplePlugins) {
  PluginManager mgr;
  auto p1 = std::make_shared<MockPlugin>("p1");
  auto p2 = std::make_shared<MockPlugin>("p2");
  mgr.addPlugin("p1", p1);
  mgr.addPlugin("p2", p2);
  EXPECT_EQ(mgr.pluginCount(), 2);
  EXPECT_TRUE(mgr.initializePlugins());
  EXPECT_TRUE(p1->initialized_);
  EXPECT_TRUE(p2->initialized_);
  mgr.destroyAll();
  EXPECT_TRUE(p1->shutdown_);
  EXPECT_TRUE(p2->shutdown_);
}

TEST(PluginManagerTest, InitializeTwice) {
  PluginManager mgr;
  auto plugin = std::make_shared<MockPlugin>("test");
  mgr.addPlugin("test", plugin);
  EXPECT_TRUE(mgr.initializePlugins());
  EXPECT_TRUE(mgr.initializePlugins());
  EXPECT_TRUE(plugin->initialized_);
}

TEST(PluginManagerTest, LoadPluginsFromNonexistentDir) {
  PluginManager mgr;
  EXPECT_TRUE(mgr.loadPluginsFromDirectory("C:\\nonexistent_dir_12345"));
  EXPECT_EQ(mgr.pluginCount(), 0);
}
