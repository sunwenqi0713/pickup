#include "pickup/application/Subsystem.h"

namespace pickup {
namespace application {

Subsystem::Subsystem(std::string name) : name_(std::move(name)) {}

Subsystem::~Subsystem() {}

void Subsystem::reinitialize() {
  uninitialize();
  initialize();
}

}  // namespace application
}  // namespace pickup