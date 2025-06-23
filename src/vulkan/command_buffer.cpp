
#ifdef ALCHEMIST_DEBUG
#include <iostream> // Include for debug output
#endif // ALCHEMIST_DEBUG

#include "vulkan/command_buffer.hpp"

void CommandBuffer::begin(VkCommandBufferUsageFlags flags) {
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = flags;

    if (vkBeginCommandBuffer(buffer, &begin_info) != VK_SUCCESS) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to begin command buffer!" << std::endl;
        #endif
    }
}

void CommandBuffer::end() {
    if (vkEndCommandBuffer(buffer) != VK_SUCCESS) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to end command buffer!" << std::endl;
        #endif
    }
}

void CommandBuffer::reset() {
    VkCommandBufferResetFlags flags = 0; // No flags for reset
    if (vkResetCommandBuffer(buffer, flags) != VK_SUCCESS) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to reset command buffer!" << std::endl;
        #endif
    }
}



CommandBuffer allocate_command_buffer(VkCommandBufferLevel level, RID command_pool) {
    CommandPool &pool = CommandPoolServer::instance().get_command_pool(command_pool);
    
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = pool.command_pool;
    alloc_info.level = level;
    alloc_info.commandBufferCount = 1;

    CommandBuffer cmd_buffer;
    if (vkAllocateCommandBuffers(CommandPoolServer::instance().device, &alloc_info, &cmd_buffer.buffer) != VK_SUCCESS) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to allocate command buffer!" << std::endl;
        #endif
        return {}; // Return an empty CommandBuffer on failure
    }

    pool.command_buffers.push_back(cmd_buffer.buffer); // Store the command buffer in the pool

    return cmd_buffer; // Return the allocated command buffer
}

void emplace_command_buffer(std::vector<CommandBuffer> &command_buffers, uint32_t count, RID command_pool) {
    for (uint32_t i = 0; i < count; ++i) {
        CommandBuffer cmd_buffer = allocate_command_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, command_pool);
        if (cmd_buffer.buffer != VK_NULL_HANDLE) {
            command_buffers.emplace_back(std::move(cmd_buffer)); // Add the command buffer to the vector
        } else {
            #ifdef ALCHEMIST_DEBUG
            std::cerr << "Failed to allocate command buffer " << i << "!" << std::endl;
            #endif
        }
    }
}
