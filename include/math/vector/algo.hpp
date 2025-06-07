
#ifndef ALCHEMIST_MATH_VECTOR_ALGO_H
#define ALCHEMIST_MATH_VECTOR_ALGO_H

#include "math/vector/vec2.hpp"
#include "math/vector/vec3.hpp"
#include "math/vector/vec4.hpp"
#include "math/quaternion.hpp"

#include "math/vector/ext.hpp"
#include "math/ext.hpp"

vec3 arcball(const vec3 &eye, const vec3 &center, const vec3 &up, float pitch, float yaw) {
    float distance = (eye - center).length();
    vec3 f = (eye - center).normalize();
    vec3 s = f.cross(up).normalize();
    vec3 u = s.cross(f);

    if (fabsf(f.dot(up)) > 0.999f) {
        // up and f are almost parallel, use a different up vector
        s = f.cross(vec3(0.0f, 0.0f, 1.0f)).normalize();
        u = s.cross(f).normalize();
    }

    quaternion q = quaternion::from_axis(s, yaw) * quaternion::from_axis(u, pitch);

    return center + q.rotate(f) * distance;
}

#endif // ALCHEMIST_MATH_VECTOR_ALGO_H
