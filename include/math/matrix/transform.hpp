
#ifndef ALCHEMIST_MATH_MATRIX_TRANSFORM_H
#define ALCHEMIST_MATH_MATRIX_TRANSFORM_H

#include "math/matrix/mat4.hpp"
#include "math/vector/vec3.hpp"

static mat4 translate(const vec3 &translation) {
    mat4 result;
    result.columns[0] = {1.0f, 0.0f, 0.0f, 0.0f};
    result.columns[1] = {0.0f, 1.0f, 0.0f, 0.0f};
    result.columns[2] = {0.0f, 0.0f, 1.0f, 0.0f};
    result.columns[3] = {translation.x, translation.y, translation.z, 1.0f};

    return result;
}

static mat4 euler(const vec3 &euler) {
    float c1 = cos(euler.x);
    float s1 = sin(euler.x);
    float c2 = cos(euler.y);
    float s2 = sin(euler.y);
    float c3 = cos(euler.z);
    float s3 = sin(euler.z);

    mat4 result;
    result.columns[0] = {c2 * c3, -c2 * s3, s2, 0.0f};
    result.columns[1] = {s1 * s2 * c3 + c1 * s3, -s1 * s2 * s3 + c1 * c3, -s1 * c2, 0.0f};
    result.columns[2] = {-c1 * s2 * c3 + s1 * s3, c1 * s2 * s3 + s1 * c3, c1 * c2, 0.0f};
    result.columns[3] = {0.0f, 0.0f, 0.0f, 1.0f};

    return result;
}

static mat4 euler_deg(const vec3 &deg) {
    return euler(deg * (M_PI / 180.0f));
}

static mat4 scale(const vec3 &scale) {
    mat4 result;
    result.columns[0] = {scale.x, 0.0f, 0.0f, 0.0f};
    result.columns[1] = {0.0f, scale.y, 0.0f, 0.0f};
    result.columns[2] = {0.0f, 0.0f, scale.z, 0.0f};
    result.columns[3] = {0.0f, 0.0f, 0.0f, 1.0f};

    return result;
}

#endif // ALCHEMIST_MATH_MATRIX_TRANSFORM_H
