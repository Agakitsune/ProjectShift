
#ifndef ALCHEMIST_GRAPHICS_VULKAN_KHR_SWAPCHAIN_H
#define ALCHEMIST_GRAPHICS_VULKAN_KHR_SWAPCHAIN_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "graphics/vulkan/physical_device.hpp"

struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities; // Surface capabilities

    VkSurfaceFormatKHR *formats;     // Available surface formats
    VkPresentModeKHR *present_modes; // Available present modes

    uint32_t format_count;       // Number of available surface formats
    uint32_t present_mode_count; // Number of available present modes
};

struct Swapchain {
    VkSwapchainKHR swapchain; // Swapchain handle
    VkSurfaceFormatKHR format;
    VkPresentModeKHR present_mode; // Present mode for the swapchain
    VkExtent2D extent;             // Extent of the swapchain

    uint32_t image_count; // Number of images in the swapchain
};

void create_swapchain(Swapchain &swapchain, GLFWwindow *window, VkDevice device,
                      VkPhysicalDevice physical_device, VkSurfaceKHR surface,
                      const FamilyIndices &indices);

#endif // ALCHEMIST_GRAPHICS_VULKAN_KHR_SWAPCHAIN_H
