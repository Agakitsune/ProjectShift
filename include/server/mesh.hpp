
#ifndef ALCHEMIST_SERVER_MESH_HPP
#define ALCHEMIST_SERVER_MESH_HPP

#include <vector>
#include <cstdint>
#include <cstring>
#include <memory>

#include <vulkan/vulkan.h>

#include "server/rid.hpp"

struct Mesh {
    RID rid = RID_INVALID; // Resource ID for the mesh
    RID buffer = RID_INVALID; // RID for the buffer

    std::vector<uint64_t> offsets; // Offsets for the mesh data in the buffer, last is indices offset if present
    VkIndexType index_type = VK_INDEX_TYPE_MAX_ENUM; // Type of indices used in the mesh

    ~Mesh();

    void bind(VkCommandBuffer cmd_buffer) const;
};

struct MeshServer; // Forward declaration

struct MeshBuilder {
    void *data = nullptr; // Pointer to the mesh data
    
    uint64_t size = 0; // Size of the mesh data
    // uint64_t capacity = 0; // Capacity of the mesh data

    std::vector<uint64_t> offsets; // Offsets for the mesh data in the buffer, last is indices offset if present
    VkIndexType index_type = VK_INDEX_TYPE_MAX_ENUM; // Type of indices used in the mesh

    MeshServer &server; // Reference to the MeshServer for building meshes

    MeshBuilder(MeshServer &server);
    ~MeshBuilder();

    template <typename T>
    MeshBuilder &add_data(const T *data, uint64_t size) {
        if (index_type != VK_INDEX_TYPE_MAX_ENUM) {
            #ifdef ALCHEMIST_DEBUG
            std::cerr << "Index type already set, cannot add mesh data!" << std::endl;
            #endif
            return *this; // If index type is already set, return without adding data
        }

        this->data = realloc(this->data, this->size + size * sizeof(T)); // Allocate memory for the mesh data
        std::memcpy((uint8_t*)this->data + this->size, data, size * sizeof(T)); // Copy the data into the allocated memory
        offsets.push_back(this->size); // Add the current size as an offset
        this->size += size * sizeof(T); // Set the size of the mesh data
        return *this; // Return the builder for chaining
    }

    template <typename T>
    requires std::is_integral_v<T>
    MeshBuilder &add_indices(const T *data, uint64_t size) {
        if (index_type == VK_INDEX_TYPE_MAX_ENUM) {
            index_type = std::is_same_v<T, uint16_t> ? VK_INDEX_TYPE_UINT16 :
                          std::is_same_v<T, uint32_t> ? VK_INDEX_TYPE_UINT32 :
                          std::is_same_v<T, uint8_t> ? VK_INDEX_TYPE_UINT8 :
                          VK_INDEX_TYPE_MAX_ENUM; // Set the index type based on the type of T
        }
        this->data = realloc(this->data, this->size + size * sizeof(T)); // Allocate memory for the mesh data
        std::memcpy((uint8_t*)this->data + this->size, data, size * sizeof(T)); // Copy the indices into the allocated memory
        offsets.push_back(this->size); // Add the current size as an offset
        this->size += size * sizeof(T); // Set the size of the mesh data
        return *this; // Return the builder for chaining
    }

    RID build() const; // Create the mesh and return its RID
};

struct MeshServer {
    std::vector<Mesh> meshes; // Vector to hold all meshes
    
    VkDevice device;
    VkPhysicalDevice physical_device;

    MeshServer(VkDevice device, VkPhysicalDevice physical_device);

    MeshBuilder new_mesh();

    void bind_mesh(RID mesh, RID memory);

    void get_requirements(RID mesh, VkMemoryRequirements &requirements) const;

    const Mesh &get_mesh(RID mesh) const;

    static MeshServer &instance();

    static std::unique_ptr<MeshServer> __instance; // Singleton instance of MeshServer
};

#endif // ALCHEMIST_SERVER_MESH_HPP
