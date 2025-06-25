
#ifndef ALCHEMIST_GRAPHICS_COLOR_HPP
#define ALCHEMIST_GRAPHICS_COLOR_HPP

#include <cstdint>

#include "math/vector/vec3.hpp"
#include "math/vector/vec4.hpp"

struct color {
    float r, g, b, a;

    // Default constructor initializes to white color
    constexpr color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}

    // Constructor with parameters
    constexpr color(float red, float green, float blue, float alpha = 1.0f)
        : r(red), g(green), b(blue), a(alpha) {}

    constexpr color(const vec3 &v, float alpha = 1.0f)
        : r(v.x), g(v.y), b(v.z), a(alpha) {}
    constexpr color(const vec4 &v) : r(v.x), g(v.y), b(v.z), a(v.w) {}

    constexpr color(const color &c) : r(c.r), g(c.g), b(c.b), a(c.a) {}
    constexpr color(color &&c) noexcept : r(c.r), g(c.g), b(c.b), a(c.a) {}

    constexpr color(const color &c, float alpha)
        : r(c.r), g(c.g), b(c.b), a(alpha) {}

    static constexpr color from_hex(uint32_t hex) {
        return {static_cast<float>((hex >> 16) & 0xFF) / 255.0f,
                static_cast<float>((hex >> 8) & 0xFF) / 255.0f,
                static_cast<float>(hex & 0xFF) / 255.0f, 1.0f};
    }

    color &operator=(const color &c) {
        if (this != &c) {
            r = c.r;
            g = c.g;
            b = c.b;
            a = c.a;
        }
        return *this;
    }

    color &operator=(color &&c) noexcept {
        if (this != &c) {
            r = c.r;
            g = c.g;
            b = c.b;
            a = c.a;
        }
        return *this;
    }

    color operator+(const color &c) const {
        return {r + c.r, g + c.g, b + c.b, a + c.a};
    }

    color operator-(const color &c) const {
        return {r - c.r, g - c.g, b - c.b, a - c.a};
    }

    color operator*(float scalar) const {
        return {r * scalar, g * scalar, b * scalar, a * scalar};
    }

    color operator/(float scalar) const {
        return {r / scalar, g / scalar, b / scalar, a / scalar};
    }

    color &operator+=(const color &c) {
        r += c.r;
        g += c.g;
        b += c.b;
        a += c.a;
        return *this;
    }

    color &operator-=(const color &c) {
        r -= c.r;
        g -= c.g;
        b -= c.b;
        a -= c.a;
        return *this;
    }

    color &operator*=(float scalar) {
        r *= scalar;
        g *= scalar;
        b *= scalar;
        a *= scalar;
        return *this;
    }

    color &operator/=(float scalar) {
        r /= scalar;
        g /= scalar;
        b /= scalar;
        a /= scalar;
        return *this;
    }

    bool operator==(const color &c) const {
        return r == c.r && g == c.g && b == c.b && a == c.a;
    }

    bool operator!=(const color &c) const { return !(*this == c); }
};

constexpr color WHITE(1.0f, 1.0f, 1.0f);
constexpr color BLACK(0.0f, 0.0f, 0.0f);
constexpr color GRAY(0.5f, 0.5f, 0.5f);
constexpr color TRANSPARENT(BLACK, 0.0f);

constexpr color RED(1.0f, 0.0f, 0.0f);
constexpr color GREEN(0.0f, 1.0f, 0.0f);
constexpr color BLUE(0.0f, 0.0f, 1.0f);

constexpr color YELLOW(1.0f, 1.0f, 0.0f);
constexpr color CYAN(0.0f, 1.0f, 1.0f);
constexpr color MAGENTA(1.0f, 0.0f, 1.0f);

#endif // ALCHEMIST_GRAPHICS_COLOR_HPP
