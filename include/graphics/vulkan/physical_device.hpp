
#ifndef ALCHEMIST_GRAPHICS_VULKAN_PHYSICAL_DEVICE_H
#define ALCHEMIST_GRAPHICS_VULKAN_PHYSICAL_DEVICE_H

#include <vulkan/vulkan.h>

enum QueueFamilyRequirements {
    QUEUE_FAMILY_REQUIREMENTS_GRAPHICS = 0x1,
    QUEUE_FAMILY_REQUIREMENTS_COMPUTE = 0x2,
    QUEUE_FAMILY_REQUIREMENTS_TRANSFER = 0x4
};

enum QueueFamilyPreferences {
    QUEUE_FAMILY_PREFERENCES_SEPARATE =
        0, // Prefer Separate queues for graphics, compute, transfer, present
    QUEUE_FAMILY_PREFERENCES_COMBINED =
        1, // Prefer Combined queues for graphics, compute, transfer, present
};

struct FamilyIndices {
    uint32_t graphics; // Vulkan specs require a graphics queue
    uint32_t present; // Queue family that supports presentation, if graphics is
                      // required, present must also be supported

    uint32_t compute;  // Optional, for compute operations
    uint32_t transfer; // Optional, for transfer operations
};

struct PhysicalDevice {
    VkPhysicalDevice device; // Vulkan physical device handle
    FamilyIndices indices;   // Queue family indices for the device
    VkPhysicalDeviceProperties properties; // Properties of the physical device
    VkPhysicalDeviceFeatures
        features; // Features supported by the physical device
};

struct PhysicalDevicePickInfo {
    QueueFamilyRequirements requirements; // Required queue family capabilities
    QueueFamilyPreferences preference; // Preferred queue family configuration
};

void pick_physical_device(VkInstance instance, VkSurfaceKHR surface,
                          PhysicalDevice &device,
                          const PhysicalDevicePickInfo &pick_info);

#endif
