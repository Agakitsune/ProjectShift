
#ifndef ALCHEMIST_MATH_MATRIX_GRAPHICS_H
#define ALCHEMIST_MATH_MATRIX_GRAPHICS_H

#include "math/matrix/mat4.hpp"

static mat4 look_at(const vec3 &eye, const vec3 &center, const vec3 &up) {
    vec3 f = (center - eye).normalize();
    vec3 s = f.cross(up).normalize();
    vec3 u = s.cross(f);

    if (fabsf(f.dot(up)) > 0.999f) {
        // up and f are almost parallel, use a different up vector
        s = f.cross(vec3(0.0f, 0.0f, 1.0f)).normalize();
        u = s.cross(f).normalize();
    }

    mat4 result;
    result.columns[0] = {s.x, u.x, -f.x, 0.0f};
    result.columns[1] = {s.y, u.y, -f.y, 0.0f};
    result.columns[2] = {s.z, u.z, -f.z, 0.0f};
    result.columns[3] = {-s.dot(eye), -u.dot(eye), f.dot(eye), 1.0f};

    return result;
}

static mat4 raw_look_at(const vec3 &eye, const vec3 &f, const vec3 &u, const vec3 &s) {
    mat4 result;
    result.columns[0] = {s.x, u.x, f.x, 0.0f};
    result.columns[1] = {s.y, u.y, f.y, 0.0f};
    result.columns[2] = {s.z, u.z, f.z, 0.0f};
    result.columns[3] = {-s.dot(eye), -u.dot(eye), -f.dot(eye), 1.0f};

    return result;
}

static mat4 perspective(float fov, float aspect, float near, float far) {
    float f = 1.0f / tan(fov / 2.0f);
    mat4 result;
    result.columns[0] = {f / aspect, 0.0f, 0.0f, 0.0f};
    result.columns[1] = {0.0f, f, 0.0f, 0.0f};
    result.columns[2] = {0.0f, 0.0f, (far + near) / (near - far), -1.0f};
    result.columns[3] = {0.0f, 0.0f, (2 * far * near) / (near - far), 0.0f};

    return result;
}

static mat4 frustum(float left, float right, float bottom, float top, float near, float far) {
    mat4 result;
    result.columns[0] = {2.0f * near / (right - left), 0.0f, 0.0f, 0.0f};
    result.columns[1] = {0.0f, 2.0f * near / (top - bottom), 0.0f, 0.0f};
    result.columns[2] = {(right + left) / (right - left), (top + bottom) / (top - bottom), -(far + near) / (far - near), -1.0f};
    result.columns[3] = {0.0f, 0.0f, -(2 * far * near) / (far - near), 0.0f};

    return result;
}

static mat4 orthographic(float left, float right, float bottom, float top, float near, float far) {
    mat4 result;
    result.columns[0] = {2.0f / (right - left), 0.0f, 0.0f, 0.0f};
    result.columns[1] = {0.0f, 2.0f / (top - bottom), 0.0f, 0.0f};
    result.columns[2] = {0.0f, 0.0f, -2.0f / (far - near), 0.0f};
    result.columns[3] = {-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1.0f};

    return result;
}

#endif // ALCHEMIST_MATH_MATRIX_GRAPHICS_H
