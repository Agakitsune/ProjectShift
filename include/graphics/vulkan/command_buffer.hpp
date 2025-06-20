
#ifndef ALCHEMIST_GRAPHICS_VULKAN_COMMAND_BUFFER_HPP
#define ALCHEMIST_GRAPHICS_VULKAN_COMMAND_BUFFER_HPP

#include <vulkan/vulkan.h>

#include "memory/vector.hpp"

struct SubmitInfo {
    alchemist::dual_vector<VkSemaphore, VkPipelineStageFlags> wait_semaphores;
    std::vector<VkCommandBuffer> command_buffers;
    std::vector<VkSemaphore> signal_semaphores;

    SubmitInfo() = default;
    SubmitInfo(uint32_t wait_semaphore_count, uint32_t command_buffer_count,
               uint32_t signal_semaphore_count);

    SubmitInfo &add_wait_semaphore(VkSemaphore semaphore,
                                   VkPipelineStageFlags stage_mask);
    SubmitInfo &add_command_buffer(VkCommandBuffer command_buffer);
    SubmitInfo &add_signal_semaphore(VkSemaphore semaphore);
};

VkCommandPool
create_command_pool(VkDevice device, uint32_t queue_family_index,
                    VkCommandPoolCreateFlags flags =
                        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

VkCommandBuffer create_command_buffer(
    VkDevice device, VkCommandPool command_pool,
    VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

void begin_command_buffer(VkCommandBuffer command_buffer,
                          VkCommandBufferUsageFlags usage);
void end_command_buffer(VkCommandBuffer command_buffer);

void submit_command_buffer(VkQueue queue, const SubmitInfo &info,
                           VkFence fence = VK_NULL_HANDLE);

#endif // ALCHEMIST_GRAPHICS_VULKAN_COMMAND_BUFFER_HPP
