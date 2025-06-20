
#ifndef ALCHEMIST_GRAPHICS_RENDERING_PASS_HPP
#define ALCHEMIST_GRAPHICS_RENDERING_PASS_HPP

#ifdef ALCHEMIST_DEBUG
#include <iostream>
#endif // ALCHEMIST_DEBUG

#include <vector>

#include <vulkan/vulkan.h>

#include "graphics/rendering_device.hpp"

struct RenderingPassBuilder;

struct AttachmentBuilder {
    VkFormat format;
    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
    VkAttachmentLoadOp load_op;
    VkAttachmentStoreOp store_op;
    // VkAttachmentLoadOp stencilLoadOp;
    // VkAttachmentStoreOp stencilStoreOp;
    VkImageLayout initial_layout;
    VkImageLayout final_layout;

    RenderingPassBuilder &rendering_pass;

    AttachmentBuilder(RenderingPassBuilder &pass);

    AttachmentBuilder &set_format(VkFormat fmt);
    AttachmentBuilder &set_samples(VkSampleCountFlagBits sample_count);
    AttachmentBuilder &set_load_op(VkAttachmentLoadOp op);
    AttachmentBuilder &set_store_op(VkAttachmentStoreOp op);
    AttachmentBuilder &set_initial_layout(VkImageLayout layout);
    AttachmentBuilder &set_final_layout(VkImageLayout layout);

    RenderingPassBuilder &build() const;
};

struct SubpassBuilder {
    std::vector<VkAttachmentReference> color_attachments;
    VkAttachmentReference depth_stencil_attachment = {VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED};

    VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;

    RenderingPassBuilder &rendering_pass;

    SubpassBuilder(RenderingPassBuilder &pass);
    
    SubpassBuilder &set_bind_point(VkPipelineBindPoint point);
    SubpassBuilder &add_color_attachment(uint32_t attachment, VkImageLayout layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    SubpassBuilder &set_depth_stencil_attachment(uint32_t attachment, VkImageLayout layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    RenderingPassBuilder &build();
};

struct SubpassDependecyBuilder {
    uint32_t                src_subpass;
    uint32_t                dst_subpass;
    VkPipelineStageFlags    src_stage_mask;
    VkPipelineStageFlags    dst_stage_mask;
    VkAccessFlags           src_access_mask;
    VkAccessFlags           dst_access_mask;
    VkDependencyFlags       dependency_flags = 0;

    RenderingPassBuilder &rendering_pass;

    SubpassDependecyBuilder(RenderingPassBuilder &pass);
    
    SubpassDependecyBuilder &set_src_subpass(uint32_t subpass);
    SubpassDependecyBuilder &set_dst_subpass(uint32_t subpass);
    SubpassDependecyBuilder &set_src_stage_mask(VkPipelineStageFlags mask);
    SubpassDependecyBuilder &set_dst_stage_mask(VkPipelineStageFlags mask);
    SubpassDependecyBuilder &set_src_access_mask(VkAccessFlags mask);
    SubpassDependecyBuilder &set_dst_access_mask(VkAccessFlags mask);

    RenderingPassBuilder &build() const;
};

struct RenderingPassBuilder {
    std::vector<VkAttachmentDescription> attachments;
    std::vector<VkSubpassDescription> subpasses;
    std::vector<VkSubpassDependency> dependencies;

    RenderingPassBuilder() = default;
    RenderingPassBuilder(uint32_t attachment_capacity, 
                         uint32_t subpass_capacity = 1, 
                         uint32_t dependency_capacity = 1);

    AttachmentBuilder new_attachment();
    SubpassBuilder new_subpass();
    SubpassDependecyBuilder new_dependency();

    VkRenderPass build(VkDevice device) const;
};

VkRenderPass default_render_pass(const RenderingDevice &device);

#ifdef ALCHEMIST_DEBUG
VkRenderPass imgui_render_pass(const RenderingDevice &device);
#endif // ALCHEMIST_DEBUG

#endif // ALCHEMIST_GRAPHICS_RENDERING_DEVICE_HPP
