
#include "graphics/vulkan/renderer.hpp"
#include "graphics/vulkan/command_buffer.hpp"
#include "graphics/vulkan/framebuffer.hpp"
#include "graphics/vulkan/sync.hpp"

SwapchainInfo &SwapchainInfo::set_images(uint32_t image_count,
                                         VkImageView *image_views) {
    this->image_count = image_count;
    this->image_views = image_views;
    return *this;
}

SwapchainInfo &
SwapchainInfo::set_depth_image_view(VkImageView depth_image_view) {
    this->depth_image_view = depth_image_view;
    return *this;
}

SwapchainInfo &SwapchainInfo::set_extent(VkExtent2D extent) {
    this->extent = extent;
    return *this;
}

RendererCreateInfo &
RendererCreateInfo::set_render_pass(VkRenderPass render_pass) {
    this->render_pass = render_pass;
    return *this;
}

RendererCreateInfo &
RendererCreateInfo::set_command_pool(VkCommandPool command_pool) {
    this->command_pool = command_pool;
    return *this;
}

RendererCreateInfo &
RendererCreateInfo::set_swapchain_info(const SwapchainInfo &swapchain_info) {
    this->swapchain_info = swapchain_info;
    return *this;
}

void create_renderer(Renderer &renderer, VkDevice device,
                     const RendererCreateInfo &create_info) {
    const SwapchainInfo &info = create_info.swapchain_info;

    renderer.framebuffers = new VkFramebuffer[info.image_count];

    VkImageView attachments[2];
    attachments[0] = VK_NULL_HANDLE;        // Placeholder for color attachment
    attachments[1] = info.depth_image_view; // Depth attachment

    VkFramebufferCreateInfo framebuffer_info = {};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = create_info.render_pass;
    framebuffer_info.attachmentCount = 2;
    framebuffer_info.pAttachments = attachments;
    framebuffer_info.width = info.extent.width;
    framebuffer_info.height = info.extent.height;
    framebuffer_info.layers = 1;

    // Create framebuffers for each swapchain image
    for (uint32_t i = 0; i < info.image_count; ++i) {
        attachments[0] = info.image_views[i]; // Set the color attachment
        if (vkCreateFramebuffer(device, &framebuffer_info, nullptr,
                                &renderer.framebuffers[i]) != VK_SUCCESS) {
            renderer.framebuffers[i] = VK_NULL_HANDLE; // Creation failed
        }
    }

    // Initialize in-flight data
    renderer.command_buffers.reserve(create_info.in_flight_count);
    renderer.image_available_semaphores.reserve(create_info.in_flight_count);
    renderer.render_finished_semaphores.reserve(create_info.in_flight_count);
    renderer.in_flight_fences.reserve(create_info.in_flight_count);

    for (uint32_t i = 0; i < create_info.in_flight_count; ++i) {
        renderer.command_buffers.push(create_command_buffer(
            device, create_info.command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY));
        renderer.image_available_semaphores.push(create_semaphore(device));
        renderer.render_finished_semaphores.push(create_semaphore(device));
        renderer.in_flight_fences.push(create_fence(device));
    }

    renderer.in_flight_count = create_info.in_flight_count;
}

Result begin_draw(Renderer &renderer, VkDevice device, VkSwapchainKHR swapchain,
                  uint32_t &image_index) {
    vkWaitForFences(device, 1,
                    &renderer.in_flight_fences.data[renderer.current_frame],
                    VK_TRUE, UINT64_MAX);

    VkResult result = vkAcquireNextImageKHR(
        device, swapchain, UINT64_MAX,
        renderer.image_available_semaphores.data[renderer.current_frame],
        VK_NULL_HANDLE, &image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // Handle swapchain recreation if necessary
        return RESULT_OUT_OF_DATE;
    } else if (result != VK_SUCCESS) {
        // Handle other errors
        return RESULT_FAILURE;
    }

    vkResetFences(device, 1,
                  &renderer.in_flight_fences.data[renderer.current_frame]);
    vkResetCommandBuffer(renderer.command_buffers.data[renderer.current_frame],
                         0);

    return RESULT_SUCCESS;
}

void submit_draw(Renderer &renderer, VkQueue queue) {
    VkCommandBuffer command_buffer =
        renderer.command_buffers.data[renderer.current_frame];

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores =
        &renderer.image_available_semaphores.data[renderer.current_frame];
    VkPipelineStageFlags wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores =
        &renderer.render_finished_semaphores.data[renderer.current_frame];

    if (vkQueueSubmit(queue, 1, &submit_info,
                      renderer.in_flight_fences.data[renderer.current_frame]) !=
        VK_SUCCESS) {
        // Handle submission failure
        return;
    }

    renderer.current_frame =
        (renderer.current_frame + 1) % renderer.in_flight_count;
}

Result present_draw(Renderer &renderer, VkQueue queue, VkSwapchainKHR swapchain,
                    uint32_t image_index) {
    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores =
        &renderer.render_finished_semaphores.data[renderer.current_frame - 1];
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain;
    present_info.pImageIndices = &image_index;

    VkResult result = vkQueuePresentKHR(queue, &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // Handle swapchain recreation if necessary
        return RESULT_OUT_OF_DATE;
    } else if (result != VK_SUCCESS) {
        // Handle other errors
        return RESULT_FAILURE;
    }

    return RESULT_SUCCESS;
}
