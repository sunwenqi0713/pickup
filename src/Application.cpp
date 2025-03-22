#include "pickup/application/Application.h"

#include <iostream>

namespace pickup {
namespace application {

Application::Application(int argc, char** argv) { setArgs(argc, argv); }

Application::~Application() {}

void Application::setArgs(int argc, char** argv) {
  args_.reserve(argc);
  for (int i = 0; i < argc; ++i) {
    std::string arg(argv[i]);
    args_.emplace_back(arg);
  }
}

void Application::addSubsystem(Subsystem::Ptr pSubsystem) { subsystems_.emplace_back(pSubsystem); }

void Application::initialize() {
  for (auto& pSub : subsystems_) {
    // std::cout << "Initializing subsystem: " << pSub->name() << std::endl;
    pSub->initialize();
  }
  initialized_ = true;
}

void Application::uninitialize() {
  if (initialized_) {
    for (auto& pSub : subsystems_) {
      // std::cout << "Uninitializing subsystem: " << pSub->name() << std::endl;
      pSub->uninitialize();
    }
    initialized_ = false;
  }
}

void Application::reinitialize() {
  for (auto& pSub : subsystems_) {
    // std::cout << "Re-initializing subsystem: " << pSub->name() << std::endl;
    pSub->reinitialize();
  }
}

void Application::loadConfiguration(const std::string& path) {}

// TODO: run in loop
int Application::run() {
  if (!onStartUp()) {
    std::cerr << "Failed to start application" << std::endl;
    return 1;
  }

  try {
    initialize();
    main(args_);
    uninitialize();
  } catch (std::exception& exc) {
    std::cerr << exc.what() << std::endl;
  } catch (...) {
    std::cerr << "system exception" << std::endl;
  }

  onCleanUp();
  return 0;
}

int Application::main(const ArgVec& args) { return 0; }

}  // namespace application
}  // namespace pickup