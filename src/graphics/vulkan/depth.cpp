
#include <algorithm>

#include "graphics/vulkan/depth.hpp"

DepthQuery::DepthQuery(const std::initializer_list<VkFormat> &candidate_formats,
                       VkImageTiling tiling, VkFormatFeatureFlags features)
    : tiling(tiling), features(features) {
    this->candidate_count = static_cast<uint32_t>(candidate_formats.size());
    this->candidate_formats = new VkFormat[this->candidate_count];

    std::copy(candidate_formats.begin(), candidate_formats.end(),
              this->candidate_formats);
}

VkFormat query_depth_format(VkPhysicalDevice physical_device,
                            const DepthQuery &query) {
    for (uint32_t i = 0; i < query.candidate_count; ++i) {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(
            physical_device, query.candidate_formats[i], &properties);

        if ((query.tiling == VK_IMAGE_TILING_OPTIMAL &&
             (properties.optimalTilingFeatures & query.features)) ||
            (query.tiling == VK_IMAGE_TILING_LINEAR &&
             (properties.linearTilingFeatures & query.features))) {
            return query.candidate_formats[i];
        }
    }

    return VK_FORMAT_UNDEFINED; // No suitable format found
}
