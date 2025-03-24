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
  ~Application();

  bool init(int argc, char** argv);
  virtual int run();

  virtual bool onStartUp() = 0;
  virtual void onCleanUp() = 0;
  virtual int main(const ArgVec& args);

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

  void parseArguments(int argc, char** argv);
  void loadConfiguration(const std::string& path);
  void initializingLogger();

  std::chrono::steady_clock::duration uptime() const;

 protected:
  void initialize();
  void uninitialize();
  void reinitialize();

 private:
  ArgVec args_;
  SubsystemVec subsystems_;
  bool initialized_ = false;
  std::atomic_bool should_exit_{false};
  std::chrono::steady_clock::time_point start_time_;
};

}  // namespace application
}  // namespace pickup