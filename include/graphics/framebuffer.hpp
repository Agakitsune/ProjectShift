
#ifndef ALCHEMIST_GRAPHICS_FRAMEBUFFER_HPP
#define ALCHEMIST_GRAPHICS_FRAMEBUFFER_HPP

#include <vector>

#include <vulkan/vulkan.h>

struct FramebufferBuilder {
    VkRenderPass render_pass;
    uint32_t width;
    uint32_t height;
    uint32_t layers = 1; // Default to 1 layer
    std::vector<VkImageView> attachments;

    FramebufferBuilder(uint32_t width, uint32_t height, uint32_t layers = 1);
    FramebufferBuilder(VkExtent2D extent, uint32_t layers = 1);

    FramebufferBuilder &set_render_pass(VkRenderPass render_pass);
    FramebufferBuilder &add_attachment(VkImageView attachment);
    FramebufferBuilder &set_attachment(VkImageView attachment, uint32_t index);

    VkFramebuffer build(VkDevice device) const;
};

#endif // ALCHEMIST_GRAPHICS_FRAMEBUFFER_HPP
