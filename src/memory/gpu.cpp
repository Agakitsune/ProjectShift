
#include "memory/gpu.hpp"

GpuMemoryServer &new_gpu_memory_server(VkDevice device, VkPhysicalDevice physical_device) {
    if (gpu_memory_server) {
        delete gpu_memory_server; // Clean up existing instance
    }
    gpu_memory_server = new GpuMemoryServer(device, physical_device);
    return *gpu_memory_server;
}

GpuMemoryServer &get_gpu_memory_server() {
    return *gpu_memory_server;
}

GpuMemoryServer *gpu_memory_server = nullptr;