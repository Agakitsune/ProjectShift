
#ifndef ALCHEMIST_GRAPHICS_VULKAN_RENDER_PASS_HPP
#define ALCHEMIST_GRAPHICS_VULKAN_RENDER_PASS_HPP

#include <vulkan/vulkan.h>

enum AttachmentType {
    ATTACHMENT_TYPE_COLOR,
    ATTACHMENT_TYPE_DEPTH_STENCIL,
    ATTACHMENT_TYPE_INPUT,
    ATTACHMENT_TYPE_RESOLVE,
};

struct AttachmentDescription {
    VkAttachmentDescriptionFlags flags;
    VkFormat format;
    VkSampleCountFlagBits samples;
    VkAttachmentLoadOp loadOp;
    VkAttachmentStoreOp storeOp;
    VkAttachmentLoadOp stencilLoadOp;
    VkAttachmentStoreOp stencilStoreOp;
    VkImageLayout initialLayout;
    VkImageLayout finalLayout;

    VkImageLayout referenceLayout;

    AttachmentType type;

    AttachmentDescription(AttachmentType type, VkFormat format,
                          VkImageLayout initialLayout,
                          VkImageLayout finalLayout, VkImageLayout reference);
    
    AttachmentDescription &set_samples(VkSampleCountFlagBits samples);
    AttachmentDescription &set_load_op(VkAttachmentLoadOp load_op);
    AttachmentDescription &set_store_op(VkAttachmentStoreOp store_op);
    AttachmentDescription &set_stencil_load_op(VkAttachmentLoadOp load_op);
    AttachmentDescription &set_stencil_store_op(VkAttachmentStoreOp store_op);
};

struct SubpassDescription {
    VkPipelineBindPoint bindPoint;

    uint32_t *inputAttachments;
    uint32_t *colorAttachments;
    uint32_t resolveAttachments;
    uint32_t depthStencilAttachment;

    uint32_t colorAttachmentCount;
    uint32_t inputAttachmentCount;

    // uint32_t            preserveAttachmentCount;
    // uint32_t *preserveAttachments;

    SubpassDescription(VkPipelineBindPoint bindPoint);

    SubpassDescription &add_input_attachment(uint32_t input_attachment);
    SubpassDescription &add_color_attachment(uint32_t color_attachment);
    SubpassDescription &
    set_depth_stencil_attachment(uint32_t depth_stencil_attachment);
    SubpassDescription &set_resolve_attachment(uint32_t resolve_attachment);
};

struct RenderPassInfo {
    AttachmentDescription *attachments = nullptr;
    SubpassDescription *descriptions = nullptr;
    VkSubpassDependency *dependencies = nullptr;

    uint32_t attachment_count = 0;
    uint32_t description_count = 0;
    uint32_t dependency_count = 0;

    RenderPassInfo() = default;

    RenderPassInfo &add_attachment(const AttachmentDescription &description);
    RenderPassInfo &add_attachment(AttachmentDescription &&description);

    RenderPassInfo &
    add_subpass_description(const SubpassDescription &description);
    RenderPassInfo &add_subpass_description(SubpassDescription &&description);

    RenderPassInfo &
    add_subpass_dependecy(const VkSubpassDependency &dependency);
    RenderPassInfo &add_subpass_dependecy(VkSubpassDependency &&dependency);
};

VkRenderPass create_render_pass(VkDevice device, const RenderPassInfo &info);

#endif // ALCHEMIST_GRAPHICS_VULKAN_RENDER_PASS_HPP
