
#ifndef ALCHEMIST_SERVER_FRAMEBUFFER_HPP
#define ALCHEMIST_SERVER_FRAMEBUFFER_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "server/rid.hpp"

struct Framebuffer {
    VkFramebuffer framebuffer; // Vulkan framebuffer object
    RID rid = RID_INVALID; // Resource ID for the framebuffer

    Framebuffer() = default;
};

struct FramebufferServer; // Forward declaration

struct FramebufferBuilder {
    VkRenderPass render_pass;
    uint32_t width;
    uint32_t height;
    uint32_t layers = 1; // Default to 1 layer
    std::vector<VkImageView> attachments;

    FramebufferServer &server;

    FramebufferBuilder(FramebufferServer &server, uint32_t width, uint32_t height, uint32_t layers = 1);
    FramebufferBuilder(FramebufferServer &server, VkExtent2D extent, uint32_t layers = 1);

    FramebufferBuilder &set_render_pass(RID render_pass);
    FramebufferBuilder &add_attachment(VkImageView attachment);
    FramebufferBuilder &set_attachment(VkImageView attachment, uint32_t index);
    FramebufferBuilder &add_attachment(RID attachment);
    FramebufferBuilder &set_attachment(RID attachment, uint32_t index);

    RID build() const;
};

struct FramebufferServer {
    std::vector<Framebuffer> framebuffers; // Vector to hold all framebuffers
    VkDevice device; // Vulkan device

    FramebufferServer(VkDevice device);
    ~FramebufferServer();

    RID new_framebuffer(const VkFramebufferCreateInfo &create_info);
    RID new_framebuffer(VkFramebufferCreateInfo &&create_info);

    FramebufferBuilder new_framebuffer(uint32_t width, uint32_t height, uint32_t layers = 1);

    const Framebuffer &get_framebuffer(RID rid) const;

    static FramebufferServer &instance();
    static std::unique_ptr<FramebufferServer> __instance; // Singleton instance of Framebuffer
};

#endif // ALCHEMIST_SERVER_FRAMEBUFFER_HPP
