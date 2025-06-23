
#ifndef ALCHEMIST_SERVER_BUFFER_HPP
#define ALCHEMIST_SERVER_BUFFER_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "server/rid.hpp"

struct BufferServer; // Forward declaration

struct BufferBuilder {
    VkBufferCreateInfo create_info;

    BufferServer &server; // Reference to the BufferServer for building buffers

    BufferBuilder(BufferServer &server);

    BufferBuilder &set_size(VkDeviceSize size);
    BufferBuilder &set_usage(VkBufferUsageFlags usage);
    BufferBuilder &set_sharing_mode(VkSharingMode sharing_mode);

    RID build() const; // Create the buffer and return its RID
};

struct Buffer {
    VkBuffer buffer;
    RID rid = RID_INVALID; // Resource ID
    RID memory_rid = RID_INVALID; // Resource ID for the GPU memory block

    ~Buffer();
};

struct CmdUploadBuffer {
    VkBuffer buffer;
    VkBuffer staging;
    RID memory_rid = RID_INVALID;

    VkBufferCopy copy_region;

    CmdUploadBuffer() = default;
    CmdUploadBuffer(VkBuffer buffer);
    ~CmdUploadBuffer();

    CmdUploadBuffer &upload_data(VkDevice device, VkPhysicalDevice physical_device, VkDeviceSize size, const void *data);
};

enum class BufferCommandType {
    UPLOAD
};

struct BufferServer {
    std::vector<Buffer> buffers; // Vector to hold all buffers

    std::vector<CmdUploadBuffer> upload_commands; // Vector to hold upload commands
    std::vector<BufferCommandType> command_types; // Vector to hold command types

    VkDevice device; // Vulkan device
    VkPhysicalDevice physical_device; // Vulkan physical device

    BufferServer(VkDevice device, VkPhysicalDevice physical_device);
    ~BufferServer();

    RID new_buffer(const VkBufferCreateInfo &create_info);
    RID new_buffer(VkBufferCreateInfo &&create_info);

    BufferBuilder new_buffer();

    RID bind_buffer(RID buffer, RID memory);
    void bind_best(RID buffer, VkMemoryPropertyFlags flags);

    const Buffer &get_buffer(RID rid) const;
    void get_requirements(RID rid, VkMemoryRequirements &requirements) const;

    CmdUploadBuffer &upload_buffer(RID rid);

    void execute_commands(VkCommandBuffer cmd_buffer);
    void clear_commands(); // Clear the command buffers, make sure to wait for the commands to

    static BufferServer &instance();

    static std::unique_ptr<BufferServer> __instance; // Singleton instance of BufferServer
};

#endif // ALCHEMIST_SERVER_BUFFER_HPP
