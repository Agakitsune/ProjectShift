
#ifndef ALCHEMIST_GRAPHICS_VULKAN_DEPTH_H
#define ALCHEMIST_GRAPHICS_VULKAN_DEPTH_H

#include <initializer_list>

#include <vulkan/vulkan.h>

struct DepthQuery {
    VkFormat *candidate_formats;
    uint32_t candidate_count;

    VkImageTiling tiling;
    VkFormatFeatureFlags features;

    DepthQuery(const std::initializer_list<VkFormat> &candidate_formats,
               VkImageTiling tiling, VkFormatFeatureFlags features);
};

VkFormat query_depth_format(VkPhysicalDevice physical_device,
                            const DepthQuery &query);

#endif // ALCHEMIST_GRAPHICS_VULKAN_DEPTH_H
