
#ifndef ALCHEMY_GRAPHICS_VULKAN_BUFFER_HPP
#define ALCHEMY_GRAPHICS_VULKAN_BUFFER_HPP

#include <vulkan/vulkan.h>

VkDeviceMemory allocate_buffer_memory(VkPhysicalDevice physical,
                                      VkDevice device, VkBuffer buffer,
                                      VkMemoryPropertyFlags properties);

VkBuffer create_buffer(VkDevice device, uint64_t size, VkBufferUsageFlags usage,
                       VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE);

VkBuffer create_staging_buffer(VkDevice device, uint64_t size);
VkBuffer create_local_vertex_buffer(VkDevice device, uint64_t size);

void *map_memory(VkDevice device, VkDeviceMemory memory, VkDeviceSize size,
                 VkDeviceSize offset = 0);
void unmap_memory(VkDevice device, VkDeviceMemory memory);

#endif // ALCHEMY_GRAPHICS_VULKAN_BUFFER_HPP
