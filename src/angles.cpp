#include "pickup/math/angles.h"

#include <corecrt_math_defines.h>
#include <math.h>

namespace pickup {
namespace utils {

double fromDegrees(double degrees) { return degrees * M_PI / 180.0; }

double toDegrees(double radians) { return radians * 180.0 / M_PI; }

double normalizeAnglePositive(double angle) {
  const double result = fmod(angle, 2.0 * M_PI);
  if (result < 0) return result + 2.0 * M_PI;
  return result;
}

double normalizeAngle(double angle) {
  const double result = fmod(angle + M_PI, 2.0 * M_PI);
  if (result <= 0.0) return result + M_PI;
  return result - M_PI;
}

}  // namespace utils
}  // namespace pickup