
#ifndef ALCHEMIST_SERVER_COMMAND_POOL_HPP
#define ALCHEMIST_SERVER_COMMAND_POOL_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "server/rid.hpp"

struct CommandPool {
    VkCommandPool command_pool; // Vulkan command pool object
    RID rid = RID_INVALID; // Resource ID for the command pool

    std::vector<VkCommandBuffer> command_buffers; // Vector to hold command buffers allocated from this pool

    CommandPool() = default;
};

struct CommandPoolServer; // Forward declaration

struct CommandPoolBuilder {
    VkCommandPoolCreateInfo create_info; // Vulkan command pool creation info
    CommandPoolServer &server; // Reference to the CommandPoolServer for building command pools

    CommandPoolBuilder(CommandPoolServer &server);

    CommandPoolBuilder &set_flags(VkCommandPoolCreateFlags flags);
    CommandPoolBuilder &set_queue_family_index(uint32_t queue_family_index);

    RID build() const; // Create the command pool and return its RID
};

struct CommandPoolServer {
    std::vector<CommandPool> command_pools; // Vector to hold all command pools

    VkDevice device; // Vulkan device

    CommandPoolServer(VkDevice device);
    ~CommandPoolServer();

    RID new_command_pool(const VkCommandPoolCreateInfo &create_info);
    RID new_command_pool(VkCommandPoolCreateInfo &&create_info);

    CommandPoolBuilder new_command_pool(); // Create a new command pool builder

    CommandPool &get_command_pool(RID rid);

    static CommandPoolServer &instance();
    static std::unique_ptr<CommandPoolServer> __instance; // Singleton instance of CommandPoolServer
};

#endif // ALCHEMIST_SERVER_COMMAND_POOL_HPP
