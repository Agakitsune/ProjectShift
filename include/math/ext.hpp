
#ifndef ALCHEMIST_MATH_EXT_H
#define ALCHEMIST_MATH_EXT_H

#include "math/quaternion.hpp"

#include <iostream>

std::ostream &operator<<(std::ostream &os, const quaternion &q) {
    os << "quaternion(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w
       << ")";
    return os;
}

#endif // ALCHEMIST_MATH_EXT_H
