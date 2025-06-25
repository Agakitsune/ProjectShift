
#ifndef ALCHEMIST_GRAPHICS_VULKAN_RENDERER_HPP
#define ALCHEMIST_GRAPHICS_VULKAN_RENDERER_HPP

#include <vulkan/vulkan.h>

#include "memory/vector.hpp"

struct Renderer {
    VkFramebuffer *framebuffers; // Array of swapchain framebuffers

    // In flight data
    std::vector<VkCommandBuffer>
        command_buffers; // Command buffers for each frame
    std::vector<VkSemaphore>
        image_available_semaphores; // Semaphores for image availability
    std::vector<VkSemaphore>
        render_finished_semaphores; // Semaphores for rendering completion
    std::vector<VkFence>
        in_flight_fences; // Fences for synchronizing frame rendering

    uint32_t in_flight_count = 2; // Number of in-flight frames (default is 2)
    uint32_t current_frame = 0;   // Index of the current frame being rendered
};

struct SwapchainInfo {
    VkImageView *image_views = nullptr; // Image views for the swapchain images
    uint32_t image_count = 0;           // Number of images in the swapchain

    VkImageView depth_image_view =
        VK_NULL_HANDLE; // Depth image view for the swapchain
    VkExtent2D extent = {0, 0};

    SwapchainInfo() = default;

    SwapchainInfo &set_images(uint32_t image_count, VkImageView *image_views);
    SwapchainInfo &set_depth_image_view(VkImageView depth_image_view);
    SwapchainInfo &set_extent(VkExtent2D extent);
};

struct RendererCreateInfo {
    VkRenderPass render_pass;     // Render pass to use for the renderer
    VkCommandPool command_pool;   // Command pool for command buffer allocation
    SwapchainInfo swapchain_info; // Information about the swapchain

    uint32_t in_flight_count = 2; // Number of in-flight frames (default is 2)

    RendererCreateInfo() = default;
    RendererCreateInfo &set_render_pass(VkRenderPass render_pass);
    RendererCreateInfo &set_command_pool(VkCommandPool command_pool);
    RendererCreateInfo &set_swapchain_info(const SwapchainInfo &swapchain_info);
};

enum Result {
    RESULT_SUCCESS = 0,
    RESULT_FAILURE = -1,
    RESULT_OUT_OF_DATE = -2,
};

void create_renderer(Renderer &renderer, VkDevice device,
                     const RendererCreateInfo &create_info);

Result begin_draw(Renderer &renderer, VkDevice device, VkSwapchainKHR swapchain,
                  uint32_t &image_index);
void submit_draw(Renderer &renderer, VkQueue queue);
Result present_draw(Renderer &renderer, VkQueue queue, VkSwapchainKHR swapchain,
                    uint32_t image_index);

#endif // ALCHEMIST_GRAPHICS_VULKAN_RENDERER_HPP
