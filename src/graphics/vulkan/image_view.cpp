
#include "graphics/vulkan/image_view.hpp"

ImageViewCreateInfo::ImageViewCreateInfo()
    : flags(0), viewType(VK_IMAGE_VIEW_TYPE_2D),
      components{VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
                 VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
      subresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1} {}

VkImageView create_image_view(VkDevice device, VkImage image, VkFormat format,
                              const ImageViewCreateInfo &create_info) {
    VkImageViewCreateInfo view_info{};
    VkImageView image_view = VK_NULL_HANDLE;

    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = image;
    view_info.viewType = create_info.viewType;
    view_info.format = format;
    view_info.components = create_info.components;
    view_info.subresourceRange = create_info.subresourceRange;

    if (vkCreateImageView(device, &view_info, nullptr, &image_view) !=
        VK_SUCCESS) {
        return VK_NULL_HANDLE; // Failed to create image view
    }

    return image_view;
}
