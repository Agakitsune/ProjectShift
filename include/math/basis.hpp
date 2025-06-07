
#ifndef ALCHEMIST_MATH_BASIS_H
#define ALCHEMIST_MATH_BASIS_H

#include "math/vector/vec3.hpp"

struct basis {
    vec3 x;
    vec3 y;
    vec3 z;

    constexpr basis()
        : x(1.0f, 0.0f, 0.0f), y(0.0f, 1.0f, 0.0f), z(0.0f, 0.0f, 1.0f) {}
    constexpr basis(const vec3 &x, const vec3 &y, const vec3 &z)
        : x(x), y(y), z(z) {}
};

#endif // ALCHEMIST_MATH_BASIS_H
