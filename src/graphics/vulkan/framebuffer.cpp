
#include "graphics/vulkan/framebuffer.hpp"

FramebufferCreateInfo::FramebufferCreateInfo(VkRenderPass render_pass,
                                             uint32_t width, uint32_t height,
                                             uint32_t layers)
    : render_pass(render_pass), width(width), height(height), layers(layers) {}

FramebufferCreateInfo &
FramebufferCreateInfo::reserve_attachments(uint32_t count) {
    attachments.reserve(count);
    return *this;
}

FramebufferCreateInfo &
FramebufferCreateInfo::add_attachment(VkImageView attachment) {
    attachments.push(attachment);
    return *this;
}

FramebufferCreateInfo &
FramebufferCreateInfo::set_attachment(VkImageView attachment, uint32_t index) {
    attachments.data[index] = attachment;
    return *this;
}

VkFramebuffer create_framebuffer(VkDevice device,
                                 const FramebufferCreateInfo &create_info) {
    VkFramebufferCreateInfo framebuffer_info = {};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = create_info.render_pass;
    framebuffer_info.attachmentCount = create_info.attachments.size;
    framebuffer_info.pAttachments = create_info.attachments.data;
    framebuffer_info.width = create_info.width;
    framebuffer_info.height = create_info.height;
    framebuffer_info.layers = create_info.layers;

    VkFramebuffer framebuffer;
    if (vkCreateFramebuffer(device, &framebuffer_info, nullptr, &framebuffer) !=
        VK_SUCCESS) {
        return VK_NULL_HANDLE; // Creation failed
    }

    return framebuffer;
}

void create_framebuffers(VkDevice device,
                         const FramebufferCreateInfo &create_info,
                         VkFramebuffer *framebuffers, uint32_t count) {
    VkFramebufferCreateInfo framebuffer_info = {};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = create_info.render_pass;
    framebuffer_info.attachmentCount = create_info.attachments.size;
    framebuffer_info.pAttachments = create_info.attachments.data;
    framebuffer_info.width = create_info.width;
    framebuffer_info.height = create_info.height;
    framebuffer_info.layers = create_info.layers;

    for (uint32_t i = 0; i < count; ++i) {
        if (vkCreateFramebuffer(device, &framebuffer_info, nullptr,
                                &framebuffers[i]) != VK_SUCCESS) {
            framebuffers[i] = VK_NULL_HANDLE; // Creation failed
        }
    }
}
