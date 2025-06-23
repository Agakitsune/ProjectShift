
#ifdef ALCHEMIST_DEBUG
#include <iostream> // Include for debug output
#endif // ALCHEMIST_DEBUG

#include "server/command_pool.hpp"

CommandPoolBuilder::CommandPoolBuilder(CommandPoolServer &server) : server(server) {
    create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Default flags, can be set later
    create_info.queueFamilyIndex = 0; // Default queue family index, must be set before building
}

CommandPoolBuilder &CommandPoolBuilder::set_flags(VkCommandPoolCreateFlags flags) {
    create_info.flags = flags;
    return *this; // Return the builder for chaining
}

CommandPoolBuilder &CommandPoolBuilder::set_queue_family_index(uint32_t queue_family_index) {
    create_info.queueFamilyIndex = queue_family_index;
    return *this; // Return the builder for chaining
}

RID CommandPoolBuilder::build() const {
    RID rid = server.new_command_pool(create_info); // Create the command pool using the server
    if (rid == RID_INVALID) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create command pool!" << std::endl;
    #endif
        return RID_INVALID; // Return an invalid RID if creation fails
    }

    return rid; // Return the RID of the newly created command pool
} // Create the command pool and return its RID



CommandPoolServer::CommandPoolServer(VkDevice device) : device(device) {}

CommandPoolServer::~CommandPoolServer() {
    for (const auto &pool : command_pools) {
        #ifdef ALCHEMIST_DEBUG
        std::cout << "Destroying command pool with RID: " << pool.rid << std::endl;
        #endif
        for (const auto &command_buffer : pool.command_buffers) {
            vkFreeCommandBuffers(device, pool.command_pool, 1, &command_buffer); // Free each command buffer in the pool
        }
        vkDestroyCommandPool(device, pool.command_pool, nullptr); // Clean up each command pool
    }
}

RID CommandPoolServer::new_command_pool(const VkCommandPoolCreateInfo &create_info) {
    VkCommandPool command_pool;
    if (vkCreateCommandPool(device, &create_info, nullptr, &command_pool) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create command pool!" << std::endl;
    #endif
        return RID_INVALID; // Return an invalid RID if creation fails
    }

    CommandPool pool;
    pool.command_pool = command_pool;
    pool.rid = RIDServer::instance().new_id(RIDServer::COMMAND_POOL); // Generate a new RID for the command pool

    command_pools.push_back(std::move(pool)); // Add the created command pool to the vector

    return command_pools.back().rid; // Return the RID of the newly created command pool
}

RID CommandPoolServer::new_command_pool(VkCommandPoolCreateInfo &&create_info) {
    VkCommandPool command_pool;
    if (vkCreateCommandPool(device, &create_info, nullptr, &command_pool) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create command pool!" << std::endl;
    #endif
        return RID_INVALID; // Return an invalid RID if creation fails
    }

    CommandPool pool;
    pool.command_pool = command_pool;
    pool.rid = RIDServer::instance().new_id(RIDServer::COMMAND_POOL); // Generate a new RID for the command pool

    command_pools.push_back(std::move(pool)); // Add the created command pool to the vector

    return command_pools.back().rid; // Return the RID of the newly created command pool
}

CommandPoolBuilder CommandPoolServer::new_command_pool() {
    return CommandPoolBuilder(*this); // Create a new command pool builder
} // Create a new command pool builder

CommandPool &CommandPoolServer::get_command_pool(RID rid) {
    for (auto &pool : command_pools) {
        if (pool.rid == rid) {
            return pool; // Return the command pool if found
        }
    }
    #ifdef ALCHEMIST_DEBUG
    std::cerr << "Command pool with RID " << rid << " not found!" << std::endl;
    #endif
    return *command_pools.end(); // Return an invalid command pool if not found
}

CommandPoolServer &CommandPoolServer::instance() {
    return *__instance; // Return the singleton instance
}

std::unique_ptr<CommandPoolServer> CommandPoolServer::__instance = nullptr; // Singleton instance of CommandPoolServer
