
#ifndef ALCHEMIST_SERVER_IMAGE_HPP
#define ALCHEMIST_SERVER_IMAGE_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "graphics/rendering_device.hpp"

#include "server/rid.hpp"

struct ImageServer; // Forward declaration
struct ImageViewServer; // Forward declaration
struct SamplerServer; // Forward declaration

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
    ImageBuilder &set_aspect(VkImageAspectFlags aspect_flags);

    RID build() const;
};

struct ImageViewBuilder {
    VkImageViewCreateInfo create_info;

    ImageViewServer &server; // Reference to the ImageServer for building image views

    ImageViewBuilder(ImageViewServer &server);

    ImageViewBuilder &set_image(VkImage image);
    ImageViewBuilder &set_image(RID image);
    ImageViewBuilder &set_view_type(VkImageViewType view_type);
    ImageViewBuilder &set_format(VkFormat format);
    ImageViewBuilder &set_aspect_mask(VkImageAspectFlags aspect_mask);
    ImageViewBuilder &set_components(VkComponentMapping components);
    ImageViewBuilder &set_subresource_range(VkImageSubresourceRange subresource_range);

    RID build() const;
};

struct SamplerBuilder {
    VkSamplerCreateInfo create_info;

    SamplerServer &server; // Reference to the ImageServer for building samplers

    SamplerBuilder(SamplerServer &server);

    SamplerBuilder &set_mag_filter(VkFilter filter);
    SamplerBuilder &set_min_filter(VkFilter filter);
    SamplerBuilder &set_address_mode_u(VkSamplerAddressMode mode);
    SamplerBuilder &set_address_mode_v(VkSamplerAddressMode mode);
    SamplerBuilder &set_address_mode_w(VkSamplerAddressMode mode);
    SamplerBuilder &set_anisotropy_enable(VkBool32 enable);
    SamplerBuilder &set_max_anisotropy(float max_anisotropy);
    SamplerBuilder &set_border_color(VkBorderColor border_color);
    SamplerBuilder &set_unnormalized_coordinates(VkBool32 unnormalized_coordinates);
    SamplerBuilder &set_compare_enable(VkBool32 compare_enable);
    SamplerBuilder &set_compare_op(VkCompareOp compare_op);
    SamplerBuilder &set_mipmap_mode(VkSamplerMipmapMode mipmap_mode);
    SamplerBuilder &set_mip_lod_bias(float mip_lod_bias);
    SamplerBuilder &set_min_lod(float min_lod);
    SamplerBuilder &set_max_lod(float max_lod);

    RID build() const;
};

struct Image {
    VkImage image;
    RID rid = RID_INVALID; // Resource ID
    RID memory_rid = RID_INVALID; // Resource ID for the GPU memory block

    ~Image();
};

struct ImageView {
    VkImageView view;
    RID rid = RID_INVALID; // Resource ID for the image view
    RID image_rid = RID_INVALID; // Resource ID for the associated image
};

struct Sampler {
    VkSampler sampler;
    RID rid = RID_INVALID; // Resource ID for the image sampler
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

enum class ImageCommandType {
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

    ImageServer(VkDevice device, VkPhysicalDevice physical_device);
    ~ImageServer();

    RID new_image(const VkImageCreateInfo &create_info);
    RID new_image(VkImageCreateInfo &&create_info);
    
    ImageBuilder new_image();

    void bind_image(RID image, RID memory);
    void bind_best(RID image, VkMemoryPropertyFlags flags);

    const Image &get_image(RID rid) const;
    void get_requirements(RID rid, VkMemoryRequirements &requirements) const;

    CmdTransitionImageLayout &transition_image_layout(RID rid);
    CmdUploadImage &upload_image(RID rid);

    void execute_commands(VkCommandBuffer cmd_buffer);
    void clear_commands(); // Make sure to wait for the commands to finish before clearing, there may be dragons

    static ImageServer &instance();

    static std::unique_ptr<ImageServer> __instance; // Singleton instance of ImageServer
};

struct ImageViewServer {
    std::vector<ImageView> image_views; // Vector to hold image views

    VkDevice device; // Vulkan device

    ImageViewServer(VkDevice device);
    ~ImageViewServer();

    RID new_image_view(const VkImageViewCreateInfo &create_info);
    RID new_image_view(VkImageViewCreateInfo &&create_info);

    ImageViewBuilder new_image_view();

    const ImageView &get_image_view(RID rid) const;

    static ImageViewServer &instance();
    static std::unique_ptr<ImageViewServer> __instance; // Singleton instance of ImageViewServer
};

struct SamplerServer {
    std::vector<Sampler> samplers; // Vector to hold samplers

    VkDevice device; // Vulkan device

    SamplerServer(VkDevice device);
    ~SamplerServer();

    RID new_sampler(const VkSamplerCreateInfo &create_info);
    RID new_sampler(VkSamplerCreateInfo &&create_info);

    SamplerBuilder new_sampler();

    const Sampler &get_sampler(RID rid) const;

    static SamplerServer &instance();
    static std::unique_ptr<SamplerServer> __instance; // Singleton instance of SamplerServer
};

#endif // ALCHEMIST_SERVER_IMAGE_HPP
