
#ifndef ALCHEMIST_GRAPHICS_VULKAN_VERTEX_INPUT_HPP
#define ALCHEMIST_GRAPHICS_VULKAN_VERTEX_INPUT_HPP

#include <vulkan/vulkan.h>

#include "math/vector/vec2.hpp"
#include "math/vector/vec3.hpp"
#include "math/vector/vec4.hpp"

#include "math/quaternion.hpp"

template <typename T> struct VertexFormat {
    static constexpr VkFormat format =
        VK_FORMAT_UNDEFINED; // Default format, should be specialized
};

template <> struct VertexFormat<float> {
    static constexpr VkFormat format = VK_FORMAT_R32_SFLOAT;
};

template <> struct VertexFormat<int> {
    static constexpr VkFormat format = VK_FORMAT_R32_SINT;
};

template <> struct VertexFormat<uint32_t> {
    static constexpr VkFormat format = VK_FORMAT_R32_UINT;
};

template <> struct VertexFormat<vec2> {
    static constexpr VkFormat format = VK_FORMAT_R32G32_SFLOAT;
};

template <> struct VertexFormat<vec3> {
    static constexpr VkFormat format = VK_FORMAT_R32G32B32_SFLOAT;
};

template <> struct VertexFormat<vec4> {
    static constexpr VkFormat format = VK_FORMAT_R32G32B32A32_SFLOAT;
};

template <> struct VertexFormat<quaternion> {
    static constexpr VkFormat format = VK_FORMAT_R32G32B32A32_SFLOAT;
};

struct VertexInput {
    VkVertexInputBindingDescription *bindings = nullptr;
    VkVertexInputAttributeDescription *attributes = nullptr;

    uint32_t binding_count = 0;
    uint32_t attribute_count = 0;

    VertexInput() = default;

    VertexInput &
    add_binding(uint32_t binding, uint32_t stride,
                VkVertexInputRate input_rate = VK_VERTEX_INPUT_RATE_VERTEX) {
        binding_count++;
        VkVertexInputBindingDescription *new_bindings =
            (VkVertexInputBindingDescription *)realloc(
                bindings,
                binding_count * sizeof(VkVertexInputBindingDescription));
        if (!new_bindings) {
            // Handle memory allocation failure
            return *this;
        }
        bindings = new_bindings;
        bindings[binding_count - 1] = {binding, stride, input_rate};

        return *this;
    }

    template <typename T>
    VertexInput &add_attribute(uint32_t location, uint32_t binding,
                               uint32_t offset) {
        attribute_count++;
        VkVertexInputAttributeDescription *new_attributes =
            (VkVertexInputAttributeDescription *)realloc(
                attributes,
                attribute_count * sizeof(VkVertexInputAttributeDescription));
        if (!new_attributes) {
            // Handle memory allocation failure
            return *this;
        }
        attributes = new_attributes;
        attributes[attribute_count - 1] = {location, binding,
                                           VertexFormat<T>::format, offset};

        return *this;
    }
};

#endif // ALCHEMIST_GRAPHICS_VULKAN_VERTEX_INPUT_HPP
