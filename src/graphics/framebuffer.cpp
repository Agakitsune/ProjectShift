
#ifdef ALCHEMIST_DEBUG
#include <iostream>
#endif // ALCHEMIST_DEBUG

#include "graphics/framebuffer.hpp"

FramebufferBuilder::FramebufferBuilder(uint32_t width, uint32_t height, uint32_t layers) :
    render_pass(VK_NULL_HANDLE), width(width), height(height), layers(layers) {}

FramebufferBuilder::FramebufferBuilder(VkExtent2D extent, uint32_t layers) :
    render_pass(VK_NULL_HANDLE), width(extent.width), height(extent.height), layers(layers) {}

FramebufferBuilder &FramebufferBuilder::set_render_pass(VkRenderPass render_pass) {
    this->render_pass = render_pass;
    return *this;
}

FramebufferBuilder &FramebufferBuilder::add_attachment(VkImageView attachment) {
    attachments.push_back(attachment);
    return *this;
}

FramebufferBuilder &FramebufferBuilder::set_attachment(VkImageView attachment, uint32_t index) {
    attachments[index] = attachment;
    return *this;
}

VkFramebuffer FramebufferBuilder::build(VkDevice device) const {
    if (render_pass == VK_NULL_HANDLE) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Render pass must be set before building framebuffer." << std::endl;
    #endif
        return VK_NULL_HANDLE;
    }
    if (attachments.empty()) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "At least one attachment must be added before building framebuffer." << std::endl;
    #endif
        return VK_NULL_HANDLE;
    }

    VkFramebufferCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    create_info.renderPass = render_pass;
    create_info.attachmentCount = attachments.size();
    create_info.pAttachments = attachments.data();
    create_info.width = width;
    create_info.height = height;
    create_info.layers = layers;

    VkFramebuffer framebuffer;
    if (vkCreateFramebuffer(device, &create_info, nullptr, &framebuffer) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create framebuffer!" << std::endl;
    #endif
        return VK_NULL_HANDLE;
    }

    return framebuffer;
}
