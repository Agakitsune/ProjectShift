
#ifndef ALCHEMIST_GRAPHICS_VULKAN_QUEUE_HPP
#define ALCHEMIST_GRAPHICS_VULKAN_QUEUE_HPP

#include <vulkan/vulkan.h>

VkQueue create_queue(VkDevice device, uint32_t queue_family_index,
                     uint32_t queue_index = 0);

#endif // ALCHEMIST_GRAPHICS_VULKAN_QUEUE_HPP
