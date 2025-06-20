
#ifndef ALCHEMIST_GRAPHICS_IMAGES_IMAGE_SERVER_HPP
#define ALCHEMIST_GRAPHICS_IMAGES_IMAGE_SERVER_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "graphics/rendering_device.hpp"

#include "memory/rid.hpp"

struct ImageServer; // Forward declaration

struct ImageBuilder {
    VkImageCreateInfo create_info;

    ImageServer &server; // Reference to the ImageServer for building images

    ImageBuilder(ImageServer &server);

    ImageBuilder &set_image_type(VkImageType type);
    ImageBuilder &set_size(uint32_t width, uint32_t height, uint32_t depth = 1);
    ImageBuilder &set_size(VkExtent2D extent, uint32_t depth = 1);
    ImageBuilder &set_size(VkExtent3D extent);
    ImageBuilder &set_format(VkFormat format);
    ImageBuilder &set_mip_levels(uint32_t mip_levels);
    ImageBuilder &set_array_layers(uint32_t array_layers);
    ImageBuilder &set_samples(VkSampleCountFlagBits samples);
    ImageBuilder &set_usage(VkImageUsageFlags usage);
    ImageBuilder &set_tiling(VkImageTiling tiling);
    ImageBuilder &set_sharing_mode(VkSharingMode sharing_mode);

    RID build() const;
};


struct Image {
    VkImage image;
    RID rid = 0; // Resource ID
    RID memory_rid = 0; // Resource ID for the GPU memory block

    ~Image();
};

struct CmdTransitionImageLayout {
    VkImage image;

    VkImageMemoryBarrier barrier;

    VkPipelineStageFlags src_stage_mask;
    VkPipelineStageFlags dst_stage_mask;

    CmdTransitionImageLayout() = default;
    CmdTransitionImageLayout(VkImage image);

    CmdTransitionImageLayout &set_old_layout(VkImageLayout layout);
    CmdTransitionImageLayout &set_new_layout(VkImageLayout layout);
    CmdTransitionImageLayout &set_src_access_mask(VkAccessFlags mask);
    CmdTransitionImageLayout &set_dst_access_mask(VkAccessFlags mask);
    CmdTransitionImageLayout &set_src_stage_mask(VkPipelineStageFlags stage);
    CmdTransitionImageLayout &set_dst_stage_mask(VkPipelineStageFlags stage);
    CmdTransitionImageLayout &set_aspect_mask(VkImageAspectFlags aspect_mask);
};

struct CmdUploadImage {
    VkImage image;
    VkBuffer staging;
    RID memory_rid = RID_INVALID;

    VkImageLayout layout;

    VkBufferImageCopy copy_region;

    CmdUploadImage() = default;
    CmdUploadImage(VkImage image);
    ~CmdUploadImage();

    CmdUploadImage &upload_data(VkDevice device, VkPhysicalDevice physical_device, VkDeviceSize size, const void *data);
    CmdUploadImage &set_layout(VkImageLayout image_layout);
    CmdUploadImage &set_extent(VkExtent3D image_extent);
};

enum ImageCommandType {
    TRANSITION_LAYOUT,
    UPLOAD
};

struct ImageServer {
    std::vector<Image> images; // Vector to hold device memory objects

    std::vector<CmdTransitionImageLayout> transition_commands; // Vector to hold transition commands
    std::vector<CmdUploadImage> upload_commands; // Vector to hold upload commands

    std::vector<ImageCommandType> command_types; // Vector to hold command types

    VkDevice device; // Vulkan device
    VkPhysicalDevice physical_device; // Vulkan physical device
    // VkCommandPool command_pool;

    ImageServer(const RenderingDevice &device);
    ~ImageServer();

    RID new_image(const VkImageCreateInfo &create_info);
    RID new_image(VkImageCreateInfo &&create_info);
    
    ImageBuilder image_builder();

    void bind_image(RID image, RID memory);
    void bind_best(RID image, VkMemoryPropertyFlags flags);

    const Image &get_image(RID rid) const;
    void get_requirements(RID rid, VkMemoryRequirements &requirements) const;

    CmdTransitionImageLayout &transition_image_layout(RID rid);
    CmdUploadImage &upload_image(RID rid);

    void execute_commands(VkCommandBuffer cmd_buffer);
    void clear_commands(); // Make sure to wait for the commands to finish before clearing, there may be dragons
};

ImageServer &new_image_server(const RenderingDevice &device);
ImageServer &get_image_server();

RID load_image(const char *path);

extern ImageServer *image_server; // Global image server instance

#endif // ALCHEMIST_GRAPHICS_IMAGES_IMAGE_SERVER_HPP
