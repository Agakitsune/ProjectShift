
#ifndef ALCHEMIST_MEMORY_GPU_HPP
#define ALCHEMIST_MEMORY_GPU_HPP

#ifdef ALCHEMIST_DEBUG
#include <iostream>
#endif // ALCHEMIST_DEBUG

#include <vulkan/vulkan.h>

#include "memory/vector.hpp"
#include "memory/rid.hpp"

template <typename T>
struct BindInterface {};

template <>
struct BindInterface<VkBuffer> {
    static constexpr void bind(VkDevice device, VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize offset) {
        vkBindBufferMemory(device, buffer, memory, offset);
    }
};

template <>
struct BindInterface<VkImage> {
    static constexpr void bind(VkDevice device, VkImage image, VkDeviceMemory memory, VkDeviceSize offset) {
        vkBindImageMemory(device, image, memory, offset);
    }
};

template <typename T>
struct Bind {
    T data;
    VkDeviceSize offset;
    VkDeviceSize size; // Size of the buffer or image in bytes
};

template <typename T>
struct MemoryBlock {
    std::vector<Bind<T>> binds;

    VkDeviceMemory device; // Device memory object
    VkMemoryRequirements requirements; // Memory requirements for the data to be stored in this block
    VkMemoryPropertyFlags properties; // Memory properties (e.g., VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    VkDeviceSize consumed = 0;
    uint32_t type_idx;
    RID rid;

    MemoryBlock() {
        rid = new_id(0);
    }

    MemoryBlock(VkMemoryRequirements reqs, VkMemoryPropertyFlags props, uint32_t type_index)
        : requirements(reqs), properties(props), type_idx(type_index) {
        binds = std::vector<Bind<T>>(0);
        rid = new_id(get_rid_atlas().MEMORY);
    }

    void allocate(VkDevice dev, VkDeviceSize block_size, VkDeviceSize required_size) {
        uint32_t i = 1;

        while ((block_size * i) < required_size) {
            i++;
        }

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = block_size * i;
        alloc_info.memoryTypeIndex = type_idx;

        std::cout << "Allocating memory block " << rid << " of size " << alloc_info.allocationSize << " bytes." << std::endl;

        requirements.size = alloc_info.allocationSize; // Update the requirements size

        if (vkAllocateMemory(dev, &alloc_info, nullptr, &device) != VK_SUCCESS) {
        #ifdef ALCHEMIST_DEBUG
            std::cerr << "Failed to allocate memory for buffer!" << std::endl;
        #endif
            return;
        }
    }

    void reallocate(VkDevice dev, VkDeviceSize block_size, VkDeviceSize required_size) {
        uint32_t i = 1;
        VkDeviceSize remaining_size = requirements.size - consumed;
        VkDeviceMemory old_memory = device;
        
        while ((remaining_size + block_size * i) < required_size) {
            i++;
        }

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = requirements.size + block_size * i;
        alloc_info.memoryTypeIndex = type_idx;

        if (vkAllocateMemory(dev, &alloc_info, nullptr, &device) != VK_SUCCESS) {
        #ifdef ALCHEMIST_DEBUG
            std::cerr << "Failed to reallocate memory for buffer!" << std::endl;
        #endif
            return;
        }

        // Rebind existing buffers to the new memory block
        for (uint32_t j = 0; j < binds.size; ++j) {
            Bind<T> &bind_info = binds.data[j];
            if (bind_info.size > 0) {
                BindInterface<T>::bind(dev, bind_info.data, device, bind_info.offset);
            }
        }

        // Copy the data from the old memory block to the new one
        VkDeviceSize old_size = requirements.size;
        VkDeviceSize new_size = requirements.size + block_size * i;
        std::cout << "Reallocating memory block " << rid << " from size " << old_size << " to " << new_size << std::endl;
        if (old_size > 0) {
            void *old_data;
            std::cout << "Copying data from old memory block to new memory block." << std::endl;
            vkMapMemory(dev, old_memory, 0, old_size, 0, &old_data);
            void *new_data;
            vkMapMemory(dev, device, 0, new_size, 0, &new_data);
            memcpy(new_data, old_data, old_size);
            vkUnmapMemory(dev, old_memory);
            vkUnmapMemory(dev, device);
        }

        // Free the old memory block
        vkFreeMemory(dev, old_memory, nullptr);

        // Update the block's requirements and consumed size
        requirements.size += block_size * i;
    }

    uint32_t find_data(T data) const {
        for (uint32_t i = 0; i < binds.size; ++i) {
            if (binds.data[i].data == data) {
                return i; // Return the index of the buffer
            }
        }
        return UINT32_MAX; // Not found
    }

    void remove(VkDevice dev, uint32_t index) {
        uint8_t *new_data;
        VkDeviceSize offset = 0;

        for (uint32_t i = 0; i < index; ++i) {
            offset += binds.data[i].size;
        }

        vkMapMemory(dev, device, 0, requirements.size, 0, (void**)&new_data);
        memcpy(new_data + offset, new_data + offset + binds.data[index].size, requirements.size - offset - binds.data[index].size);
        vkUnmapMemory(dev, device);
        
        consumed -= binds.data[index].size;
        
        binds.remove(index);
    }

    void map(VkDevice dev, T data, void **mapping) const {
        uint32_t index = find_data(data);
        if (index != UINT32_MAX) {
            std::cout << "Mapping memory for data at index: " << index << std::endl;
            vkMapMemory(dev, device, binds.data[index].offset, binds.data[index].size, 0, mapping);
        } else {
            *mapping = nullptr; // Data not found
        }
    }
};

struct GpuMemoryServer {
    std::vector<MemoryBlock<VkBuffer>> buffers_memory;
    std::vector<MemoryBlock<VkImage>> images_memory;
    VkDeviceSize block_size;

    VkDevice device;
    VkPhysicalDevice physical_device;

    GpuMemoryServer(VkDeviceSize block_size = 1024 * 1024) : block_size(block_size) {};
    ~GpuMemoryServer();

    RID bind_buffer(VkBuffer buffer, VkDeviceSize size, VkMemoryPropertyFlags properties);
    RID bind_buffer(VkBuffer buffer, VkDeviceSize size, RID rid, VkMemoryPropertyFlags properties);
    RID new_bind_buffer(VkBuffer buffer, VkDeviceSize size, VkMemoryPropertyFlags properties);
    void unbind_buffer(VkBuffer buffer);
    void unbind_buffer(VkBuffer buffer, RID rid);

    RID bind_image(VkImage image, VkDeviceSize size, VkMemoryPropertyFlags properties);
    RID bind_image(VkImage image, VkDeviceSize size, RID rid, VkMemoryPropertyFlags properties);
    RID new_bind_image(VkImage image, VkDeviceSize size, VkMemoryPropertyFlags properties);
    void unbind_image(VkImage image);
    void unbind_image(VkImage image, RID rid);

    template <typename T>
    const MemoryBlock<T> &get_memory_block(RID rid) const {
        if constexpr (std::is_same_v<T, VkBuffer>) {
            for (uint32_t i = 0; i < buffers_memory.size; ++i) {
                if (buffers_memory.data[i].rid == rid) {
                    return buffers_memory.data[i];
                }
            }
        } else if constexpr (std::is_same_v<T, VkImage>) {
            for (uint32_t i = 0; i < images_memory.size; ++i) {
                if (images_memory.data[i].rid == rid) {
                    return images_memory.data[i];
                }
            }
        }
    }
};

// GpuMemoryServer &new_gpu_memory_server(VkDevice device, VkPhysicalDevice physical_device, VkDeviceSize block_size = 1024 * 1024);
// GpuMemoryServer &get_gpu_memory_server();

// extern GpuMemoryServer *gpu_memory_server;

#endif // ALCHEMIST_MEMORY_GPU_HPP
