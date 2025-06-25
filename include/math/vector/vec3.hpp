
#ifndef ALCHEMIST_MATH_VECTOR_VEC3_H
#define ALCHEMIST_MATH_VECTOR_VEC3_H

#include <cmath>

struct vec2;
struct vec4;

struct vec3 {
    float x;
    float y;
    float z;

    constexpr vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    constexpr vec3(float s) : x(s), y(s), z(s) {}
    constexpr vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    constexpr vec3(const vec3 &v) : x(v.x), y(v.y), z(v.z) {}
    constexpr vec3(vec3 &&v) : x(v.x), y(v.y), z(v.z) {}

    constexpr vec3(const vec2 &v, float z = 0.0f);
    constexpr vec3(const vec4 &v);

    constexpr vec3 &operator=(const vec3 &v) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    constexpr vec3 &operator=(vec3 &&v) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    constexpr vec3 operator+(const vec3 &v) const {
        return {x + v.x, y + v.y, z + v.z};
    }

    constexpr vec3 operator-(const vec3 &v) const {
        return {x - v.x, y - v.y, z - v.z};
    }

    constexpr vec3 operator*(float scalar) const {
        return {x * scalar, y * scalar, z * scalar};
    }

    constexpr vec3 operator/(float scalar) const {
        return {x / scalar, y / scalar, z / scalar};
    }

    constexpr vec3 &operator+=(const vec3 &v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    constexpr vec3 &operator-=(const vec3 &v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    constexpr vec3 &operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    constexpr vec3 &operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    constexpr bool operator==(const vec3 &v) const {
        return x == v.x && y == v.y && z == v.z;
    }

    constexpr bool operator!=(const vec3 &v) const {
        return !(*this == v);
    }

    constexpr bool operator<(const vec3 &v) const {
        return x < v.x || (x == v.x && (y < v.y || (y == v.y && z < v.z)));
    }

    constexpr bool operator>(const vec3 &v) const {
        return x > v.x || (x == v.x && (y > v.y || (y == v.y && z > v.z)));
    }

    constexpr bool operator<=(const vec3 &v) const {
        return x <= v.x && y <= v.y && z <= v.z;
    }

    constexpr bool operator>=(const vec3 &v) const {
        return x >= v.x && y >= v.y && z >= v.z;
    }

    constexpr float &operator[](int index) {
        return (&x)[index];
    }

    constexpr const float &operator[](int index) const {
        return (&x)[index];
    }

    constexpr vec3 operator-() const {
        return {-x, -y, -z};
    }

    constexpr vec3 operator+() const {
        return {x, y, z};
    }

    float length() const {
        return sqrtf(x * x + y * y + z * z);
    }

    constexpr float length_squared() const {
        return x * x + y * y + z * z;
    }

    vec3 normalize() const {
        float len = length();
        if (len == 0.0f) {
            return {0.0f, 0.0f, 0.0f};
        }
        return {x / len, y / len, z / len};
    }

    constexpr float dot(const vec3 &v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    constexpr vec3 cross(const vec3 &v) const {
        return {y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x};
    }

    constexpr vec3 min(const vec3 &v) const {
        return {std::fmin(x, v.x), std::fmin(y, v.y), std::fmin(z, v.z)};
    }

    constexpr vec3 max(const vec3 &v) const {
        return {std::fmax(x, v.x), std::fmax(y, v.y), std::fmax(z, v.z)};
    }

    constexpr vec3 abs() const {
        return {std::fabs(x), std::fabs(y), std::fabs(z)};
    }

    constexpr vec3 reflect(const vec3 &normal) const {
        return *this - normal * (2.0f * dot(normal));
    }

    constexpr vec3 refract(const vec3 &normal, float eta) const {
        float cos_theta = -dot(normal);
        float sin_theta2 = eta * eta * (1.0f - cos_theta * cos_theta);
        if (sin_theta2 > 1.0f) {
            return {0.0f, 0.0f, 0.0f}; // Total internal reflection
        }
        float cos_phi = sqrtf(1.0f - sin_theta2);
        return (*this) * eta + normal * (eta * cos_theta - cos_phi);
    }

    constexpr vec3 project(const vec3 &normal) const {
        return *this - normal * dot(normal);
    }

    constexpr float *data() {
        return &x;
    }

    constexpr const float *data() const {
        return &x;
    }
};

#include "math/vector/vec2.hpp"
#include "math/vector/vec4.hpp"

constexpr vec3::vec3(const vec2 &v, float z) : x(v.x), y(v.y), z(z) {}
constexpr vec3::vec3(const vec4 &v) : x(v.x), y(v.y), z(v.z) {}

#endif // ALCHEMIST_MATH_VECTOR_VEC3_H
