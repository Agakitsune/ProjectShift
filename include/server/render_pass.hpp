
#ifndef ALCHEMIST_SERVER_RENDER_PASS_HPP
#define ALCHEMIST_SERVER_RENDER_PASS_HPP

#include <vulkan/vulkan.h>

#include "server/rid.hpp"
#include "vulkan/command_buffer.hpp"
#include "graphics/color.hpp"

struct RenderPassBegin {
    VkCommandBuffer cmd_buffer = VK_NULL_HANDLE; // Command buffer to begin the render pass on
    VkRenderPassBeginInfo begin_info = {}; // Render pass begin info structure
    std::vector<VkClearValue> clear_values; // Clear values for the attachments

    RenderPassBegin() = default;
    RenderPassBegin(VkCommandBuffer buffer);

    RenderPassBegin &set_render_pass(RID pass);
    RenderPassBegin &set_framebuffer(RID fb);
    RenderPassBegin &set_render_offset(VkOffset2D area);
    RenderPassBegin &set_render_size(VkExtent2D area);
    RenderPassBegin &add_clear_color(const color &value);
    RenderPassBegin &add_clear_depth(float value); 

    void begin();
    void end();
};

struct RenderPass {
    VkRenderPass render_pass; // Vulkan render pass object
    RID rid = RID_INVALID; // Resource ID for the render pass

    RenderPass() = default;

    RenderPassBegin begin(VkCommandBuffer cmd_buffer) const;
};

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

struct RenderPassServer;

struct RenderingPassBuilder {
    std::vector<VkAttachmentDescription> attachments;
    std::vector<VkSubpassDescription> subpasses;
    std::vector<VkSubpassDependency> dependencies;

    RenderPassServer &server;

    RenderingPassBuilder(RenderPassServer &server);
    RenderingPassBuilder(RenderPassServer &server, 
                         uint32_t attachment_capacity, 
                         uint32_t subpass_capacity, 
                         uint32_t dependency_capacity);

    AttachmentBuilder new_attachment();
    SubpassBuilder new_subpass();
    SubpassDependecyBuilder new_dependency();

    RID build() const;
};

struct RenderPassServer {
    std::vector<RenderPass> render_passes; // Vector to hold all buffers

    VkDevice device; // Vulkan device

    RenderPassServer(VkDevice device);
    ~RenderPassServer();

    RID new_render_pass(const VkRenderPassCreateInfo &create_info);
    RID new_render_pass(VkRenderPassCreateInfo &&create_info);

    RenderingPassBuilder new_render_pass();
    RenderingPassBuilder new_render_pass(
        uint32_t attachment_capacity, 
        uint32_t subpass_capacity, 
        uint32_t dependency_capacity);
    
    const RenderPass &get_render_pass(RID rid) const;

    static RenderPassServer &instance();

    static std::unique_ptr<RenderPassServer> __instance; // Singleton instance of BufferServer
};

RID default_render_pass(VkFormat image_format, VkFormat depth_format);

#ifdef ALCHEMIST_DEBUG
RID imgui_render_pass(VkFormat image_format);
#endif // ALCHEMIST_DEBUG

#endif // ALCHEMIST_SERVER_RENDER_PASS_HPP
