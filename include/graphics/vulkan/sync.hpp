
#ifndef ALCHEMIST_GRAPHICS_VULKAN_FENCE_HPP
#define ALCHEMIST_GRAPHICS_VULKAN_FENCE_HPP

#include <vulkan/vulkan.h>

VkFence create_fence(VkDevice device);

VkSemaphore create_semaphore(VkDevice device);

#endif // ALCHEMIST_GRAPHICS_VULKAN_FENCE_HPP
