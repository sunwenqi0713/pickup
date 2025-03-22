#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <iostream>

#include "pickup/application/Subsystem.h"

namespace pickup {
namespace application {

class Application {
 public:
  using ArgVec = std::vector<std::string>;
  using SubsystemVec = std::vector<Subsystem::Ptr>;

  explicit Application(int argc, char** argv);
  ~Application();

  virtual bool onStartUp() = 0;
  virtual void onCleanUp() = 0;
  virtual int run();
  virtual int main(const ArgVec& args);

  void setArgs(int argc, char** argv);
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

  void loadConfiguration(const std::string& path);

 protected:
  void initialize();
  void uninitialize();
  void reinitialize();

 private:
  std::string name_;
  SubsystemVec subsystems_;
  bool initialized_ = false;
  ArgVec args_;
};

}  // namespace application
}  // namespace pickup