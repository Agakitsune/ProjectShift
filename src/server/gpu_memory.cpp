
#include "server/gpu_memory.hpp"

GpuMemoryServer &GpuMemoryServer::instance() {
    return *__instance; // Return the singleton instance
}

std::unique_ptr<GpuMemoryServer> GpuMemoryServer::__instance = nullptr; // Singleton instance of GpuMemoryServer
