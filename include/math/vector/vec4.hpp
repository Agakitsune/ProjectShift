
#ifndef ALCHEMIST_MATH_VECTOR_VEC4_H
#define ALCHEMIST_MATH_VECTOR_VEC4_H

#include <cmath>

struct vec2;
struct vec3;

struct vec4 {
    float x;
    float y;
    float z;
    float w;

    constexpr vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    constexpr vec4(float s) : x(s), y(s), z(s), w(s) {}
    constexpr vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    constexpr vec4(const vec4 &v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
    constexpr vec4(vec4 &&v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

    constexpr vec4(const vec2 &v, float z = 0.0f, float w = 0.0f);
    constexpr vec4(const vec3 &v, float w = 0.0f);

    constexpr vec4 &operator=(const vec4 &v) {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
        return *this;
    }

    constexpr vec4 &operator=(vec4 &&v) {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
        return *this;
    }

    constexpr vec4 operator+(const vec4 &v) const {
        return {x + v.x, y + v.y, z + v.z, w + v.w};
    }

    constexpr vec4 operator-(const vec4 &v) const {
        return {x - v.x, y - v.y, z - v.z, w - v.w};
    }

    constexpr vec4 operator*(float scalar) const {
        return {x * scalar, y * scalar, z * scalar, w * scalar};
    }

    constexpr vec4 operator/(float scalar) const {
        return {x / scalar, y / scalar, z / scalar, w / scalar};
    }

    constexpr vec4 operator*(const vec4 &v) const {
        return {x * v.x, y * v.y, z * v.z, w * v.w};
    }

    constexpr vec4 &operator+=(const vec4 &v) {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
        return *this;
    }

    constexpr vec4 &operator-=(const vec4 &v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
        return *this;
    }

    constexpr vec4 &operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }

    constexpr vec4 &operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        w /= scalar;
        return *this;
    }

    constexpr bool operator==(const vec4 &v) const {
        return x == v.x && y == v.y && z == v.z && w == v.w;
    }

    constexpr bool operator!=(const vec4 &v) const {
        return !(*this == v);
    }

    constexpr bool operator<(const vec4 &v) const {
        return x < v.x || (x == v.x && (y < v.y || (y == v.y && (z < v.z || (z == v.z && w < v.w)))));
    }

    constexpr bool operator>(const vec4 &v) const {
        return x > v.x || (x == v.x && (y > v.y || (y == v.y && (z > v.z || (z == v.z && w > v.w)))));
    }

    constexpr bool operator<=(const vec4 &v) const {
        return x <= v.x && y <= v.y && z <= v.z && w <= v.w;
    }

    constexpr bool operator>=(const vec4 &v) const {
        return x >= v.x && y >= v.y && z >= v.z && w >= v.w;
    }

    constexpr float &operator[](int index) {
        return (&x)[index];
    }

    constexpr const float &operator[](int index) const {
        return (&x)[index];
    }

    constexpr vec4 operator-() const {
        return {-x, -y, -z, -w};
    }

    constexpr vec4 operator+() const {
        return {x, y, z, w};
    }

    constexpr float length() const {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }

    constexpr float length_squared() const {
        return x * x + y * y + z * z + w * w;
    }

    constexpr vec4 normalize() const {
        float len = length();
        if (len == 0.0f) {
            return {0.0f, 0.0f, 0.0f, 0.0f};
        }
        return {x / len, y / len, z / len, w / len};
    }

    constexpr float dot(const vec4 &v) const {
        return x * v.x + y * v.y + z * v.z + w * v.w;
    }

    constexpr vec4 min(const vec4 &v) const {
        return {std::fmin(x, v.x), std::fmin(y, v.y), std::fmin(z, v.z), std::fmin(w, v.w)};
    }

    constexpr vec4 max(const vec4 &v) const {
        return {std::fmax(x, v.x), std::fmax(y, v.y), std::fmax(z, v.z), std::fmax(w, v.w)};
    }

    constexpr vec4 abs() const {
        return {std::fabs(x), std::fabs(y), std::fabs(z), std::fabs(w)};
    }

    constexpr float *data() {
        return &x;
    }

    constexpr const float *data() const {
        return &x;
    }
};

#include "math/vector/vec2.hpp"
#include "math/vector/vec3.hpp"

constexpr vec4::vec4(const vec2 &v, float z, float w) : x(v.x), y(v.y), z(z), w(w) {}
constexpr vec4::vec4(const vec3 &v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}

#endif // ALCHEMIST_MATH_VECTOR_VEC4_H
