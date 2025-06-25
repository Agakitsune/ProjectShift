
#ifndef ALCHEMIST_GRAPHICS_RENDERING_DEVICE_HPP
#define ALCHEMIST_GRAPHICS_RENDERING_DEVICE_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

enum QueueFamilyPreferences {
    QUEUE_FAMILY_PREFERENCES_SEPARATE =
        0, // Prefer Separate queues for graphics, compute, transfer, present
    QUEUE_FAMILY_PREFERENCES_COMBINED =
        1, // Prefer Combined queues for graphics, compute, transfer, present
};

struct Indices {
    uint32_t graphics = UINT32_MAX; // Graphics queue family index
    uint32_t present = UINT32_MAX;  // Presentation queue family index
    uint32_t compute = UINT32_MAX;  // Compute queue family index
    uint32_t transfer = UINT32_MAX; // Transfer queue family index
};

struct ApplicationInfo {
    GLFWwindow *window; // GLFW window handle

    const char *application_name;
    const char *engine_name;
    uint32_t application_version;
    uint32_t engine_version;

    QueueFamilyPreferences
        queue_family_preferences; // Preferences for queue families
};

struct RenderingDevice {
    VkInstance instance;

    #ifdef ALCHEMIST_DEBUG
    VkDebugUtilsMessengerEXT messenger; // Debug messenger for validation layers
    #endif
    
    VkPhysicalDevice physical_device;
    uint32_t graphics_queue_family_index;
    uint32_t compute_queue_family_index;
    uint32_t transfer_queue_family_index;
    uint32_t present_queue_family_index;
    
    VkDevice device = VK_NULL_HANDLE; // Vulkan logical device handle

    VkFormat depth_format;

    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    VkSurfaceFormatKHR surface_format;
    VkPresentModeKHR present_mode;
    VkExtent2D swapchain_extent;

    uint32_t swapchain_image_count;

    VkImage *swapchain_images;
    VkImageView *swapchain_image_views;

    RenderingDevice() = default;
    RenderingDevice(const ApplicationInfo &info);
    RenderingDevice(RenderingDevice &&other);

    RenderingDevice &operator=(RenderingDevice &&other);

    ~RenderingDevice();
};

// RenderingDevice &new_rendering_device(const ApplicationInfo &info);
// RenderingDevice &get_rendering_device();

// extern RenderingDevice *rendering_device;

#endif // ALCHEMIST_GRAPHICS_RENDERING_DEVICE_HPP
