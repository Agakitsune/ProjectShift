
#ifdef ALCHEMIST_DEBUG
#include <iostream>
#endif // ALCHEMIST_DEBUG

#include "vulkan/sync.hpp"

Semaphore::~Semaphore() {
    if (semaphore != VK_NULL_HANDLE) {
        #ifdef ALCHEMIST_DEBUG
        std::cout << "Destroying semaphore: " << semaphore << " for device: " << device << std::endl;
        #endif
        vkDestroySemaphore(device, semaphore, nullptr); // Destroy the semaphore
    }
}


void Semaphore::wait(uint64_t timeout) const {
    VkSemaphoreWaitInfo wait_info{};
    wait_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    wait_info.semaphoreCount = 1; // Number of semaphores to wait for
    wait_info.pSemaphores = &semaphore; // Pointer to the semaphore
    wait_info.pValues = nullptr; // Optional values, can be null if not needed

    if (vkWaitSemaphores(device, &wait_info, timeout) != VK_SUCCESS) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to wait for semaphore!" << std::endl;
        #endif
    }
}

void Semaphore::signal() const {
    VkSemaphoreSignalInfo signal_info{};
    signal_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
    signal_info.semaphore = semaphore;
    signal_info.value = 1; // Default value, can be modified if needed

    if (vkSignalSemaphore(device, &signal_info) != VK_SUCCESS) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to signal semaphore!" << std::endl;
        #endif
    }
}



Fence::~Fence() {
    if (fence != VK_NULL_HANDLE) {
        #ifdef ALCHEMIST_DEBUG
        std::cout << "Destroying fence: " << fence << " for device: " << device << std::endl;
        #endif
        vkDestroyFence(device, fence, nullptr); // Destroy the fence
    }
}

void Fence::reset() const {
    if (vkResetFences(device, 1, &fence) != VK_SUCCESS) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to reset fence!" << std::endl;
        #endif
    }
}

void Fence::wait(uint64_t timeout) const {
    if (vkWaitForFences(device, 1, &fence, VK_TRUE, timeout) != VK_SUCCESS) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to wait for fence with timeout!" << std::endl;
        #endif
    }
}

uint32_t Fence::is_signaled() const {
    VkResult result = vkGetFenceStatus(device, fence);
    if (result == VK_SUCCESS) {
        return 1; // Fence is signaled
    } else if (result == VK_NOT_READY) {
        return 0; // Fence is not signaled
    } else {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to get fence status!" << std::endl;
        #endif
        return UINT32_MAX; // Error occurred
    }
}



SemaphoreBuilder::SemaphoreBuilder(VkDevice device) : device(device) {
    create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0; // Default flags, can be modified later if needed
}

Semaphore SemaphoreBuilder::build() const {
    VkSemaphore semaphore;
    if (vkCreateSemaphore(device, &create_info, nullptr, &semaphore) != VK_SUCCESS) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create semaphore!" << std::endl;
        #endif
        return Semaphore(VK_NULL_HANDLE, device); // Return an invalid semaphore on failure
    }
    Semaphore sem;
    sem.semaphore = semaphore; // Assign the created semaphore
    sem.device = device; // Assign the device associated with the semaphore
    return std::move(sem); // Return the created semaphore
}

void SemaphoreBuilder::emplace(std::vector<Semaphore> &semaphores, uint32_t count) const {
    for (uint32_t i = 0; i < count; ++i) {
        semaphores.emplace_back(std::move(build())); // Create and add semaphores to the vector
    }
}



FenceBuilder::FenceBuilder(VkDevice device) : device(device) {
    create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0; // Default flags, can be modified later if needed
}

FenceBuilder &FenceBuilder::signaled() {
    create_info.flags |= VK_FENCE_CREATE_SIGNALED_BIT; // Set the signaled flag
    return *this; // Return the builder for chaining
}

Fence FenceBuilder::build() const {
    VkFence fence;
    if (vkCreateFence(device, &create_info, nullptr, &fence) != VK_SUCCESS) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create fence!" << std::endl;
        #endif
        return Fence(VK_NULL_HANDLE, device); // Return an invalid fence on failure
    }
    Fence f;
    f.fence = fence; // Assign the created fence
    f.device = device; // Assign the device associated with the fence
    return std::move(f); // Return the created fence
}

void FenceBuilder::emplace(std::vector<Fence> &fences, uint32_t count) const {
    for (uint32_t i = 0; i < count; ++i) {
        fences.emplace_back(std::move(build())); // Create and add fences to the vector
    }
}
