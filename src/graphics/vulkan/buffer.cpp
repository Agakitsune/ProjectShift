
#include "graphics/vulkan/buffer.hpp"

static uint32_t find_memory_type(VkPhysicalDevice physical_device,
                                 uint32_t type_filter,
                                 VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_properties);

    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
        if ((type_filter & (1 << i)) &&
            (mem_properties.memoryTypes[i].propertyFlags & properties) ==
                properties) {
            return i;
        }
    }

    return UINT32_MAX; // Memory type not found
}

VkDeviceMemory allocate_buffer_memory(VkPhysicalDevice physical,
                                      VkDevice device, VkBuffer buffer,
                                      VkMemoryPropertyFlags properties) {
    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(device, buffer, &mem_requirements);

    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = find_memory_type(
        physical, mem_requirements.memoryTypeBits,
        properties); // This should be set based on memory type properties

    VkDeviceMemory memory;
    if (vkAllocateMemory(device, &alloc_info, nullptr, &memory) != VK_SUCCESS) {
        return VK_NULL_HANDLE;
    }

    vkBindBufferMemory(device, buffer, memory, 0);
    return memory;
}

VkBuffer create_buffer(VkDevice device, uint64_t size, VkBufferUsageFlags usage,
                       VkSharingMode sharing_mode) {
    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = sharing_mode;

    VkBuffer buffer;
    if (vkCreateBuffer(device, &buffer_info, nullptr, &buffer) != VK_SUCCESS) {
        return VK_NULL_HANDLE; // Buffer creation failed
    }

    return buffer;
}

VkBuffer create_staging_buffer(VkDevice device, uint64_t size) {
    return create_buffer(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_SHARING_MODE_EXCLUSIVE);
}

VkBuffer create_local_vertex_buffer(VkDevice device, uint64_t size) {
    return create_buffer(device, size,
                         VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                             VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                         VK_SHARING_MODE_EXCLUSIVE);
}

void *map_memory(VkDevice device, VkDeviceMemory memory, VkDeviceSize size,
                 VkDeviceSize offset) {
    void *data;
    if (vkMapMemory(device, memory, offset, size, 0, &data) != VK_SUCCESS) {
        return nullptr; // Mapping failed
    }
    return data;
}

void unmap_memory(VkDevice device, VkDeviceMemory memory) {
    vkUnmapMemory(device, memory);
}
