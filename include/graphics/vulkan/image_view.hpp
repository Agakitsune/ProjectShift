
#ifndef ALCHEMIST_GRAPHICS_VULKAN_IMAGE_VIEW_H
#define ALCHEMIST_GRAPHICS_VULKAN_IMAGE_VIEW_H

#include <vulkan/vulkan.h>

struct ImageViewCreateInfo {
    VkImageViewCreateFlags flags;
    VkImageViewType viewType;
    VkComponentMapping components;
    VkImageSubresourceRange subresourceRange;

    ImageViewCreateInfo();

    static ImageViewCreateInfo aspect(VkImageAspectFlags aspect_flags) {
        ImageViewCreateInfo info;
        info.subresourceRange.aspectMask = aspect_flags;
        return info;
    }
};

VkImageView create_image_view(VkDevice device, VkImage image, VkFormat format,
                              const ImageViewCreateInfo &create_info);

#endif
