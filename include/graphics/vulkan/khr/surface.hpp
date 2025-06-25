
#ifndef ALCHEMIST_GRAPHICS_VULKAN_KHR_SURFACE_H
#define ALCHEMIST_GRAPHICS_VULKAN_KHR_SURFACE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

VkSurfaceKHR create_surface(GLFWwindow *window, VkInstance instance);

#endif
