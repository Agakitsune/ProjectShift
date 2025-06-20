
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
    VkDeviceSize offset; // Offset in the device memory
    VkDeviceSize size; // Size of the data in bytes

    RID rid; // Resource ID for tracking

    Bind() = default;
    ~Bind() {
        get_rid_server().free(get_rid_atlas().BIND, rid); // Free the RID when the bind is destroyed
    }
};



enum GpuDeviceMemoryAllocationMethod {
    Linear,
    Geometric,
};



template <typename T, GpuDeviceMemoryAllocationMethod M = Linear>
struct GpuDeviceMemory {
    std::vector<Bind<T>> binds; // Vector of binds on this GPU Device Memory

    void (*__reallocate)(GpuDeviceMemory<T> &, VkDevice, VkDeviceSize) = nullptr; // Function pointer for reallocation logic

    VkDeviceMemory device; // Device memory object

    uint32_t type_idx = 0; // Memory type index

    VkMemoryPropertyFlags properties = 0; // Memory properties (e.g., VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)

    VkDeviceSize capacity = 0; // Total capacity of the memory block in bytes
    VkDeviceSize size = 0; // Current size of the memory block in bytes

    RID rid = 0; // Resource ID for tracking

    ~GpuDeviceMemory() {
        get_rid_server().free(get_rid_atlas().MEMORY, rid);
    }

    void allocate(VkDevice dev, VkDeviceSize capacity) {
        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = capacity;
        alloc_info.memoryTypeIndex = type_idx;

        this->capacity = capacity;

        if (vkAllocateMemory(dev, &alloc_info, nullptr, &device) != VK_SUCCESS) {
            #ifdef ALCHEMIST_DEBUG
            std::cerr << "Failed to allocate GPU memory!" << std::endl;
            #endif
            device = VK_NULL_HANDLE; // Reset to null on failure
        }
    }

    void reallocate(VkDevice dev, VkDeviceSize new_capacity) {
        __reallocate(*this, dev, new_capacity);
    }

    uint32_t is_valid(const VkMemoryRequirements &requirements) const {
        if (requirements.memoryTypeBits & (1 << type_idx) == 0) {
            #ifdef ALCHEMIST_DEBUG
            std::cerr << "Memory type index does not match the requirements!" << std::endl;
            #endif
            return 0; // Return 0 if the memory type index does not match
        }

        // if (size + requirements.size > capacity) {
        //     #ifdef ALCHEMIST_DEBUG
        //     std::cerr << "Not enough capacity to allocate memory!" << std::endl;
        //     #endif
        //     return 0; // Return 0 if there is not enough capacity
        // }

        return 1; // Return 1 if the memory is valid for allocation
    }

    RID bind(VkDevice dev, const VkMemoryRequirements &requirements, T data) {
        VkDeviceSize aligned_size = (requirements.size + requirements.alignment - 1) & ~(requirements.alignment - 1);

        if (requirements.memoryTypeBits & (1 << type_idx) == 0) {
            #ifdef ALCHEMIST_DEBUG
            std::cerr << "Memory type index does not match the requirements!" << std::endl;
            #endif
            return RID_INVALID; // Return 0 if the memory type index does not match
        }

        if (size + aligned_size > capacity) {
            if constexpr (M == Linear) {
                uint32_t i = 2;
                while (size + aligned_size > (capacity * i)) {
                    i++;
                }
                capacity *= i;
            } else if constexpr (M == Geometric) {
                while (size + aligned_size > capacity) {
                    capacity <<= 1; // Double the capacity
                }
            }
            reallocate(dev, capacity);
        }

        Bind<T> bind_info;
        bind_info.data = data;
        bind_info.offset = size;
        bind_info.size = aligned_size;
        bind_info.rid = new_id(get_rid_atlas().BIND); // Generate a new RID for the bind

        binds.emplace_back(std::move(bind_info)); // Add the bind to the vector
        BindInterface<T>::bind(dev, data, this->device, size);

        size += aligned_size; // Update the current size

        return bind_info.rid; // Return the RID of the bind
    }

    // will not succeed if the memory can't be mapped, there may be dragons
    void map(VkDevice dev, void **data) const {
        if (properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
            #ifdef ALCHEMIST_DEBUG
            std::cerr << "Mapping GPU memory of size: " << size << " bytes." << std::endl;
            #endif
        } else {
            #ifdef ALCHEMIST_DEBUG
            std::cerr << "Cannot map GPU memory, not host visible!" << std::endl;
            #endif
            *data = nullptr; // Reset data pointer if not mappable
            return;
        }
        
        if (vkMapMemory(dev, this->device, 0, size, 0, data) != VK_SUCCESS) {
            #ifdef ALCHEMIST_DEBUG
            std::cerr << "Failed to map GPU memory!" << std::endl;
            #endif
            *data = nullptr; // Reset data pointer on failure
        }
    }

    void map_bind(VkDevice dev, RID rid, void **data) const {
        if (properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
            #ifdef ALCHEMIST_DEBUG
            std::cerr << "Mapping GPU memory for bind with RID: " << rid << std::endl;
            #endif
        } else {
            #ifdef ALCHEMIST_DEBUG
            std::cerr << "Cannot map GPU memory for bind, not host visible!" << std::endl;
            #endif
            *data = nullptr; // Reset data pointer if not mappable
            return;
        }

        for (const auto &bind : binds) {
            if (bind.rid == rid) {
                if (vkMapMemory(dev, this->device, bind.offset, bind.size, 0, data) != VK_SUCCESS) {
                    #ifdef ALCHEMIST_DEBUG
                    std::cerr << "Failed to map GPU memory for bind with RID: " << rid << std::endl;
                    #endif
                    *data = nullptr; // Reset data pointer on failure
                } else {
                    #ifdef ALCHEMIST_DEBUG
                    std::cerr << "Mapped GPU memory for bind with RID: " << rid << std::endl;
                    #endif
                }
                return; // Return if bind is found and mapped successfully
            }
        }
        *data = nullptr; // If bind not found, reset data pointer
    }

    void unmap(VkDevice dev) const {
        if (properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
            vkUnmapMemory(dev, this->device); // Unmap the memory
            #ifdef ALCHEMIST_DEBUG
            std::cerr << "Unmapped GPU memory of size: " << size << " bytes." << std::endl;
            #endif
        } else {
            #ifdef ALCHEMIST_DEBUG
            std::cerr << "Cannot unmap GPU memory, not host visible!" << std::endl;
            #endif
        }
    }

    const Bind<T> &get_bind(RID rid) const {
        for (const auto &bind : binds) {
            if (bind.rid == rid) {
                return bind; // Return the bind if found
            }
        }
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Bind with RID " << rid << " not found!" << std::endl;
        #endif
        return *((const Bind<T> *)nullptr); // Return a null bind if not found
    }
};

struct GpuMemoryServer {
    std::vector<GpuDeviceMemory<VkBuffer>> buffers_memory; // Vector of GPU memory blocks for buffers
    std::vector<GpuDeviceMemory<VkImage>> images_memory; // Vector of GPU memory blocks for images

    VkDevice device;
    VkPhysicalDevice physical_device;

    GpuMemoryServer(VkDevice device, VkPhysicalDevice physical_device) : device(device), physical_device(physical_device) {
        buffers_memory = std::vector<GpuDeviceMemory<VkBuffer>>();
        images_memory = std::vector<GpuDeviceMemory<VkImage>>();
    }

    ~GpuMemoryServer() {
        for (auto &block : buffers_memory) {
            vkFreeMemory(device, block.device, nullptr); // Free each buffer memory block
        }
        for (auto &block : images_memory) {
            vkFreeMemory(device, block.device, nullptr); // Free each image memory block
        }
    }

    template <typename T>
    RID allocate_block(VkDeviceSize size, VkMemoryPropertyFlags flags, uint32_t type_index) {
        GpuDeviceMemory<T> block;
        block.type_idx = type_index;
        block.properties = flags;
        block.allocate(device, size);
        block.rid = new_id(get_rid_atlas().MEMORY); // Generate a new RID for the block
        
        if constexpr (std::is_same_v<T, VkBuffer>) {
            buffers_memory.emplace_back(std::move(block)); // Move the block into the vector
        } else if constexpr (std::is_same_v<T, VkImage>) {
            images_memory.emplace_back(std::move(block));
        }

        #ifdef ALCHEMIST_DEBUG
        std::cout << "Allocated GPU memory block with RID: " << block.rid << " of size: " << size << " bytes." << std::endl;
        #endif
        
        return block.rid; // Return the RID of the new block
    }

    void free_block(RID rid) {
        #ifdef ALCHEMIST_DEBUG
        std::cout << "Freeing GPU memory block with RID: " << rid << std::endl;
        #endif
        for (auto it = buffers_memory.begin(); it != buffers_memory.end(); ++it) {
            if (it->rid == rid) {
                vkFreeMemory(device, it->device, nullptr); // Free the buffer memory
                buffers_memory.erase(it); // Remove the block from the vector
                return;
            }
        }
        for (auto it = images_memory.begin(); it != images_memory.end(); ++it) {
            if (it->rid == rid) {
                vkFreeMemory(device, it->device, nullptr); // Free the image memory
                images_memory.erase(it); // Remove the block from the vector
                return;
            }
        }
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to free memory block with RID: " << rid << std::endl;
        #endif
    }

    template <typename T>
    const GpuDeviceMemory<T> &get_memory_block(RID rid) const {
        if constexpr (std::is_same_v<T, VkBuffer>) {
            for (const auto &block : buffers_memory) {
                if (block.rid == rid) {
                    return block; // Return the block if found
                }
            }
        } else if constexpr (std::is_same_v<T, VkImage>) {
           for (const auto &block : images_memory) {
                if (block.rid == rid) {
                    return block; // Return the block if found
                }
            }
        }
        return *((const GpuDeviceMemory<T> *)nullptr); // Return the first block if not found (or handle error appropriately)
    }

    uint32_t is_valid(RID rid, const VkMemoryRequirements &requirements) const {
        for (const auto &block : buffers_memory) {
            if (block.rid == rid) {
                return block.is_valid(requirements); // Check if the buffer memory is valid
            }
        }
        for (const auto &block : images_memory) {
            if (block.rid == rid) {
                return block.is_valid(requirements); // Check if the image memory is valid
            }
        }
        return 0; // Return 0 if not found
    }

    void map(RID rid, void **data) const {
        for (const auto &block : buffers_memory) {
            if (block.rid == rid) {
                block.map(device, data); // Map the buffer memory block
                return;
            }
        }
        for (const auto &block : images_memory) {
            if (block.rid == rid) {
                block.map(device, data); // Map the image memory block
                return;
            }
        }
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to map memory block with RID: " << rid << std::endl;
        #endif
        *data = nullptr; // Reset data pointer if not found
    }

    void unmap(RID rid) const {
        for (const auto &block : buffers_memory) {
            if (block.rid == rid) {
                block.unmap(device); // Unmap the buffer memory block
                return;
            }
        }
        for (const auto &block : images_memory) {
            if (block.rid == rid) {
                block.unmap(device); // Unmap the image memory block
                return;
            }
        }
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to unmap memory block with RID: " << rid << std::endl;
        #endif
    }

    template <typename T>
    RID find_best(const VkMemoryRequirements &requirements, VkMemoryPropertyFlags flags) {
        if constexpr (std::is_same_v<T, VkBuffer>) {
            for (const auto &block : buffers_memory) {
                if (block.is_valid(requirements) && 
                    (block.properties & flags) == flags) {
                    return block.rid; // Return the first valid memory block
                }
            }
        } else if constexpr (std::is_same_v<T, VkImage>) {
            for (const auto &block : images_memory) {
                if (block.is_valid(requirements) && 
                    (block.properties & flags) == flags) {
                    return block.rid; // Return the first valid memory block
                }
            }
        }

        return RID_INVALID; // Return 0 if no valid memory block is found
    }

    template <typename T>
    RID bind(RID rid, const VkMemoryRequirements &requirements, T data) {
        if constexpr (std::is_same_v<T, VkBuffer>) {
            for (auto &block : buffers_memory) {
                if (block.rid == rid) {
                    return block.bind(device, requirements, data); // Bind the buffer memory and return the RID
                }
            }
        } else if constexpr (std::is_same_v<T, VkImage>) {
            for (auto &block : images_memory) {
                if (block.rid == rid) {
                    return block.bind(device, requirements, data); // Bind the image memory and return the RID
                }
            }
        }
        return RID_INVALID; // Return 0 if not found
    }
};

GpuMemoryServer &new_gpu_memory_server(VkDevice device, VkPhysicalDevice physical_device);
GpuMemoryServer &get_gpu_memory_server();

extern GpuMemoryServer *gpu_memory_server;

#endif // ALCHEMIST_MEMORY_GPU_HPP
