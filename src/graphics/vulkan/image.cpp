
#include "graphics/vulkan/image.hpp"

ImageCreateInfo::ImageCreateInfo(uint32_t width, uint32_t height,
                                 uint32_t depth)
    : width(width), height(height), depth(depth) {
    // Constructor initializes the image dimensions
}

ImageCreateInfo &ImageCreateInfo::set_width(uint32_t width) {
    this->width = width;
    return *this;
}

ImageCreateInfo &ImageCreateInfo::set_height(uint32_t height) {
    this->height = height;
    return *this;
}

ImageCreateInfo &ImageCreateInfo::set_depth(uint32_t depth) {
    this->depth = depth;
    return *this;
}

ImageCreateInfo &ImageCreateInfo::set_size_2d(uint32_t width, uint32_t height) {
    this->width = width;
    this->height = height;
    return *this;
}

ImageCreateInfo &ImageCreateInfo::set_size_3d(uint32_t width, uint32_t height,
                                              uint32_t depth) {
    this->width = width;
    this->height = height;
    this->depth = depth;
    return *this;
}

ImageCreateInfo &ImageCreateInfo::set_format(VkFormat format) {
    this->format = format;
    return *this;
}

ImageCreateInfo &ImageCreateInfo::set_tiling(VkImageTiling tiling) {
    this->tiling = tiling;
    return *this;
}

ImageCreateInfo &
ImageCreateInfo::set_initial_layout(VkImageLayout initial_layout) {
    this->initial_layout = initial_layout;
    return *this;
}

ImageCreateInfo &ImageCreateInfo::set_usage(VkImageUsageFlags usage) {
    this->usage = usage;
    return *this;
}

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

VkDeviceMemory allocate_image_memory(VkPhysicalDevice physical, VkDevice device,
                                     VkImage image,
                                     VkMemoryPropertyFlags properties) {
    VkMemoryRequirements mem_requirements;
    vkGetImageMemoryRequirements(device, image, &mem_requirements);

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

    vkBindImageMemory(device, image, memory, 0);
    return memory;
}

VkImage create_image(VkDevice device, const ImageCreateInfo &create_info) {
    VkImageCreateInfo image_info = {};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = create_info.width;
    image_info.extent.height = create_info.height;
    image_info.extent.depth = create_info.depth;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.format = create_info.format;
    image_info.tiling = create_info.tiling;
    image_info.initialLayout = create_info.initial_layout;
    image_info.usage = create_info.usage;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkImage image;
    if (vkCreateImage(device, &image_info, nullptr, &image) != VK_SUCCESS) {
        return VK_NULL_HANDLE; // Creation failed
    }

    return image;
}
