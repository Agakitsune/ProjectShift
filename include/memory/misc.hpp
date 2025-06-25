
#ifndef ALCHEMIST_MEMORY_MISC_HPP
#define ALCHEMIST_MEMORY_MISC_HPP

#include <cstdint>

#include <vulkan/vulkan.h>

uint32_t find_memory_type(VkPhysicalDevice device, uint32_t type_filter, VkMemoryPropertyFlags properties);

#endif // ALCHEMIST_MEMORY_MISC_HPP