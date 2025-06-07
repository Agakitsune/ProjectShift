
#ifndef ALCHEMIST_MATH_VECTOR_VEC2_H
#define ALCHEMIST_MATH_VECTOR_VEC2_H

#include <cmath>

struct vec3;
struct vec4;

struct vec2 {
    float x;
    float y;

    constexpr vec2() : x(0.0f), y(0.0f) {}
    constexpr vec2(float s) : x(s), y(s) {}
    constexpr vec2(float x, float y) : x(x), y(y) {}
    constexpr vec2(const vec2 &v) : x(v.x), y(v.y) {}
    constexpr vec2(vec2 &&v) : x(v.x), y(v.y) {}
    
    constexpr vec2(const vec3 &v);
    constexpr vec2(const vec4 &v);
    
    constexpr vec2 &operator=(const vec2 &v) {
        x = v.x;
        y = v.y;
        return *this;
    }

    constexpr vec2 &operator=(vec2 &&v) {
        x = v.x;
        y = v.y;
        return *this;
    }

    constexpr vec2 operator+(const vec2 &v) const {
        return {x + v.x, y + v.y};
    }

    constexpr vec2 operator-(const vec2 &v) const {
        return {x - v.x, y - v.y};
    }

    constexpr vec2 operator*(float scalar) const {
        return {x * scalar, y * scalar};
    }

    constexpr vec2 operator/(float scalar) const {
        return {x / scalar, y / scalar};
    }

    constexpr vec2 &operator+=(const vec2 &v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    constexpr vec2 &operator-=(const vec2 &v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    constexpr vec2 &operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    constexpr vec2 &operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    constexpr bool operator==(const vec2 &v) const {
        return x == v.x && y == v.y;
    }

    constexpr bool operator!=(const vec2 &v) const {
        return !(*this == v);
    }

    constexpr bool operator<(const vec2 &v) const {
        return x < v.x || (x == v.x && y < v.y);
    }

    constexpr bool operator>(const vec2 &v) const {
        return x > v.x || (x == v.x && y > v.y);
    }

    constexpr bool operator<=(const vec2 &v) const {
        return x <= v.x && y <= v.y;
    }

    constexpr bool operator>=(const vec2 &v) const {
        return x >= v.x && y >= v.y;
    }

    constexpr float &operator[](int index) {
        return (&x)[index];
    }

    constexpr const float &operator[](int index) const {
        return (&x)[index];
    }

    constexpr vec2 operator-() const {
        return {-x, -y};
    }

    constexpr vec2 operator+() const {
        return {x, y};
    }

    constexpr float length() const {
        return sqrtf(x * x + y * y);
    }

    constexpr float length_squared() const {
        return x * x + y * y;
    }

    constexpr vec2 normalize() const {
        float len = length();
        if (len == 0.0f) {
            return {0.0f, 0.0f};
        }
        return {x / len, y / len};
    }

    constexpr float dot(const vec2 &v) const {
        return x * v.x + y * v.y;
    }

    constexpr vec2 min(const vec2 &v) const {
        return {fminf(x, v.x), fminf(y, v.y)};
    }

    constexpr vec2 max(const vec2 &v) const {
        return {fmaxf(x, v.x), fmaxf(y, v.y)};
    }

    constexpr vec2 abs() const {
        return {fabsf(x), fabsf(y)};
    }

    constexpr vec2 reflect(const vec2 &normal) const {
        float dot_product = dot(normal);
        return {x - 2.0f * dot_product * normal.x, y - 2.0f * dot_product * normal.y};
    }

    constexpr vec2 refract(const vec2 &normal, float eta) const {
        float dot_product = dot(normal);
        float k = 1.0f - eta * eta * (1.0f - dot_product * dot_product);
        if (k < 0.0f) {
            return {0.0f, 0.0f};
        } else {
            return {eta * x - (eta * dot_product + sqrtf(k)) * normal.x,
                    eta * y - (eta * dot_product + sqrtf(k)) * normal.y};
        }
    }

    constexpr vec2 project(const vec2 &b) const {
        float dot_product = dot(b);
        float length_squared = b.length_squared();
        if (length_squared == 0.0f) {
            return {0.0f, 0.0f};
        }
        return {(dot_product / length_squared) * b.x, (dot_product / length_squared) * b.y};
    }

    constexpr float *data() {
        return &x;
    }

    constexpr const float *data() const {
        return &x;
    }
};

#include "math/vector/vec3.hpp"
#include "math/vector/vec4.hpp"

constexpr vec2::vec2(const vec3 &v) : x(v.x), y(v.y) {}
constexpr vec2::vec2(const vec4 &v) : x(v.x), y(v.y) {}

#endif
