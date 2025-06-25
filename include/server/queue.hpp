
#ifndef ALCHEMIST_SERVER_QUEUE_HPP
#define ALCHEMIST_SERVER_QUEUE_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "server/rid.hpp"

struct CommandBuffer;

struct Submit {
    VkQueue queue;
    std::vector<VkCommandBuffer> command_buffers; // Vector to hold command buffers
    std::vector<VkSemaphore> wait_semaphores; // Optional wait semaphores
    std::vector<VkPipelineStageFlags> wait_stages; // Optional wait stages
    std::vector<VkSemaphore> signal_semaphores; // Optional signal semaphores

    Submit(VkQueue queue);
    Submit(Submit &&other);

    ~Submit();

    void wait() const;
};

struct SubmitBuilder {
    VkQueue queue;
    std::vector<VkCommandBuffer> command_buffers; // Vector to hold command buffers
    std::vector<VkSemaphore> wait_semaphores; // Optional wait semaphores
    std::vector<VkPipelineStageFlags> wait_stages; // Optional wait stages
    std::vector<VkSemaphore> signal_semaphores; // Optional signal semaphores

    SubmitBuilder(VkQueue queue);

    SubmitBuilder &add_command_buffer(const CommandBuffer &buffer);
    SubmitBuilder &add_wait_semaphore(VkSemaphore semaphore, VkPipelineStageFlags stage);
    SubmitBuilder &add_signal_semaphore(VkSemaphore semaphore);

    Submit submit(VkFence fence = VK_NULL_HANDLE);
};

struct Queue {
    VkQueue queue; // Vulkan queue object
    RID rid = RID_INVALID; // Resource ID for the queue

    Queue() = default;

    SubmitBuilder submit() const;
    void wait() const;
};

struct QueueServer {
    std::vector<Queue> queues; // Vector to hold all queues

    VkDevice device; // Vulkan device

    QueueServer(VkDevice device);
    ~QueueServer();

    RID new_queue(uint32_t queue_family_index, uint32_t queue_index = 0);

    SubmitBuilder submit(RID rid) const;

    const Queue &get_queue(RID rid) const;

    static QueueServer &instance();
    static std::unique_ptr<QueueServer> __instance; // Singleton instance of QueueServer
};

#endif // ALCHEMIST_SERVER_QUEUE_HPP
