
#ifndef ALCHEMIST_MATH_VECTOR_EXT_H
#define ALCHEMIST_MATH_VECTOR_EXT_H

#include "math/vector/vec2.hpp"
#include "math/vector/vec3.hpp"
#include "math/vector/vec4.hpp"

#include <iostream>

std::ostream &operator<<(std::ostream &os, const vec2 &v) {
    os << "vec2(" << v.x << ", " << v.y << ")";
    return os;
}

std::ostream &operator<<(std::ostream &os, const vec3 &v) {
    os << "vec3(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

std::ostream &operator<<(std::ostream &os, const vec4 &v) {
    os << "vec4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
    return os;
}

#endif // ALCHEMIST_MATH_VECTOR_EXT_H
