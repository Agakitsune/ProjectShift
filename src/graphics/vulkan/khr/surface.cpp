
#include "graphics/vulkan/khr/surface.hpp"

VkSurfaceKHR create_surface(GLFWwindow *window, VkInstance instance) {
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) !=
        VK_SUCCESS) {
        return VK_NULL_HANDLE; // Failed to create Vulkan surface
    }
    return surface;
}
