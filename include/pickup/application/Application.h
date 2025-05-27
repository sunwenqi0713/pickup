#pragma once

#include <atomic>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include "pickup/application/Subsystem.h"

namespace pickup {
namespace application {

class Application {
 public:
  using ArgVec = std::vector<std::string>;
  using SubsystemVec = std::vector<Subsystem::Ptr>;

  Application();
  virtual ~Application();

  bool init(int argc, char** argv);
  int run();

  virtual int main(const ArgVec& args);

  bool startup();
  void shutdown();

  /**
   * @brief 添加子系统
   *
   * @param pSubsystem
   */
  void addSubsystem(Subsystem::Ptr pSubsystem);

  template <typename SubsystemType>
  SubsystemType& getSubsystem() const {
    for (const auto& pSub : subsystems_) {
      const Subsystem* pSS(pSub.get());
      SubsystemType* pST = dynamic_cast<SubsystemType*>(pSS);
      if (pST != nullptr) return *pST;
    }
    std::cout << "Subsystem not found: " << typeid(SubsystemType).name() << std::endl;
  }

  std::chrono::steady_clock::duration uptime() const;

 protected:
  virtual bool initialize() = 0;
  bool preInitialize();
  bool postInitialize();
  void parseArguments(int argc, char** argv);
  bool initializingLogger();
  bool loadConfiguration(const std::string& path);

 private:
  ArgVec args_;
  SubsystemVec subsystems_;
  bool initialized_{false};
  std::atomic_bool should_exit_{false};
  std::chrono::steady_clock::time_point start_time_;
};

}  // namespace application
}  // namespace pickup