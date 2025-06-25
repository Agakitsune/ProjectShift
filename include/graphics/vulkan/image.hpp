
#ifndef ALCHEMIST_GRAPHICS_VULKAN_IMAGE_HPP
#define ALCHEMIST_GRAPHICS_VULKAN_IMAGE_HPP

#include <vulkan/vulkan.h>

struct ImageCreateInfo {
    uint32_t width = 0; // Width of the image
    uint32_t height =
        1;              // Height of the image (for 1D images, this is always 1)
    uint32_t depth = 1; // Depth of the image (for 2D images, this is always 1)

    VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;      // Format of the image
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL; // Tiling of the image
    VkImageLayout initial_layout =
        VK_IMAGE_LAYOUT_UNDEFINED; // Initial layout of the image
    VkImageUsageFlags usage =
        VK_IMAGE_USAGE_SAMPLED_BIT; // Usage flags for the image

    ImageCreateInfo() = default;
    ImageCreateInfo(uint32_t width, uint32_t height = 1, uint32_t depth = 1);

    ImageCreateInfo &set_width(uint32_t width);
    ImageCreateInfo &set_height(uint32_t height);
    ImageCreateInfo &set_depth(uint32_t depth);
    ImageCreateInfo &set_size_2d(uint32_t width, uint32_t height);
    ImageCreateInfo &set_size_3d(uint32_t width, uint32_t height,
                                 uint32_t depth);

    ImageCreateInfo &set_format(VkFormat format);
    ImageCreateInfo &set_tiling(VkImageTiling tiling);
    ImageCreateInfo &set_initial_layout(VkImageLayout initial_layout);
    ImageCreateInfo &set_usage(VkImageUsageFlags usage);
};

VkDeviceMemory allocate_image_memory(VkPhysicalDevice physical, VkDevice device,
                                     VkImage image,
                                     VkMemoryPropertyFlags properties);

VkImage create_image(VkDevice device, const ImageCreateInfo &create_info);

#endif // ALCHEMIST_GRAPHICS_VULKAN_IMAGE_HPP
