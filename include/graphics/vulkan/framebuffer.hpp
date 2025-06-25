
#ifndef ALCHEMIST_GRAPHICS_VULKAN_FRAMEBUFFER_HPP
#define ALCHEMIST_GRAPHICS_VULKAN_FRAMEBUFFER_HPP

#include <vector>

#include <vulkan/vulkan.h>

struct FramebufferCreateInfo {
    VkRenderPass render_pass;
    std::vector<VkImageView> attachments;
    uint32_t width;
    uint32_t height;
    uint32_t layers = 1; // Default to 1 layer

    FramebufferCreateInfo(VkRenderPass render_pass, uint32_t width,
                          uint32_t height, uint32_t layers = 1);

    FramebufferCreateInfo &reserve_attachments(uint32_t count);
    FramebufferCreateInfo &add_attachment(VkImageView attachment);
    FramebufferCreateInfo &set_attachment(VkImageView attachment,
                                          uint32_t index);
};

VkFramebuffer create_framebuffer(VkDevice device,
                                 const FramebufferCreateInfo &create_info);
void create_framebuffers(VkDevice device,
                         const FramebufferCreateInfo &create_info,
                         VkFramebuffer *framebuffers, uint32_t count);

#endif // ALCHEMIST_GRAPHICS_VULKAN_FRAMEBUFFER_HPP
