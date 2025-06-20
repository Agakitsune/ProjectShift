
#ifndef ALCHEMIST_GRAPHICS_MESH_DATA_HPP
#define ALCHEMIST_GRAPHICS_MESH_DATA_HPP

#include <vulkan/vulkan.h>

#include <cstdint> // for uint32_t
#include <concepts> // for std::is_integral_v
#include <typeinfo> // for typeid

#ifdef ALCHEMIST_DEBUG
#include <iostream> // for std::cout, std::cerr
#endif

#include "memory/vector.hpp"
#include "memory/table.hpp"
#include "memory/slice.hpp"
#include "memory/rid.hpp"
#include "graphics/meshes/mesh.hpp"
#include "graphics/rendering_device.hpp"

struct GpuUpdate {
    const void *data;
    uint32_t size;
    uint32_t offset;
};

struct MeshTableCpu {
    Table table;

    uint64_t type_hash;
    uint32_t dirty;

    MeshTableCpu() = default;
    MeshTableCpu(Table &&table, uint64_t type_hash)
        : table(std::move(table)), type_hash(type_hash), dirty(0) {}
};

struct MeshTableGpu {
    VkBuffer buffer = VK_NULL_HANDLE; // Vulkan buffer for the mesh data
    RID rid = 0; // Resource ID for the GPU memory block
    VkIndexType index_type = VK_INDEX_TYPE_MAX_ENUM; // Default index type

    MeshTableGpu() = default;
    MeshTableGpu(VkIndexType index_type)
        : index_type(index_type) {}
    ~MeshTableGpu();

    void upload(VkCommandBuffer cmd_buffer, const RenderingDevice &device, const GpuUpdate &update);
};

struct MeshBuilder;

struct MeshServer {
    alchemist::vector<MeshTableCpu> cpu;
    alchemist::vector<MeshTableGpu> gpu;

    VkCommandPool command_pool;

    MeshServer(const RenderingDevice &device);
    ~MeshServer();

    template <typename T>
    void add_table() {
        // static_assert(sizeof(T) & 3 == 0, "Mesh data must be aligned to 4 bytes");
        uint32_t stride = sizeof(T);
        cpu.push(MeshTableCpu(Table(stride), typeid(T).hash_code()));
        gpu.push(MeshTableGpu());
    }

    template <typename N>
    requires std::is_integral_v<N>
    void add_index_table() {
        // static_assert(sizeof(T) & 3 == 0, "Mesh data must be aligned to 4 bytes");
        uint32_t stride = sizeof(N);
        cpu.push(MeshTableCpu(Table(stride), typeid(N).hash_code()));
        gpu.push(MeshTableGpu((stride == 1) ? VK_INDEX_TYPE_UINT8 : (stride == 2) ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32));
    }

    template <typename T>
    uint32_t has_table() const {
        // static_assert(sizeof(T) & 3 == 0, "Mesh data must be aligned to 4 bytes");
        uint64_t type_hash = typeid(T).hash_code();
        for (uint32_t i = 0; i < cpu.size; ++i) {
            if (cpu.data[i].type_hash == type_hash) {
                return 1; // Return the index of the table
            }
        }
        return 0; // Not found
    }

    Slice load_data(uint64_t hash, void *data, uint32_t count);

    void unload_data(uint64_t hash, void *data, uint32_t count);
    Slice duplicate_data(uint64_t hash, uint64_t src, uint32_t count);

    VkCommandBuffer upload_gpu_data(const RenderingDevice &device, uint32_t index);
    VkCommandBuffer upload_gpu_data(const RenderingDevice &device);

    void cmd_upload_gpu_data(VkCommandBuffer cmd_buffer, const RenderingDevice &device, uint32_t index);
    void cmd_upload_gpu_data(VkCommandBuffer cmd_buffer, const RenderingDevice &device);

    void cmd_bind_buffers(VkCommandBuffer cmd_buffer, const Mesh &mesh, uint32_t first_binding = 0);

    uint32_t find_table(const Slice &slice) const;

    MeshBuilder mesh(uint32_t entries = 0);
};

MeshServer &new_mesh_server(const RenderingDevice &device);
MeshServer &get_mesh_server();

extern MeshServer *mesh_server; // Global mesh server instance

struct MeshBuilder {
    alchemist::dual_vector<void*, uint32_t> data; // Pointers to the data for the mesh tables
    alchemist::vector<uint64_t> hashes; // Type hashes for the mesh tables
    
    VkIndexType index_type = VK_INDEX_TYPE_UINT32; // Default index type
    void *indices = nullptr; // Pointer to the indices for the mesh
    uint32_t index_count = 0; // Count of indices
    
    MeshServer &server;

    MeshBuilder(MeshServer &server, uint32_t entries = 0);

    template <typename T>
    void add_data(T *value, uint32_t size) {
        if (!server.has_table<T>()) {
            server.add_table<T>();
        }

        data.push(const_cast<T*>(value), size * sizeof(T));
        hashes.push(typeid(T).hash_code());
    }

    template <typename T>
    void modify_entry(uint32_t index, T *value, uint32_t size) {
        data.data1[index] = value;
        data.data2[index] = size * sizeof(T);
        hashes.data[index] = typeid(T).hash_code();
    }

    template <typename N>
    requires std::is_integral_v<N>
    void add_indices(const N *indices, uint32_t count) {
        this->indices = const_cast<N*>(indices); // Reset indices pointer
        index_count = count;
        index_type = (sizeof(N) == 1) ? VK_INDEX_TYPE_UINT8 : (sizeof(N) == 2) ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32;
    }

    template <typename N>
    requires std::is_integral_v<N>
    void modify_indices(uint32_t index, const N *indices, uint32_t count) {
        this->indices = const_cast<N*>(indices);
        index_count = count;
        index_type = (sizeof(N) == 1) ? VK_INDEX_TYPE_UINT8 : (sizeof(N) == 2) ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32;
    }

    Mesh build() const;
};

#endif // ALCHEMIST_GRAPHICS_MESH_DATA_HPP
