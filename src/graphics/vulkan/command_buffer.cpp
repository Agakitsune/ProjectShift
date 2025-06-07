
#include "graphics/vulkan/command_buffer.hpp"

SubmitInfo::SubmitInfo(uint32_t wait_semaphore_count,
                       uint32_t command_buffer_count,
                       uint32_t signal_semaphore_count)
    : wait_semaphores(wait_semaphore_count),
      command_buffers(command_buffer_count),
      signal_semaphores(signal_semaphore_count) {}

SubmitInfo &SubmitInfo::add_wait_semaphore(VkSemaphore semaphore,
                                           VkPipelineStageFlags stage_mask) {
    wait_semaphores.push(semaphore, stage_mask);
    return *this;
}

SubmitInfo &SubmitInfo::add_command_buffer(VkCommandBuffer command_buffer) {
    command_buffers.push(command_buffer);
    return *this;
}

SubmitInfo &SubmitInfo::add_signal_semaphore(VkSemaphore semaphore) {
    signal_semaphores.push(semaphore);
    return *this;
}

VkCommandPool create_command_pool(VkDevice device, uint32_t queue_family_index,
                                  VkCommandPoolCreateFlags flags) {
    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = queue_family_index;
    pool_info.flags = flags;

    VkCommandPool command_pool;
    if (vkCreateCommandPool(device, &pool_info, nullptr, &command_pool) !=
        VK_SUCCESS) {
        return VK_NULL_HANDLE; // Creation failed
    }

    return command_pool;
}

VkCommandBuffer create_command_buffer(VkDevice device,
                                      VkCommandPool command_pool,
                                      VkCommandBufferLevel level) {
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = command_pool;
    alloc_info.level = level;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    if (vkAllocateCommandBuffers(device, &alloc_info, &command_buffer) !=
        VK_SUCCESS) {
        return VK_NULL_HANDLE; // Allocation failed
    }

    return command_buffer;
}

void begin_command_buffer(VkCommandBuffer command_buffer,
                          VkCommandBufferUsageFlags usage) {
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = usage;

    if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
        // Handle error (e.g., throw an exception or log an error)
    }
}

void end_command_buffer(VkCommandBuffer command_buffer) {
    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        // Handle error (e.g., throw an exception or log an error)
    }
}

void submit_command_buffer(VkQueue queue, const SubmitInfo &info,
                           VkFence fence) {
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submit_info.waitSemaphoreCount = info.wait_semaphores.size;
    submit_info.pWaitSemaphores = info.wait_semaphores.data1;
    submit_info.pWaitDstStageMask = info.wait_semaphores.data2;
    submit_info.commandBufferCount = info.command_buffers.size;
    submit_info.pCommandBuffers = info.command_buffers.data;
    submit_info.signalSemaphoreCount = info.signal_semaphores.size;
    submit_info.pSignalSemaphores = info.signal_semaphores.data;

    if (vkQueueSubmit(queue, 1, &submit_info, fence) != VK_SUCCESS) {
        // Handle error (e.g., throw an exception or log an error)
    }
}
