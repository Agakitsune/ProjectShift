
#include <cstring>

#include "graphics/images/image_server.hpp"

#include "memory/gpu.hpp"
#include "memory/rid.hpp"
#include "memory/misc.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

RID ImageBuilder::build() const {
    if (create_info.format == VK_FORMAT_UNDEFINED) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Image format must be set before building!" << std::endl;
    #endif
        return UINT32_MAX;
    }

    return server.new_image(create_info); // Create the image using the server
}



Image::~Image() {
    get_rid_server().free(get_rid_atlas().IMAGE, rid);
    get_rid_server().free(get_rid_atlas().MEMORY, memory_rid);
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
    vkDestroyBuffer(image_server->device, staging, nullptr); // Clean up the staging buffer
    if (memory_rid != RID_INVALID) {
        get_gpu_memory_server().free_block(memory_rid); // Free the memory block associated with the staging buffer
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

    memory_rid = get_gpu_memory_server().allocate_block<VkBuffer>(
        size,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        find_memory_type(physical_device, mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    );
    get_gpu_memory_server().bind(memory_rid, mem_requirements, staging);
    

    // Copy data to the staging buffer
    void *mapped_data;
    get_gpu_memory_server().map(memory_rid, &mapped_data);
    memcpy(mapped_data, data, size);
    get_gpu_memory_server().unmap(memory_rid);

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



ImageServer::ImageServer(const RenderingDevice &device) : device(device.device), physical_device(device.physical_device) {}
ImageServer::~ImageServer() {
    for (const auto &image : images) {
        vkDestroyImage(device, image.image, nullptr); // Clean up each image
    }
    images.clear(); // Clear the images vector
}

RID ImageServer::new_image(const VkImageCreateInfo &create_info) {
    VkImage image;
    VkDeviceSize size = create_info.extent.width * create_info.extent.height;
    RID rid = new_id(get_rid_atlas().IMAGE);

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

    // RID bind_rid = get_gpu_memory_server().bind(device, memory, mem_requirements, image);

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
    RID rid = new_id(get_rid_atlas().IMAGE);

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

    // RID bind_rid = get_gpu_memory_server().bind(device, memory, mem_requirements, image);

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

ImageBuilder ImageServer::image_builder() {
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
            RID bind_rid = get_gpu_memory_server().bind(memory, mem_requirements, img.image);

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

            img.memory_rid = get_gpu_memory_server().find_best<VkImage>(mem_requirements, flags);

            if (img.memory_rid == RID_INVALID) {
                #ifdef ALCHEMIST_DEBUG
                std::cerr << "Failed to find suitable memory for image with RID: " << image << std::endl;
                #endif
                return; // Return if no suitable memory is found
            }
            
            RID bind_rid = get_gpu_memory_server().bind(img.memory_rid, mem_requirements, img.image);

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
    command_types.emplace_back(TRANSITION_LAYOUT);
    return transition_commands.data()[transition_commands.size() - 1];
}

CmdUploadImage &ImageServer::upload_image(RID rid) {
    const Image &image = get_image(rid);

    CmdUploadImage cmd(image.image);

    upload_commands.emplace_back(std::move(cmd));
    command_types.emplace_back(UPLOAD);
    return upload_commands[upload_commands.size() - 1];
}

void ImageServer::execute_commands(VkCommandBuffer cmd_buffer) {
    uint32_t transition_index = 0;
    uint32_t upload_index = 0;

    for (const auto &command : command_types) {
        if (command == TRANSITION_LAYOUT) {
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
        } else if (command == UPLOAD) {
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

ImageServer &new_image_server(const RenderingDevice &device) {
    if (image_server) {
        delete image_server; // Clean up existing instance
    }
    image_server = new ImageServer(device);
    return *image_server;
}

ImageServer &get_image_server() {
    return *image_server; // Return the existing image server instance
}

RID load_image(const char *path) {
    int width, height, channels;
    stbi_uc* pixels = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
    VkDeviceSize image_size = width * height * channels;
    RID rid;
    ImageServer &server = get_image_server();

    if (!pixels) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to load image: " << path << std::endl;
    #endif
        return 0; // Return 0 if the image loading fails
    }

    rid = server.image_builder()
        .set_format(channels == 4 ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R8G8B8_UNORM)
        .set_size(width, height)
        .set_usage(VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        .set_tiling(VK_IMAGE_TILING_OPTIMAL)
        .set_mip_levels(1)
        .set_array_layers(1)
        .set_samples(VK_SAMPLE_COUNT_1_BIT)
        .set_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
        .build();
    
    server.transition_image_layout(rid)
        .set_old_layout(VK_IMAGE_LAYOUT_UNDEFINED)
        .set_new_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        .set_src_access_mask(0)
        .set_dst_access_mask(VK_ACCESS_TRANSFER_WRITE_BIT)
        .set_src_stage_mask(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT)
        .set_dst_stage_mask(VK_PIPELINE_STAGE_TRANSFER_BIT)
        .set_aspect_mask(VK_IMAGE_ASPECT_COLOR_BIT);
    server.upload_image(rid)
        .upload_data(image_server->device, image_server->physical_device, image_size, pixels)
        .set_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        .set_extent({static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1});
    server.transition_image_layout(rid)
        .set_old_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        .set_new_layout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        .set_src_access_mask(VK_ACCESS_TRANSFER_WRITE_BIT)
        .set_dst_access_mask(VK_ACCESS_SHADER_READ_BIT)
        .set_src_stage_mask(VK_PIPELINE_STAGE_TRANSFER_BIT)
        .set_dst_stage_mask(VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT)
        .set_aspect_mask(VK_IMAGE_ASPECT_COLOR_BIT);
    
    stbi_image_free(pixels); // Free the loaded image data
    
    return rid;
}

ImageServer *image_server = nullptr;
