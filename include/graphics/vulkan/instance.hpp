
#ifndef ALCHEMIST_GRAPHICS_VULKAN_INSTANCE_H
#define ALCHEMIST_GRAPHICS_VULKAN_INSTANCE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct InstanceCreateInfo {
    const char *application_name;
    const char *engine_name;
    uint32_t application_version;
    uint32_t engine_version;

    InstanceCreateInfo(const char *app_name, uint32_t app_version);
};

VkInstance create_instance(const InstanceCreateInfo &create_info,
                           VkDebugUtilsMessengerEXT *debug_messenger = nullptr);

#endif
