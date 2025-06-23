
#include <cstring>

#include "server/image.hpp"

#include "server/gpu_memory.hpp"
#include "server/rid.hpp"
#include "memory/misc.hpp"

// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"

ImageBuilder::ImageBuilder(ImageServer &server) : server(server) {
    create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    create_info.imageType = VK_IMAGE_TYPE_2D; // Default to 2D image type
    create_info.extent = {1, 1, 1}; // Default size
    create_info.format = VK_FORMAT_UNDEFINED; // Default format
    create_info.mipLevels = 1; // Default mip levels
    create_info.arrayLayers = 1; // Default array layers
    create_info.samples = VK_SAMPLE_COUNT_1_BIT; // Default sample count
    create_info.tiling = VK_IMAGE_TILING_OPTIMAL; // Default tiling
    create_info.usage = VK_IMAGE_USAGE_SAMPLED_BIT; // Default usage
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Default sharing mode
}

ImageBuilder &ImageBuilder::set_image_type(VkImageType type) {
    create_info.imageType = type;
    return *this;
}

ImageBuilder &ImageBuilder::set_size(uint32_t width, uint32_t height, uint32_t depth) {
    create_info.extent.width = width;
    create_info.extent.height = height;
    create_info.extent.depth = depth;
    return *this;
}

ImageBuilder &ImageBuilder::set_size(VkExtent2D extent, uint32_t depth) {
    create_info.extent.width = extent.width;
    create_info.extent.height = extent.height;
    create_info.extent.depth = depth;
    return *this;
}

ImageBuilder &ImageBuilder::set_size(VkExtent3D extent) {
    create_info.extent = extent;
    return *this;
}

ImageBuilder &ImageBuilder::set_format(VkFormat format) {
    create_info.format = format;
    return *this;
}

ImageBuilder &ImageBuilder::set_mip_levels(uint32_t mip_levels) {
    create_info.mipLevels = mip_levels;
    return *this;
}

ImageBuilder &ImageBuilder::set_array_layers(uint32_t array_layers) {
    create_info.arrayLayers = array_layers;
    return *this;
}

ImageBuilder &ImageBuilder::set_samples(VkSampleCountFlagBits samples) {
    create_info.samples = samples;
    return *this;
}

ImageBuilder &ImageBuilder::set_usage(VkImageUsageFlags usage) {
    create_info.usage = usage;
    return *this;
}

ImageBuilder &ImageBuilder::set_tiling(VkImageTiling tiling) {
    create_info.tiling = tiling;
    return *this;
}

ImageBuilder &ImageBuilder::set_sharing_mode(VkSharingMode sharing_mode) {
    create_info.sharingMode = sharing_mode;
    return *this;
}

ImageBuilder &ImageBuilder::set_aspect(VkImageAspectFlags aspect_flags) {
    // Set the aspect flags in the create info
    create_info.flags = aspect_flags; // Note: This is not a standard field, but for demonstration purposes
    return *this;
}

RID ImageBuilder::build() const {
    if (create_info.format == VK_FORMAT_UNDEFINED) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Image format must be set before building!" << std::endl;
    #endif
        return UINT32_MAX;
    }

    return server.new_image(create_info); // Create the image using the server
}



ImageViewBuilder::ImageViewBuilder(ImageViewServer &server) : server(server) {
    create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = VK_NULL_HANDLE; // Default to no image
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D; // Default to 2D view type
    create_info.format = VK_FORMAT_UNDEFINED; // Default format
    create_info.subresourceRange = {}; // Initialize subresource range to default values
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; // Default aspect mask
    create_info.subresourceRange.levelCount = 1; // Default to 1 mip level
    create_info.subresourceRange.layerCount = 1; // Default to 1 layer
}

ImageViewBuilder &ImageViewBuilder::set_image(VkImage image) {
    create_info.image = image;
    return *this;
}

ImageViewBuilder &ImageViewBuilder::set_image(RID image) {
    const Image &img = ImageServer::instance().get_image(image);
    create_info.image = img.image; // Set the image from the RID
    return *this;
}

ImageViewBuilder &ImageViewBuilder::set_view_type(VkImageViewType view_type) {
    create_info.viewType = view_type;
    return *this;
}

ImageViewBuilder &ImageViewBuilder::set_format(VkFormat format) {
    create_info.format = format;
    return *this;
}

ImageViewBuilder &ImageViewBuilder::set_aspect_mask(VkImageAspectFlags aspect_mask) {
    create_info.subresourceRange.aspectMask = aspect_mask;
    // Set the aspect mask in the subresource range
    create_info.subresourceRange.baseMipLevel = 0; // Default to base mip level
    create_info.subresourceRange.levelCount = 1; // Default to 1 mip level
    create_info.subresourceRange.baseArrayLayer = 0; // Default to base array layer
    create_info.subresourceRange.layerCount = 1; // Default to 1 layer
    return *this;
}

ImageViewBuilder &ImageViewBuilder::set_components(VkComponentMapping components) {
    create_info.components = components;
    // // Set the component mapping for the image view
    // // Default to identity mapping
    // create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    // create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    // create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    // create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    return *this;
}

ImageViewBuilder &ImageViewBuilder::set_subresource_range(VkImageSubresourceRange subresource_range) {
    create_info.subresourceRange = subresource_range;
    // // Set the subresource range for the image view
    // // Default to full range
    // create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; // Default aspect mask
    // create_info.subresourceRange.baseMipLevel = 0; // Default to base mip level
    // create_info.subresourceRange.levelCount = 1; // Default to 1 mip level
    // create_info.subresourceRange.baseArrayLayer = 0; // Default to base array layer
    // create_info.subresourceRange.layerCount = 1; // Default to 1 layer
    return *this;
}

RID ImageViewBuilder::build() const {
    if (create_info.image == VK_NULL_HANDLE) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Image must be set before building ImageView!" << std::endl;
    #endif
        return UINT32_MAX; // Invalid RID
    }

    if (create_info.format == VK_FORMAT_UNDEFINED) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Image format must be set before building ImageView!" << std::endl;
    #endif
        return UINT32_MAX; // Invalid RID
    }

    return server.new_image_view(create_info); // Create the image view using the server
}



SamplerBuilder::SamplerBuilder(SamplerServer &server) : server(server) {
    create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    create_info.magFilter = VK_FILTER_LINEAR; // Default mag filter
    create_info.minFilter = VK_FILTER_LINEAR; // Default min filter
    create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT; // Default address mode U
    create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT; // Default address mode V
    create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT; // Default address mode W
    create_info.anisotropyEnable = VK_FALSE; // Default anisotropy disabled
    create_info.maxAnisotropy = 1.0f; // Default max anisotropy
    create_info.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK; // Default border color
    create_info.unnormalizedCoordinates = VK_FALSE; // Default unnormalized coordinates disabled
    create_info.compareEnable = VK_FALSE; // Default compare enable disabled
    create_info.compareOp = VK_COMPARE_OP_ALWAYS; // Default compare operation
    create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR; // Default mipmap mode
    create_info.mipLodBias = 0.0f; // Default mip LOD bias
    create_info.minLod = 0.0f; // Default minimum LOD
    create_info.maxLod = 0.0f; // Default maximum LOD
}

SamplerBuilder &SamplerBuilder::set_mag_filter(VkFilter filter) {
    create_info.magFilter = filter;
    return *this;
}

SamplerBuilder &SamplerBuilder::set_min_filter(VkFilter filter) {
    create_info.minFilter = filter;
    return *this;
}

SamplerBuilder &SamplerBuilder::set_address_mode_u(VkSamplerAddressMode mode) {
    create_info.addressModeU = mode;
    return *this;
}

SamplerBuilder &SamplerBuilder::set_address_mode_v(VkSamplerAddressMode mode) {
    create_info.addressModeV = mode;
    return *this;
}

SamplerBuilder &SamplerBuilder::set_address_mode_w(VkSamplerAddressMode mode) {
    create_info.addressModeW = mode;
    return *this;
}

SamplerBuilder &SamplerBuilder::set_anisotropy_enable(VkBool32 enable) {
    create_info.anisotropyEnable = enable;
    if (enable) {
        create_info.maxAnisotropy = 16.0f; // Default max anisotropy if enabled
    } else {
        create_info.maxAnisotropy = 1.0f; // Reset to 1.0 if anisotropy is disabled
    }
    return *this;
}

SamplerBuilder &SamplerBuilder::set_max_anisotropy(float max_anisotropy) {
    create_info.maxAnisotropy = max_anisotropy;
    if (max_anisotropy < 1.0f) {
        create_info.maxAnisotropy = 1.0f; // Ensure max anisotropy is at least 1.0
    }
    return *this;
}

SamplerBuilder &SamplerBuilder::set_border_color(VkBorderColor border_color) {
    create_info.borderColor = border_color;
    // Set the border color for the sampler
    // Default to transparent black if not set
    if (border_color == VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK) {
        create_info.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    }
    return *this;
}

SamplerBuilder &SamplerBuilder::set_unnormalized_coordinates(VkBool32 unnormalized_coordinates) {
    create_info.unnormalizedCoordinates = unnormalized_coordinates;
    // Set whether the sampler uses unnormalized coordinates
    // Default to false (normalized coordinates)
    if (unnormalized_coordinates) {
        create_info.unnormalizedCoordinates = VK_TRUE;
    } else {
        create_info.unnormalizedCoordinates = VK_FALSE;
    }
    return *this;
}

SamplerBuilder &SamplerBuilder::set_compare_enable(VkBool32 compare_enable) {
    create_info.compareEnable = compare_enable;
    // Set whether the sampler uses comparison
    // Default to false (no comparison)
    if (compare_enable) {
        create_info.compareOp = VK_COMPARE_OP_LESS; // Default compare operation if enabled
    } else {
        create_info.compareOp = VK_COMPARE_OP_ALWAYS; // Reset to always if disabled
    }
    return *this;
}

SamplerBuilder &SamplerBuilder::set_compare_op(VkCompareOp compare_op) {
    create_info.compareOp = compare_op;
    // Set the comparison operation for the sampler
    // Default to VK_COMPARE_OP_ALWAYS if not set
    if (compare_op == VK_COMPARE_OP_ALWAYS) {
        create_info.compareOp = VK_COMPARE_OP_ALWAYS;
    }
    return *this;
}

SamplerBuilder &SamplerBuilder::set_mipmap_mode(VkSamplerMipmapMode mipmap_mode) {
    create_info.mipmapMode = mipmap_mode;
    // Set the mipmap mode for the sampler
    // Default to VK_SAMPLER_MIPMAP_MODE_LINEAR if not set
    if (mipmap_mode == VK_SAMPLER_MIPMAP_MODE_LINEAR) {
        create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    } else {
        create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST; // Reset to nearest if not linear
    }
    return *this;
}

SamplerBuilder &SamplerBuilder::set_mip_lod_bias(float mip_lod_bias) {
    create_info.mipLodBias = mip_lod_bias;
    // Set the mip LOD bias for the sampler
    // Default to 0.0f if not set
    if (mip_lod_bias < 0.0f) {
        create_info.mipLodBias = 0.0f; // Ensure non-negative bias
    }
    return *this;
}

SamplerBuilder &SamplerBuilder::set_min_lod(float min_lod) {
    create_info.minLod = min_lod;
    // Set the minimum LOD for the sampler
    // Default to 0.0f if not set
    if (min_lod < 0.0f) {
        create_info.minLod = 0.0f; // Ensure non-negative minimum LOD
    }
    return *this;
}

SamplerBuilder &SamplerBuilder::set_max_lod(float max_lod) {
    create_info.maxLod = max_lod;
    // Set the maximum LOD for the sampler
    // Default to 0.0f if not set
    if (max_lod < 0.0f) {
        create_info.maxLod = 0.0f; // Ensure non-negative maximum LOD
    }
    return *this;
}

RID SamplerBuilder::build() const {
    return server.new_sampler(create_info); // Create the sampler using the server
}



Image::~Image() {
    RIDServer::instance().free(RIDServer::IMAGE, rid);
    RIDServer::instance().free(RIDServer::MEMORY, memory_rid);
}



CmdTransitionImageLayout::CmdTransitionImageLayout(VkImage image) : image(image) {
    barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Default old layout
    barrier.newLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Default new layout
    barrier.srcAccessMask = 0; // Default source access mask
    barrier.dstAccessMask = 0; // Default destination access mask
    src_stage_mask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; // Default source stage mask
    dst_stage_mask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT; // Default destination stage mask
}

CmdTransitionImageLayout &CmdTransitionImageLayout::set_old_layout(VkImageLayout layout) {
    barrier.oldLayout = layout;
    return *this;
}

CmdTransitionImageLayout &CmdTransitionImageLayout::set_new_layout(VkImageLayout layout) {
    barrier.newLayout = layout;
    return *this;
}

CmdTransitionImageLayout &CmdTransitionImageLayout::set_src_access_mask(VkAccessFlags mask) {
    barrier.srcAccessMask = mask;
    return *this;
}

CmdTransitionImageLayout &CmdTransitionImageLayout::set_dst_access_mask(VkAccessFlags mask) {
    barrier.dstAccessMask = mask;
    return *this;
}

CmdTransitionImageLayout &CmdTransitionImageLayout::set_src_stage_mask(VkPipelineStageFlags stage) {
    src_stage_mask = stage;
    // barrier.srcAccessMask = 0; // Reset source access mask to default
    // switch (stage) {
    //     case VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT:
    //         barrier.srcAccessMask = 0; // No access required at the top of the pipe
    //         break;
    //     case VK_PIPELINE_STAGE_VERTEX_INPUT_BIT:
    //         barrier.srcAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT; // Vertex input stage
    //         break;
    //     case VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT:
    //         barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT; // Fragment shader stage
    //         break;
    //     // Add more cases as needed for other stages
    //     default:
    //         break;
    // }
    return *this;
}

CmdTransitionImageLayout &CmdTransitionImageLayout::set_dst_stage_mask(VkPipelineStageFlags stage) {
    dst_stage_mask = stage;
    // barrier.dstAccessMask = 0; // Reset destination access mask to default
    // switch (stage) {
    //     case VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT:
    //         barrier.dstAccessMask = 0; // No access required at the bottom of the pipe
    //         break;
    //     case VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT:
    //         barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; // Color attachment output stage
    //         break;
    //     case VK_PIPELINE_STAGE_TRANSFER_BIT:
    //         barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; // Transfer stage
    //         break;
    //     // Add more cases as needed for other stages
    //     default:
    //         break;
    // }
    return *this;
}

CmdTransitionImageLayout &CmdTransitionImageLayout::set_aspect_mask(VkImageAspectFlags aspect_mask) {
    barrier.subresourceRange.aspectMask = aspect_mask;
    // Set the aspect mask in the barrier
    barrier.subresourceRange.baseMipLevel = 0; // Default to base mip level
    barrier.subresourceRange.levelCount = 1; // Default to 1 mip level
    barrier.subresourceRange.baseArrayLayer = 0; // Default to base array layer
    barrier.subresourceRange.layerCount = 1; // Default to 1 layer
    return *this;
}

CmdUploadImage::CmdUploadImage(VkImage image) : image(image) {
    staging = VK_NULL_HANDLE; // Default to no staging buffer
    layout = VK_IMAGE_LAYOUT_UNDEFINED; // Default image layout
    copy_region = {}; // Initialize copy region to default values
    copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; // Default aspect mask
    copy_region.imageSubresource.layerCount = 1; // Default layer count
}

CmdUploadImage::~CmdUploadImage() {
    vkDestroyBuffer(ImageServer::instance().device, staging, nullptr); // Clean up the staging buffer
    if (memory_rid != RID_INVALID) {
        GpuMemoryServer::instance().free_block(memory_rid); // Free the memory block associated with the staging buffer
    }
}

CmdUploadImage &CmdUploadImage::upload_data(VkDevice device, VkPhysicalDevice physical_device, VkDeviceSize size, const void *data) {
    RID rid;

    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT; // Staging buffer for transfer
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &buffer_info, nullptr, &staging) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create staging buffer!" << std::endl;
    #endif
        return *this;
    }

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(device, staging, &mem_requirements);

    memory_rid = GpuMemoryServer::instance().allocate_block<VkBuffer>(
        size,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        find_memory_type(physical_device, mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    );
    GpuMemoryServer::instance().bind(memory_rid, mem_requirements, staging);
    

    // Copy data to the staging buffer
    void *mapped_data;
    GpuMemoryServer::instance().map(memory_rid, &mapped_data);
    memcpy(mapped_data, data, size);
    GpuMemoryServer::instance().unmap(memory_rid);

    return *this; // Return the command for chaining
}

CmdUploadImage &CmdUploadImage::set_layout(VkImageLayout image_layout) {
    layout = image_layout;
    // Set the image layout in the copy region
    copy_region.imageSubresource.aspectMask = (image_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    return *this;
}

CmdUploadImage &CmdUploadImage::set_extent(VkExtent3D image_extent) {
    copy_region.imageExtent = image_extent;
    // Set the extent in the copy region
    copy_region.imageSubresource.layerCount = 1; // Default to 1 layer
    return *this;
}



ImageServer::ImageServer(VkDevice device, VkPhysicalDevice physical_device) : device(device), physical_device(physical_device) {}
ImageServer::~ImageServer() {
    for (const auto &image : images) {
        #ifdef ALCHEMIST_DEBUG
        std::cout << "Destroying image with RID: " << image.rid << std::endl;
        #endif
        vkDestroyImage(device, image.image, nullptr); // Clean up each image
    }
    images.clear(); // Clear the images vector
}

RID ImageServer::new_image(const VkImageCreateInfo &create_info) {
    VkImage image;
    VkDeviceSize size = create_info.extent.width * create_info.extent.height;
    RID rid = RIDServer::instance().new_id(RIDServer::IMAGE);

    if (create_info.format == VK_FORMAT_R8G8B8_UNORM) {
        size *= 3;
    } else if (create_info.format == VK_FORMAT_R8G8B8A8_UNORM) {
        size *= 4;
    } else {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Unsupported image format!" << std::endl;
        #endif
        return RID_INVALID;
    }

    if (vkCreateImage(device, &create_info, nullptr, &image) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create image!" << std::endl;
    #endif
        return RID_INVALID;
    }

    // VkMemoryRequirements mem_requirements;
    // vkGetImageMemoryRequirements(device, image, &mem_requirements);

    // RID bind_rid = GpuMemoryServer::instance().bind(device, memory, mem_requirements, image);

    // if (bind_rid == RID_INVALID) {
    //     vkDestroyImage(device, image, nullptr); // Clean up the image if binding fails
    //     #ifdef ALCHEMIST_DEBUG
    //     std::cerr << "Failed to bind image memory!" << std::endl;
    //     #endif
    //     return RID_INVALID; // Return 0 if the binding fails
    // }

    images.emplace_back(image, rid, 0); // Add the created image to the images vector

    return rid;
}

RID ImageServer::new_image(VkImageCreateInfo &&create_info) {
    VkImage image;
    VkDeviceSize size = create_info.extent.width * create_info.extent.height;
    RID rid = RIDServer::instance().new_id(RIDServer::IMAGE);

    if (create_info.format == VK_FORMAT_R8G8B8_UNORM) {
        size *= 3;
    } else if (create_info.format == VK_FORMAT_R8G8B8A8_UNORM) {
        size *= 4;
    } else {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Unsupported image format!" << std::endl;
        #endif
        return UINT32_MAX; // Return 0 if the format is unsupported
    }

    if (vkCreateImage(device, &create_info, nullptr, &image) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create image!" << std::endl;
    #endif
        return UINT32_MAX;
    }

    // VkMemoryRequirements mem_requirements;
    // vkGetImageMemoryRequirements(device, image, &mem_requirements);

    // RID bind_rid = GpuMemoryServer::instance().bind(device, memory, mem_requirements, image);

    // if (bind_rid == RID_INVALID) {
    //     vkDestroyImage(device, image, nullptr); // Clean up the image if binding fails
    //     #ifdef ALCHEMIST_DEBUG
    //     std::cerr << "Failed to bind image memory!" << std::endl;
    //     #endif
    //     return RID_INVALID; // Return 0 if the binding fails
    // }

    images.emplace_back(image, rid, 0); // Add the created image to the images vector

    return rid;
}

ImageBuilder ImageServer::new_image() {
    return ImageBuilder(*this); // Return an ImageBuilder instance for creating images
}

const Image &ImageServer::get_image(RID rid) const {
    for (const auto &image : images) {
        if (image.rid == rid) {
            return image; // Return the image if found
        }
    }
    #ifdef ALCHEMIST_DEBUG
    std::cerr << "Image with RID " << rid << " not found!" << std::endl;
    #endif
    return *images.end(); // Return the last image as a fallback (or could throw an exception)
}

void ImageServer::get_requirements(RID rid, VkMemoryRequirements &requirements) const {
    for (const auto &image : images) {
        if (image.rid == rid) {
            vkGetImageMemoryRequirements(device, image.image, &requirements);
            return; // Return the requirements if the image is found
        }
    }
    #ifdef ALCHEMIST_DEBUG
    std::cerr << "Image with RID " << rid << " not found for memory requirements!" << std::endl;
    #endif
}

void ImageServer::bind_image(RID image, RID memory) {
    VkMemoryRequirements mem_requirements;

    for (auto &img : images) {
        if (img.rid == image) {
            vkGetImageMemoryRequirements(device, img.image, &mem_requirements);
            img.memory_rid = memory; // Bind the memory to the image
            RID bind_rid = GpuMemoryServer::instance().bind(memory, mem_requirements, img.image);

            if (bind_rid == RID_INVALID) {
                #ifdef ALCHEMIST_DEBUG
                std::cerr << "Failed to bind image memory for RID: " << image << std::endl;
                #endif
                return; // Return if binding fails
            }
            #ifdef ALCHEMIST_DEBUG
            std::cout << "Image with RID " << image << " bound to memory with RID " << memory << std::endl;
            #endif

            return;
        }
    }

    #ifdef ALCHEMIST_DEBUG
    std::cerr << "Image with RID " << image << " not found for binding!" << std::endl;
    #endif
}

void ImageServer::bind_best(RID image, VkMemoryPropertyFlags flags) {
    VkMemoryRequirements mem_requirements;

    for (auto &img : images) {
        if (img.rid == image) {
            vkGetImageMemoryRequirements(device, img.image, &mem_requirements);

            img.memory_rid = GpuMemoryServer::instance().find_best<VkImage>(mem_requirements, flags);

            if (img.memory_rid == RID_INVALID) {
                #ifdef ALCHEMIST_DEBUG
                std::cerr << "Failed to find suitable memory for image with RID: " << image << std::endl;
                #endif
                return; // Return if no suitable memory is found
            }
            
            RID bind_rid = GpuMemoryServer::instance().bind(img.memory_rid, mem_requirements, img.image);

            if (bind_rid == RID_INVALID) {
                #ifdef ALCHEMIST_DEBUG
                std::cerr << "Failed to bind image memory for RID: " << image << std::endl;
                #endif
                return; // Return if binding fails
            }
            #ifdef ALCHEMIST_DEBUG
            std::cout << "Image with RID " << image << " bound to best memory with RID " << img.memory_rid << std::endl;
            #endif

            return;
        }
    }

    #ifdef ALCHEMIST_DEBUG
    std::cerr << "Image with RID " << image << " not found for binding!" << std::endl;
    #endif
}

CmdTransitionImageLayout &ImageServer::transition_image_layout(RID rid) {
    const Image &image = get_image(rid);
    
    CmdTransitionImageLayout cmd(image.image);

    transition_commands.emplace_back(std::move(cmd));
    command_types.emplace_back(ImageCommandType::TRANSITION_LAYOUT);
    return transition_commands.data()[transition_commands.size() - 1];
}

CmdUploadImage &ImageServer::upload_image(RID rid) {
    const Image &image = get_image(rid);

    CmdUploadImage cmd(image.image);

    upload_commands.emplace_back(std::move(cmd));
    command_types.emplace_back(ImageCommandType::UPLOAD);
    return upload_commands[upload_commands.size() - 1];
}

void ImageServer::execute_commands(VkCommandBuffer cmd_buffer) {
    uint32_t transition_index = 0;
    uint32_t upload_index = 0;

    for (const auto &command : command_types) {
        if (command == ImageCommandType::TRANSITION_LAYOUT) {
            const auto &data = transition_commands[transition_index];
            vkCmdPipelineBarrier(
                cmd_buffer,
                data.src_stage_mask,
                data.dst_stage_mask,
                0, // No flags
                0, nullptr, // No memory barriers
                0, nullptr, // No buffer barriers
                1, &data.barrier // Image barrier
            );
            transition_index++; // Move to the next transition command
        } else if (command == ImageCommandType::UPLOAD) {
            const auto &data = upload_commands[upload_index];
            vkCmdCopyBufferToImage(
                cmd_buffer,
                data.staging,
                data.image,
                data.layout,
                1, // One region
                &data.copy_region
            );
            upload_index++; // Move to the next upload command
        } else {
            #ifdef ALCHEMIST_DEBUG
            std::cerr << "Unknown command type encountered!" << std::endl;
            #endif
        }
    }
}

void ImageServer::clear_commands() {
    upload_commands.clear();
    transition_commands.clear();
    command_types.clear();
}

ImageServer &ImageServer::instance() {
    return *__instance; // Return the singleton instance of ImageServer
}

std::unique_ptr<ImageServer> ImageServer::__instance = nullptr; // Singleton instance of ImageServer



ImageViewServer::ImageViewServer(VkDevice device) : device(device) {
    // Initialize the ImageViewServer with the Vulkan device
}
ImageViewServer::~ImageViewServer() {
    #ifdef ALCHEMIST_DEBUG
    for (const auto &view : image_views) {
        std::cout << "Destroying image view with RID: " << view.rid << std::endl;
    }
    #endif
}

RID ImageViewServer::new_image_view(const VkImageViewCreateInfo &create_info) {
    VkImageView image_view;
    RID rid = RIDServer::instance().new_id(RIDServer::IMAGE_VIEW);

    if (vkCreateImageView(device, &create_info, nullptr, &image_view) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create image view!" << std::endl;
    #endif
        return UINT32_MAX; // Return 0 if the creation fails
    }

    image_views.emplace_back(image_view, rid); // Add the created image view to the image views vector

    return rid;
}

RID ImageViewServer::new_image_view(VkImageViewCreateInfo &&create_info) {
    VkImageView image_view;
    RID rid = RIDServer::instance().new_id(RIDServer::IMAGE_VIEW);

    if (vkCreateImageView(device, &create_info, nullptr, &image_view) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create image view!" << std::endl;
    #endif
        return UINT32_MAX; // Return 0 if the creation fails
    }

    image_views.emplace_back(image_view, rid); // Add the created image view to the image views vector

    return rid;
}

ImageViewBuilder ImageViewServer::new_image_view() {
    return ImageViewBuilder(*this); // Return an ImageViewBuilder instance for creating image views
}

const ImageView &ImageViewServer::get_image_view(RID rid) const {
    for (const auto &view : image_views) {
        if (view.rid == rid) {
            return view; // Return the image view if found
        }
    }
    #ifdef ALCHEMIST_DEBUG
    std::cerr << "ImageView with RID " << rid << " not found!" << std::endl;
    #endif
    return *image_views.end(); // Return the last image view as a fallback (or could throw an exception)
}

ImageViewServer &ImageViewServer::instance() {
    return *__instance; // Return the singleton instance of ImageViewServer
}

std::unique_ptr<ImageViewServer> ImageViewServer::__instance = nullptr; // Singleton instance of ImageViewServer



SamplerServer::SamplerServer(VkDevice device) : device(device) {
    // Initialize the SamplerServer with the Vulkan device
}

SamplerServer::~SamplerServer() {
    for (const auto &sampler : samplers) {
        #ifdef ALCHEMIST_DEBUG
        std::cout << "Destroying sampler with RID: " << sampler.rid << std::endl;
        #endif
        vkDestroySampler(device, sampler.sampler, nullptr); // Clean up each sampler
    }
    samplers.clear(); // Clear the samplers vector
}

RID SamplerServer::new_sampler(const VkSamplerCreateInfo &create_info) {
    VkSampler sampler;
    RID rid = RIDServer::instance().new_id(RIDServer::SAMPLER);

    if (vkCreateSampler(device, &create_info, nullptr, &sampler) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create sampler!" << std::endl;
    #endif
        return UINT32_MAX; // Return 0 if the creation fails
    }

    samplers.emplace_back(sampler, rid); // Add the created sampler to the samplers vector

    return rid;
}

RID SamplerServer::new_sampler(VkSamplerCreateInfo &&create_info) {
    VkSampler sampler;static 
    RID rid = RIDServer::instance().new_id(RIDServer::SAMPLER);

    if (vkCreateSampler(device, &create_info, nullptr, &sampler) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create sampler!" << std::endl;
    #endif
        return UINT32_MAX; // Return 0 if the creation fails
    }

    samplers.emplace_back(sampler, rid); // Add the created sampler to the samplers vector

    return rid;
}

SamplerBuilder SamplerServer::new_sampler() {
    return SamplerBuilder(*this); // Return a SamplerBuilder instance for creating samplers
}

const Sampler &SamplerServer::get_sampler(RID rid) const {
    for (const auto &sampler : samplers) {
        if (sampler.rid == rid) {
            return sampler; // Return the sampler if found
        }
    }
    #ifdef ALCHEMIST_DEBUG
    std::cerr << "Sampler with RID " << rid << " not found!" << std::endl;
    #endif
    return *samplers.end(); // Return the last sampler as a fallback (or could throw an exception)
}

SamplerServer &SamplerServer::instance() {
    return *__instance; // Return the singleton instance of SamplerServer
}

std::unique_ptr<SamplerServer> SamplerServer::__instance = nullptr; // Singleton instance of SamplerServer
