
#include "graphics/vulkan/sync.hpp"

VkFence create_fence(VkDevice device) {
    VkFenceCreateInfo fence_info = {};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT; // No special flags

    VkFence fence;
    if (vkCreateFence(device, &fence_info, nullptr, &fence) != VK_SUCCESS) {
        return VK_NULL_HANDLE; // Creation failed
    }

    return fence;
}

VkSemaphore create_semaphore(VkDevice device) {
    VkSemaphoreCreateInfo semaphore_info = {};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_info.flags = 0; // No special flags

    VkSemaphore semaphore;
    if (vkCreateSemaphore(device, &semaphore_info, nullptr, &semaphore) !=
        VK_SUCCESS) {
        return VK_NULL_HANDLE; // Creation failed
    }

    return semaphore;
}
