
#ifdef ALCHEMIST_DEBUG
#include <iostream>
#endif // ALCHEMIST_DEBUG

#include "server/queue.hpp"
#include "vulkan/sync.hpp"

#include "vulkan/command_buffer.hpp"

Submit::Submit(VkQueue queue) : queue(queue) {}

Submit::Submit(Submit &&other)
    : queue(other.queue) {
    command_buffers = std::move(other.command_buffers); // Move command buffers from the other Submit
    wait_semaphores = std::move(other.wait_semaphores); // Move wait semaphores from the other Submit
    signal_semaphores = std::move(other.signal_semaphores); // Move signal semaphores from the other Submit
}

Submit::~Submit() {}

void Submit::wait() const {
    // Wait for the queue to finish processing the submitted command buffers
    if (vkQueueWaitIdle(queue) != VK_SUCCESS) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to wait for queue!" << std::endl;
        #endif
    }
}



SubmitBuilder::SubmitBuilder(VkQueue queue) : queue(queue) {}

SubmitBuilder &SubmitBuilder::add_command_buffer(const CommandBuffer &buffer) {
    command_buffers.push_back(buffer.buffer); // Add the Vulkan command buffer handle to the vector
    return *this; // Return the builder for method chaining
}

SubmitBuilder &SubmitBuilder::add_wait_semaphore(VkSemaphore semaphore, VkPipelineStageFlags stage) {
    wait_semaphores.push_back(semaphore); // Add the Vulkan semaphore handle to
    wait_stages.push_back(stage); // Add the corresponding pipeline stage flag to the vector
    return *this; // Return the builder for method chaining
}

SubmitBuilder &SubmitBuilder::add_signal_semaphore(VkSemaphore semaphore) {
    signal_semaphores.push_back(semaphore); // Add the Vulkan semaphore handle to
    return *this; // Return the builder for method chaining
}

Submit SubmitBuilder::submit(VkFence fence) {
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = static_cast<uint32_t>(command_buffers.size());
    submit_info.pCommandBuffers = command_buffers.data(); // Pointer to the command buffers

    if (!wait_semaphores.empty()) {
        submit_info.waitSemaphoreCount = static_cast<uint32_t>(wait_semaphores.size());
        submit_info.pWaitSemaphores = wait_semaphores.data(); // Pointer to the wait semaphores
        submit_info.pWaitDstStageMask = wait_stages.data(); // Pointer to the wait stages
    }

    if (!signal_semaphores.empty()) {
        submit_info.signalSemaphoreCount = static_cast<uint32_t>(signal_semaphores.size());
        submit_info.pSignalSemaphores = signal_semaphores.data(); // Pointer to the signal semaphores
    }

    Submit submit(queue); // Create a Submit object with the queue and command buffers
    submit.command_buffers.swap(command_buffers); // Transfer ownership of command buffers to the Submit object
    submit.wait_semaphores.swap(wait_semaphores); // Transfer ownership of wait semap
    submit.signal_semaphores.swap(signal_semaphores); // Transfer ownership of signal semaphores
    submit.wait_stages.swap(wait_stages); // Transfer ownership of wait stages

    // Submit the command buffers to the queue
    if (vkQueueSubmit(queue, 1, &submit_info, fence) != VK_SUCCESS) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to submit command buffer!" << std::endl;
        #endif
        return Submit(queue); // Return an empty Submit on failure
    }

    return submit; // Return the Submit object
}



SubmitBuilder Queue::submit() const {
    return SubmitBuilder(queue); // Return a SubmitBuilder initialized with the queue
}

void Queue::wait() const {
    if (vkQueueWaitIdle(queue) != VK_SUCCESS) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to wait for queue!" << std::endl;
        #endif
    }
}



QueueServer::QueueServer(VkDevice device) : device(device) {}

QueueServer::~QueueServer() {
    #ifdef ALCHEMIST_DEBUG
    for (const auto &queue : queues) {
        std::cout << "Destroying queue with RID: " << queue.rid << std::endl;
    }
    #endif
}

RID QueueServer::new_queue(uint32_t queue_family_index, uint32_t queue_index) {
    VkQueue queue;
    RID rid = RIDServer::instance().new_id(RIDServer::QUEUE); // Get a new RID for the queue

    vkGetDeviceQueue(device, queue_family_index, queue_index, &queue);

    Queue new_queue;
    new_queue.queue = queue;
    new_queue.rid = rid; // Set the RID for the new queue

    queues.emplace_back(std::move(new_queue)); // Add the new queue to the queues vector

    return rid; // Return the RID of the newly created queue
}

SubmitBuilder QueueServer::submit(RID rid) const {
    for (const auto &queue : queues) {
        if (queue.rid == rid) {
            return queue.submit(); // Return a SubmitBuilder for the specified queue
        }
    }
    #ifdef ALCHEMIST_DEBUG
    std::cerr << "Queue with RID " << rid << " not found!" << std::endl;
    #endif
    return SubmitBuilder(VK_NULL_HANDLE); // Return an empty SubmitBuilder if the queue is not found
}

const Queue &QueueServer::get_queue(RID rid) const {
    for (const auto &queue : queues) {
        if (queue.rid == rid) {
            return queue; // Return the queue with the specified RID
        }
    }
    #ifdef ALCHEMIST_DEBUG
    std::cerr << "Queue with RID " << rid << " not found!" << std::endl;
    #endif
    throw std::runtime_error("Queue not found"); // Throw an exception if the queue is not found
}

QueueServer &QueueServer::instance() {
    return *__instance; // Return the singleton instance
}

std::unique_ptr<QueueServer> QueueServer::__instance = nullptr; // Singleton instance of QueueServer
