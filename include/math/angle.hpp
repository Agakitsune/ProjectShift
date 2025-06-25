
#ifndef ALCHEMIST_MATH_ANGLE_H
#define ALCHEMIST_MATH_ANGLE_H

#define PI 3.1415926535897932384626

#include <cmath>

constexpr float radians(float degrees) {
    return degrees * (PI / 180.0f);
}

constexpr float degrees(float radians) {
    return radians * (180.0f / PI);
}

#endif // ALCHEMIST_MATH_ANGLE_H
