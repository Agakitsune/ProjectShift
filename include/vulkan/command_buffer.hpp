
#ifndef ALCHEMIST_VULKAN_COMMAND_BUFFER_HPP
#define ALCHEMIST_VULKAN_COMMAND_BUFFER_HPP

#include <cstdint>

#include <vulkan/vulkan.h>

#include "server/command_pool.hpp"

struct CommandBuffer {
    VkCommandBuffer buffer;

    void begin(VkCommandBufferUsageFlags flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    void end();

    void reset();
};

CommandBuffer allocate_command_buffer(VkCommandBufferLevel level, RID command_pool);
void emplace_command_buffer(std::vector<CommandBuffer> &command_buffers, uint32_t count, RID command_pool);

#endif // ALCHEMIST_VULKAN_COMMAND_BUFFER_HPP
