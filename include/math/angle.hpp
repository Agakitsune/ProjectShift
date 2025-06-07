
#ifndef ALCHEMIST_MATH_ANGLE_H
#define ALCHEMIST_MATH_ANGLE_H

#include <cmath>

constexpr float radians(float degrees) {
    return degrees * (M_PI / 180.0f);
}

constexpr float degrees(float radians) {
    return radians * (180.0f / M_PI);
}

#endif // ALCHEMIST_MATH_ANGLE_H
