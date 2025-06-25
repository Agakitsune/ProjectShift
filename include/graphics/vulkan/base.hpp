
#ifndef GRAPHICS_VULKAN_BASE_H
#define GRAPHICS_VULKAN_BASE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "graphics/vulkan/khr/swapchain.hpp"
#include "graphics/vulkan/physical_device.hpp"

// Struct to hold Vulkan base data that is common
struct VulkanBase {
    VkInstance instance;
    VkSurfaceKHR surface;

#ifdef ALCHEMIST_DEBUG
    VkDebugUtilsMessengerEXT messenger; // debug messenger
#endif

    PhysicalDevice physical_device; // Physical device information

    VkDevice device; // logical device

    Swapchain swapchain; // Swapchain information

    VkFormat depth_format;

    VkImage *swapchain_images;
    VkImageView *swapchain_image_views;
};

struct VulkanBaseCreateInfo {
    const char *application_name;
    const char *engine_name;
    uint32_t application_version;
    uint32_t engine_version;

    QueueFamilyPreferences
        queue_family_preferences; // Preferences for queue families
};

int vulkan_base_init(VulkanBase &base, GLFWwindow *window,
                     const VulkanBaseCreateInfo &create_info);

#endif // GRAPHICS_VULKAN_BASE_H
