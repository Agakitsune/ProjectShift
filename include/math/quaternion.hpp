
#ifndef ALCHEMIST_MATH_QUATERNION_H
#define ALCHEMIST_MATH_QUATERNION_H

#include "math/angle.hpp"
#include "math/vector/vec3.hpp"
#include "math/vector/vec4.hpp"

struct quaternion {
    float x;
    float y;
    float z;
    float w;

    constexpr quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    constexpr quaternion(float x, float y, float z, float w)
        : x(x), y(y), z(z), w(w) {}
    constexpr quaternion(const quaternion &q)
        : x(q.x), y(q.y), z(q.z), w(q.w) {}
    constexpr quaternion(quaternion &&q) : x(q.x), y(q.y), z(q.z), w(q.w) {}

    constexpr quaternion(const vec3 &v, float w)
        : x(v.x), y(v.y), z(v.z), w(w) {}
    constexpr quaternion(const vec4 &v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

    static quaternion from_axis(const vec3 &axis, float angle) {
        float half_angle = angle * 0.5f;
        float s = std::sinf(half_angle);
        return {axis.x * s, axis.y * s, axis.z * s, std::cosf(half_angle)};
    }

    static quaternion from_euler(float pitch, float yaw, float roll) {
        float half_pitch = pitch * 0.5f;
        float half_yaw = yaw * 0.5f;
        float half_roll = roll * 0.5f;

        float sin_pitch = std::sinf(half_pitch);
        float cos_pitch = std::cosf(half_pitch);
        float sin_yaw = std::sinf(half_yaw);
        float cos_yaw = std::cosf(half_yaw);
        float sin_roll = std::sinf(half_roll);
        float cos_roll = std::cosf(half_roll);

        return {cos_yaw * sin_pitch * cos_roll + sin_yaw * cos_pitch * sin_roll,
                sin_yaw * cos_pitch * cos_roll - cos_yaw * sin_pitch * sin_roll,
                cos_yaw * cos_pitch * sin_roll - sin_yaw * sin_pitch * cos_roll,
                cos_yaw * cos_pitch * cos_roll +
                    sin_yaw * sin_pitch * sin_roll};
    }

    static quaternion arc(const vec3 &a, const vec3 &b) {
        float cos_theta = a.dot(b);
        if (cos_theta < -1.0f || cos_theta > 1.0f) {
            return {0.0f, 0.0f, 0.0f, 1.0f}; // Invalid input
        }
        float angle = std::acos(cos_theta);
        vec3 axis = a.cross(b).normalize();
        return from_axis(axis, angle);
    }

    constexpr quaternion &operator=(const quaternion &q) {
        x = q.x;
        y = q.y;
        z = q.z;
        w = q.w;
        return *this;
    }

    constexpr quaternion &operator=(quaternion &&q) {
        x = q.x;
        y = q.y;
        z = q.z;
        w = q.w;
        return *this;
    }

    constexpr quaternion operator+(const quaternion &q) const {
        return {x + q.x, y + q.y, z + q.z, w + q.w};
    }

    constexpr quaternion operator-(const quaternion &q) const {
        return {x - q.x, y - q.y, z - q.z, w - q.w};
    }

    constexpr quaternion operator*(float scalar) const {
        return {x * scalar, y * scalar, z * scalar, w * scalar};
    }

    constexpr quaternion operator/(float scalar) const {
        return {x / scalar, y / scalar, z / scalar, w / scalar};
    }

    constexpr quaternion &operator+=(const quaternion &q) {
        x += q.x;
        y += q.y;
        z += q.z;
        w += q.w;
        return *this;
    }

    constexpr quaternion &operator-=(const quaternion &q) {
        x -= q.x;
        y -= q.y;
        z -= q.z;
        w -= q.w;
        return *this;
    }

    constexpr quaternion &operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }

    constexpr quaternion &operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        w /= scalar;
        return *this;
    }

    constexpr bool operator==(const quaternion &q) const {
        return x == q.x && y == q.y && z == q.z && w == q.w;
    }

    constexpr bool operator!=(const quaternion &q) const {
        return !(*this == q);
    }

    constexpr quaternion conjugate() const { return {-x, -y, -z, w}; }

    constexpr quaternion normalize() const {
        float len = std::sqrt(x * x + y * y + z * z + w * w);
        if (len == 0.0f) {
            return {0.0f, 0.0f, 0.0f, 1.0f};
        }
        return {x / len, y / len, z / len, w / len};
    }

    constexpr quaternion inverse() const {
        float len_squared = x * x + y * y + z * z + w * w;
        if (len_squared == 0.0f) {
            return {0.0f, 0.0f, 0.0f, 1.0f};
        }
        return conjugate() / len_squared;
    }

    constexpr vec3 rotate(const vec3 &v) const {
        quaternion qv = {v.x, v.y, v.z, 0.0f};
        quaternion t = (*this) * qv * conjugate();
        return {t.x, t.y, t.z};
    }

    constexpr quaternion operator*(const quaternion &q) const {
        return {w * q.x + x * q.w + y * q.z - z * q.y,
                w * q.y - x * q.z + y * q.w + z * q.x,
                w * q.z + x * q.y - y * q.x + z * q.w,
                w * q.w - x * q.x - y * q.y - z * q.z};
    }

    constexpr quaternion operator-() const { return {-x, -y, -z, -w}; }

    constexpr quaternion operator+() const { return {x, y, z, w}; }

    constexpr float length() const {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }

    constexpr float length_squared() const {
        return x * x + y * y + z * z + w * w;
    }

    constexpr float *data() { return &x; }

    constexpr const float *data() const { return &x; }
};

#endif // ALCHEMIST_MATH_QUATERNION_H
