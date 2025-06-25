
#ifndef ALCHEMIST_MATH_MATRIX_MAT4_H
#define ALCHEMIST_MATH_MATRIX_MAT4_H

#include "math/vector/vec4.hpp"

#include "math/vector/vec3.hpp"

struct mat4 {
    vec4 columns[4];

    constexpr mat4() : columns{vec4(1.0f, 0.0f, 0.0f, 0.0f),
                     vec4(0.0f, 1.0f, 0.0f, 0.0f),
                     vec4(0.0f, 0.0f, 1.0f, 0.0f),
                     vec4(0.0f, 0.0f, 0.0f, 1.0f)} {}
    constexpr mat4(float diagonal) : columns{vec4(diagonal, 0.0f, 0.0f, 0.0f),
                                    vec4(0.0f, diagonal, 0.0f, 0.0f),
                                    vec4(0.0f, 0.0f, diagonal, 0.0f),
                                    vec4(0.0f, 0.0f, 0.0f, diagonal)} {}
    constexpr mat4(const vec4 &col0, const vec4 &col1, const vec4 &col2, const vec4 &col3)
        : columns{col0, col1, col2, col3} {}
    constexpr mat4(const mat4 &m) : columns{m.columns[0], m.columns[1], m.columns[2], m.columns[3]} {}
    constexpr mat4(mat4 &&m) : columns{m.columns[0], m.columns[1], m.columns[2], m.columns[3]} {}

    static mat4 translate(const vec3 &translation) {
        mat4 result(1.0f);
        result.columns[3].x = translation.x;
        result.columns[3].y = translation.y;
        result.columns[3].z = translation.z;
        return result;
    }

    constexpr mat4 &operator=(const mat4 &m) {
        columns[0] = m.columns[0];
        columns[1] = m.columns[1];
        columns[2] = m.columns[2];
        columns[3] = m.columns[3];
        return *this;
    }

    constexpr mat4 &operator=(mat4 &&m) {
        columns[0] = m.columns[0];
        columns[1] = m.columns[1];
        columns[2] = m.columns[2];
        columns[3] = m.columns[3];
        return *this;
    }

    constexpr mat4 operator+(const mat4 &m) const {
        return {columns[0] + m.columns[0], columns[1] + m.columns[1],
                columns[2] + m.columns[2], columns[3] + m.columns[3]};
    }

    constexpr mat4 operator-(const mat4 &m) const {
        return {columns[0] - m.columns[0], columns[1] - m.columns[1],
                columns[2] - m.columns[2], columns[3] - m.columns[3]};
    }

    constexpr mat4 operator*(float scalar) const {
        return {columns[0] * scalar, columns[1] * scalar,
                columns[2] * scalar, columns[3] * scalar};
    }

    constexpr mat4 operator/(float scalar) const {
        return {columns[0] / scalar, columns[1] / scalar,
                columns[2] / scalar, columns[3] / scalar};
    }

    constexpr mat4 operator*(const mat4 &m) const {
        // multiply this matrix by another matrix
        mat4 result(0.0f);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.columns[i] += columns[j] * m.columns[i][j];
            }
        }
        return result;
    }

    constexpr vec4 &operator[](int index) {
        return columns[index];
    }

    constexpr const vec4 &operator[](int index) const {
        return columns[index];
    }

    constexpr mat4 inverse() const {
        float coef_00 = columns[2][2] * columns[3][3] - columns[3][2] * columns[2][3];
        float coef_02 = columns[1][2] * columns[3][3] - columns[3][2] * columns[1][3];
        float coef_03 = columns[1][2] * columns[2][3] - columns[2][2] * columns[1][3];
        float coef_04 = columns[2][1] * columns[3][3] - columns[3][1] * columns[2][3];
        float coef_06 = columns[1][1] * columns[3][3] - columns[3][1] * columns[1][3];
        float coef_07 = columns[1][1] * columns[2][3] - columns[2][1] * columns[1][3];
        float coef_08 = columns[2][0] * columns[3][3] - columns[3][0] * columns[2][3];
        float coef_10 = columns[1][0] * columns[3][3] - columns[3][0] * columns[1][3];
        float coef_11 = columns[1][0] * columns[2][3] - columns[2][0] * columns[1][3];
        float coef_12 = columns[2][0] * columns[3][2] - columns[3][0] * columns[2][2];
        float coef_14 = columns[1][0] * columns[3][2] - columns[3][0] * columns[1][2];
        float coef_15 = columns[1][0] * columns[2][2] - columns[2][0] * columns[1][2];
        float coef_16 = columns[2][1] * columns[3][2] - columns[3][1] * columns[2][2];
        float coef_18 = columns[1][1] * columns[3][2] - columns[3][1] * columns[1][2];
        float coef_19 = columns[1][1] * columns[2][2] - columns[2][1] * columns[1][2];
        float coef_20 = columns[2][0] * columns[3][1] - columns[3][0] * columns[2][1];
        float coef_22 = columns[1][0] * columns[3][1] - columns[3][0] * columns[1][1];
        float coef_23 = columns[1][0] * columns[2][1] - columns[2][0] * columns[1][1];

        vec4 fac0{coef_00, coef_00, coef_02, coef_03};
        vec4 fac1{coef_04, coef_04, coef_06, coef_07};
        vec4 fac2{coef_08, coef_08, coef_10, coef_11};
        vec4 fac3{coef_12, coef_12, coef_14, coef_15};
        vec4 fac4{coef_16, coef_16, coef_18, coef_19};
        vec4 fac5{coef_20, coef_20, coef_22, coef_23};

        vec4 vec0{columns[1][0], columns[0][0], columns[0][0], columns[0][0]};
        vec4 vec1{columns[1][1], columns[0][1], columns[0][1], columns[0][1]};
        vec4 vec2{columns[1][2], columns[0][2], columns[0][2], columns[0][2]};
        vec4 vec3{columns[1][3], columns[0][3], columns[0][3], columns[0][3]};

        vec4 inv0 = vec1 * fac0 - vec2 * fac1 + vec3 * fac2;
        vec4 inv1 = vec0 * fac0 - vec2 * fac3 + vec3 * fac4;
        vec4 inv2 = vec0 * fac1 - vec1 * fac3 + vec3 * fac5;
        vec4 inv3 = vec0 * fac2 - vec1 * fac4 + vec2 * fac5;
        
        vec4 sign_a{1, -1, 1, -1};
        vec4 sign_b = -sign_a;

        mat4 result{
            inv0 * sign_a,
            inv1 * sign_b,
            inv2 * sign_a,
            inv3 * sign_b,
        };

        vec4 row{
            result[0][0],
            result[1][0],
            result[2][0],
            result[3][0],
        };

        vec4 dot0 = result[0] * row;
        float dot1 = dot0.x + dot0.y + dot0.z + dot0.w;

        return result / dot1;
    }

    constexpr mat4 transpose() const {
        mat4 result;

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
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

#endif // ALCHEMIST_MATH_MATRIX_MAT4_H
