
#ifndef ALCHEMIST_MATH_MATRIX_MAT3_H
#define ALCHEMIST_MATH_MATRIX_MAT3_H

#include "math/vector/vec3.hpp"

#include "math/matrix/mat4.hpp"

struct mat3 {
    vec3 columns[3];

    constexpr mat3() : columns{vec3(1.0f, 0.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f),
        vec3(0.0f, 0.0f, 1.0f)} {}
    constexpr mat3(float diagonal) : columns{vec3(diagonal, 0.0f, 0.0f),
                        vec3(0.0f, diagonal, 0.0f),
                        vec3(0.0f, 0.0f, diagonal)} {}
    constexpr mat3(const vec3 &col0, const vec3 &col1, const vec3 &col2)
    : columns{col0, col1, col2} {}
    constexpr mat3(const mat3 &m) : columns{m.columns[0], m.columns[1], m.columns[2]} {}
    constexpr mat3(mat3 &&m) : columns{m.columns[0], m.columns[1], m.columns[2]} {}

    constexpr mat3(const mat4 &m) : columns{m.columns[0], m.columns[1], m.columns[2]} {}

    constexpr mat3 operator*(float scalar) {
        mat3 result = *this;

        result.columns[0] *= scalar;
        result.columns[1] *= scalar;
        result.columns[2] *= scalar;

        return result;
    }

    constexpr mat3 inverse() const {
        float determinant = 1.0f / (
            columns[0][0] * (columns[1][1] * columns[2][2] - columns[2][1] * columns[1][2])
            - columns[1][0] * (columns[0][1] * columns[2][2] - columns[2][1] * columns[0][2])
            + columns[2][0] * (columns[0][1] * columns[1][2] - columns[1][1] * columns[0][2])
        );

        mat3 inverse(
            vec3(
                +(columns[1][1] * columns[2][2] - columns[2][1] * columns[1][2]),
                -(columns[0][1] * columns[2][2] - columns[2][1] * columns[0][2]),
                +(columns[0][1] * columns[1][2] - columns[1][1] * columns[0][2])
            ),
            vec3(
                -(columns[1][0] * columns[2][2] - columns[2][0] * columns[1][2]),
                +(columns[0][0] * columns[2][2] - columns[2][0] * columns[0][2]),
                -(columns[0][0] * columns[1][2] - columns[1][0] * columns[0][2])
            ),
            vec3(
                +(columns[1][0] * columns[2][1] - columns[2][0] * columns[1][1]),
                -(columns[0][0] * columns[2][1] - columns[2][0] * columns[0][1]),
                +(columns[0][0] * columns[1][1] - columns[1][0] * columns[0][1])
            )
        );

        return inverse * determinant;
    }

    constexpr mat3 transpose() const {
        mat3 result;

        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                result.columns[i][j] = columns[j][i];
            }
        }

        return result;
    }

    constexpr float *data() {
        return &columns[0].x;
    }

    constexpr const float *data() const {
        return &columns[0].x;
    }
};

#endif
