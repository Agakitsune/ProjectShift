
#ifndef ALCHEMIST_VULKAN_SYNC_HPP
#define ALCHEMIST_VULKAN_SYNC_HPP

#include <vector>

#include <vulkan/vulkan.h>

struct Semaphore {
    VkSemaphore semaphore = VK_NULL_HANDLE; // Vulkan semaphore object
    VkDevice device = VK_NULL_HANDLE; // Vulkan device associated with the semaphore

    ~Semaphore();
    Semaphore() = default;
    Semaphore(VkSemaphore semaphore, VkDevice device)
        : semaphore(semaphore), device(device) {}
    Semaphore(Semaphore &&other) noexcept
        : semaphore(other.semaphore), device(other.device) {
        other.semaphore = VK_NULL_HANDLE; // Transfer ownership
        other.device = VK_NULL_HANDLE;
    }

    void wait(uint64_t timeout = UINT64_MAX) const;
    void signal() const;
};

struct Fence {
    VkFence fence = VK_NULL_HANDLE; // Vulkan fence object
    VkDevice device = VK_NULL_HANDLE; // Vulkan device associated with the fence

    ~Fence();
    Fence() = default;
    Fence(VkFence fence, VkDevice device)
        : fence(fence), device(device) {}
    Fence(Fence &&other) noexcept
        : fence(other.fence), device(other.device) {
        other.fence = VK_NULL_HANDLE; // Transfer ownership
        other.device = VK_NULL_HANDLE;
    }

    void reset() const;
    void wait(uint64_t timeout = UINT64_MAX) const;
    uint32_t is_signaled() const;
};

struct SemaphoreBuilder {
    VkSemaphoreCreateInfo create_info;
    VkDevice device;

    SemaphoreBuilder(VkDevice device);

    Semaphore build() const;
    void emplace(std::vector<Semaphore> &semaphores, uint32_t count) const;
};

struct FenceBuilder {
    VkFenceCreateInfo create_info;
    VkDevice device;

    FenceBuilder(VkDevice device);

    FenceBuilder &signaled();
    
    Fence build() const;
    void emplace(std::vector<Fence> &fences, uint32_t count) const;
};

#endif // ALCHEMIST_VULKAN_SYNC_HPP
