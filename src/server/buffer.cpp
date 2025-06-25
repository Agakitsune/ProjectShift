
#ifdef ALCHEMIST_DEBUG
#include <iostream>
#endif // ALCHEMIST_DEBUG

#include <cstring>

#include "server/buffer.hpp"

#include "server/gpu_memory.hpp"
#include "memory/misc.hpp"

BufferBuilder::BufferBuilder(BufferServer &server) : server(server) {
    create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    create_info.size = 0; // Default size, must be set before building
    create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT; // Default usage
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Default sharing mode
}

BufferBuilder &BufferBuilder::set_size(VkDeviceSize size) {
    create_info.size = size;
    return *this; // Return the builder for chaining
}

BufferBuilder &BufferBuilder::set_usage(VkBufferUsageFlags usage) {
    create_info.usage = usage;
    return *this; // Return the builder for chaining
}

BufferBuilder &BufferBuilder::set_sharing_mode(VkSharingMode sharing_mode) {
    create_info.sharingMode = sharing_mode;
    return *this; // Return the builder for chaining
}

RID BufferBuilder::build() const {
    if (create_info.size == 0) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Buffer size must be set before building!" << std::endl;
    #endif
        return RID_INVALID; // Return an invalid RID if size is not set
    }

    RID rid = server.new_buffer(create_info); // Create the buffer using the server
    if (rid == RID_INVALID) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create buffer!" << std::endl;
    #endif
        return RID_INVALID; // Return an invalid RID if creation fails
    }

    return rid; // Return the RID of the newly created buffer
}


Buffer::~Buffer() {
    #ifdef ALCHEMIST_DEBUG
    std::cout << "Destroying buffer with RID: " << rid << std::endl;
    #endif
    if (rid != RID_INVALID) {
        RIDServer::instance().free(RIDServer::BUFFER, rid); // Free the RID of the buffer
    }
    if (memory_rid != RID_INVALID) {
        RIDServer::instance().free(RIDServer::MEMORY, memory_rid); // Free the memory block associated with the buffer
    } 
}



CmdUploadBuffer::CmdUploadBuffer(CmdUploadBuffer &&other) : buffer(std::move(other.buffer)), staging(std::move(other.staging)), memory_rid(other.memory_rid), copy_region(other.copy_region) {
    other.staging = VK_NULL_HANDLE; // Reset the staging buffer in the moved-from object
    other.memory_rid = RID_INVALID; // Reset the memory RID in the moved-from object
}

CmdUploadBuffer::CmdUploadBuffer(VkBuffer buffer) : buffer(buffer), staging(VK_NULL_HANDLE), memory_rid(RID_INVALID) {}

CmdUploadBuffer::~CmdUploadBuffer() {
    if (staging != VK_NULL_HANDLE) {
        vkDestroyBuffer(BufferServer::instance().device, staging, nullptr); // Clean up the staging buffer
    }
    // vkDestroyBuffer(BufferServer::instance().device, staging, nullptr); // Clean up the staging buffer
    if (memory_rid != RID_INVALID) {
        GpuMemoryServer::instance().free_block(memory_rid); // Free the memory block associated with the staging buffer
    }
}

CmdUploadBuffer &CmdUploadBuffer::upload_data(VkDevice device, VkPhysicalDevice physical_device, VkDeviceSize size, const void *data) {
    RID rid;

    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT; // Staging buffer for transfer
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &buffer_info, nullptr, &staging) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create staging buffer!" << std::endl;
    #endif
        return *this;
    }

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(device, staging, &mem_requirements);

    memory_rid = GpuMemoryServer::instance().allocate_block<VkBuffer>(
        size,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        find_memory_type(physical_device, mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    );
    GpuMemoryServer::instance().bind(memory_rid, mem_requirements, staging);
    
    copy_region = {};
    copy_region.srcOffset = 0; // No offset for staging buffer
    copy_region.dstOffset = 0; // No offset for destination buffer
    copy_region.size = size; // Size of the data to copy

    // Copy data to the staging buffer
    void *mapped_data;
    GpuMemoryServer::instance().map(memory_rid, &mapped_data);
    memcpy(mapped_data, data, size);
    GpuMemoryServer::instance().unmap(memory_rid);

    return *this; // Return the command for chaining
}



BufferServer::BufferServer(VkDevice device, VkPhysicalDevice physical_device) : device(device), physical_device(physical_device) {
    buffers = std::vector<Buffer>();
    upload_commands = std::vector<CmdUploadBuffer>();
    command_types = std::vector<BufferCommandType>();
}

BufferServer::~BufferServer() {
    for (const auto &buffer : buffers) {
        vkDestroyBuffer(device, buffer.buffer, nullptr); // Clean up each buffer
    }
    buffers.clear(); // Clear the buffers vector
}

RID BufferServer::new_buffer(const VkBufferCreateInfo &create_info) {
    VkBuffer buffer;
    RID rid = RIDServer::instance().new_id(RIDServer::BUFFER);

    if (vkCreateBuffer(device, &create_info, nullptr, &buffer) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create buffer!" << std::endl;
    #endif
        return RID_INVALID; // Return an invalid RID on failure
    }

    buffers.emplace_back(buffer, rid, 0); // Add the created buffer to the buffers vector

    return rid;
}

RID BufferServer::new_buffer(VkBufferCreateInfo &&create_info) {
    VkBuffer buffer;
    RID rid = RIDServer::instance().new_id(RIDServer::BUFFER);

    if (vkCreateBuffer(device, &create_info, nullptr, &buffer) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create buffer!" << std::endl;
    #endif
        return RID_INVALID; // Return an invalid RID on failure
    }

    buffers.emplace_back(buffer, rid, 0); // Add the created buffer to the buffers vector

    return rid;
}

BufferBuilder BufferServer::new_buffer() {
    return BufferBuilder(*this); // Return a BufferBuilder instance for creating buffers
}

RID BufferServer::bind_buffer(RID buffer, RID memory) {
    VkMemoryRequirements mem_requirements;

    for (auto &buf : buffers) {
        if (buf.rid == buffer) {
            vkGetBufferMemoryRequirements(device, buf.buffer, &mem_requirements);
            buf.memory_rid = memory; // Set the memory RID for the buffer
            RID bind_rid = GpuMemoryServer::instance().bind(buf.memory_rid, mem_requirements, buf.buffer); // Bind the buffer to the GPU memory

            if (bind_rid == RID_INVALID) {
                #ifdef ALCHEMIST_DEBUG
                std::cerr << "Failed to bind buffer memory for RID: " << buffer << std::endl;
                #endif
                return bind_rid; // Return if binding fails
            }
            #ifdef ALCHEMIST_DEBUG
            std::cout << "Buffer with RID " << buffer << " bound to memory with RID " << memory << std::endl;
            #endif

            return bind_rid; // Exit after binding
        }
    }
    #ifdef ALCHEMIST_DEBUG
    std::cerr << "Buffer with RID " << buffer << " not found for binding!" << std::endl;
    #endif
    return RID_INVALID; // Return an invalid RID if the buffer is not found
}

void BufferServer::bind_best(RID buffer, VkMemoryPropertyFlags flags) {
    VkMemoryRequirements mem_requirements;

    for (auto &buf : buffers) {
        if (buf.rid == buffer) {
            vkGetBufferMemoryRequirements(device, buf.buffer, &mem_requirements);

            buf.memory_rid = GpuMemoryServer::instance().find_best<VkBuffer>(mem_requirements, flags);

            if (buf.memory_rid == RID_INVALID) {
                #ifdef ALCHEMIST_DEBUG
                std::cerr << "Failed to find suitable memory for buffer with RID: " << buffer << std::endl;
                #endif
                return; // Return if no suitable memory is found
            }

            RID bind_rid = GpuMemoryServer::instance().bind(buf.memory_rid, mem_requirements, buf.buffer);

            if (bind_rid == RID_INVALID) {
                #ifdef ALCHEMIST_DEBUG
                std::cerr << "Failed to bind buffer memory for RID: " << buffer << std::endl;
                #endif
                return; // Return if binding fails
            }
            #ifdef ALCHEMIST_DEBUG
            std::cout << "Buffer with RID " << buffer << " bound to best memory with RID " << buf.memory_rid << std::endl;
            #endif

            return;
        }
    }

    #ifdef ALCHEMIST_DEBUG
    std::cerr << "Buffer with RID " << buffer << " not found for binding!" << std::endl;
    #endif
}

const Buffer &BufferServer::get_buffer(RID rid) const {
    for (const auto &buffer : buffers) {
        if (buffer.rid == rid) {
            return buffer; // Return the buffer if found
        }
    }
    #ifdef ALCHEMIST_DEBUG
    std::cerr << "Buffer with RID " << rid << " not found!" << std::endl;
    #endif
    return *buffers.cend(); // Return the first buffer as a fallback (should be handled better)
}

void BufferServer::get_requirements(RID rid, VkMemoryRequirements &requirements) const {
    for (const auto &buffer : buffers) {
        if (buffer.rid == rid) {
            vkGetBufferMemoryRequirements(device, buffer.buffer, &requirements);
            return; // Return the requirements if the buffer is found
        }
    }
    #ifdef ALCHEMIST_DEBUG
    std::cerr << "Buffer with RID " << rid << " not found for memory requirements!" << std::endl;
    #endif
}

CmdUploadBuffer &BufferServer::upload_buffer(RID rid) {
    const Buffer &buffer = get_buffer(rid);
    
    if (buffer.rid == RID_INVALID) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Buffer with RID " << rid << " not found for upload!" << std::endl;
        #endif
        return *upload_commands.end(); // Return an invalid command if the buffer is not found
    }

    CmdUploadBuffer cmd(buffer.buffer);
    upload_commands.emplace_back(std::move(cmd));
    command_types.emplace_back(BufferCommandType::UPLOAD);

    return upload_commands.back(); // Return the last added command
}

void BufferServer::execute_commands(VkCommandBuffer cmd_buffer) {
    size_t upload_index = 0; // Index for upload commands
    size_t command_index = 0; // Index for command types

    for (const auto &command : command_types) {
        if (command == BufferCommandType::UPLOAD) {
            const auto &data = upload_commands[upload_index];
            vkCmdCopyBuffer(
                cmd_buffer,
                data.staging,
                data.buffer,
                1, // One region
                &data.copy_region
            );
            upload_index++; // Move to the next upload command
        } else {
            #ifdef ALCHEMIST_DEBUG
            std::cerr << "Unknown command type encountered!" << std::endl;
            #endif
        }
        command_index++;
    }
}

void BufferServer::clear_commands() {
    upload_commands.clear(); // Clear the upload commands
    command_types.clear(); // Clear the command types

    for (auto &buffer : buffers) {
        buffer.memory_rid = RID_INVALID; // Reset memory RID for each buffer
    }

    #ifdef ALCHEMIST_DEBUG
    std::cout << "Buffer commands cleared." << std::endl;
    #endif
} // Clear the command buffers, make sure to wait for the commands to

BufferServer &BufferServer::instance() {
    return *__instance; // Return the singleton instance of BufferServer
}

std::unique_ptr<BufferServer> BufferServer::__instance = nullptr; // Singleton instance of BufferServer