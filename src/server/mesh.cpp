
#ifdef ALCHEMIST_DEBUG
#include <iostream>
#endif // ALCHEMIST_DEBUG

#include "server/mesh.hpp"
#include "server/buffer.hpp"

Mesh::~Mesh() {
    if (rid != RID_INVALID) {
        #ifdef ALCHEMIST_DEBUG
        std::cout << "Destroying mesh with RID: " << rid << std::endl;
        #endif
        RIDServer::instance().free(RIDServer::MESH, rid); // Free the RID of the mesh
    }
    if (buffer != RID_INVALID) {
        RIDServer::instance().free(RIDServer::BUFFER, buffer); // Free the RID of the buffer
    }
}

void Mesh::bind(VkCommandBuffer cmd_buffer) const {
    if (buffer == RID_INVALID) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Mesh with RID " << rid << " has no valid buffer!" << std::endl;
        #endif
        return; // If the buffer is invalid, do not bind
    }
    
    const Buffer &buf = BufferServer::instance().get_buffer(buffer); // Get the buffer from the server
    uint32_t count = offsets.size();

    if (index_type != VK_INDEX_TYPE_MAX_ENUM) {
        count--; // If there are indices, reduce the count by one
        vkCmdBindIndexBuffer(cmd_buffer, buf.buffer, offsets.back(), index_type); // Bind the index buffer
    }

    VkBuffer *buffers = new VkBuffer[count]; // Create an array of buffers
    for (uint32_t i = 0; i < count; ++i) {
        buffers[i] = buf.buffer; // Fill the array with the mesh buffer
    }

    VkDeviceSize *offsets_ptr = new VkDeviceSize[count]; // Create an array for offsets
    for (uint32_t i = 0; i < count; ++i) {
        offsets_ptr[i] = this->offsets[i]; // Fill the offsets array
    }

    vkCmdBindVertexBuffers(cmd_buffer, 0, count, buffers, offsets_ptr); // Bind the vertex buffers
}



MeshBuilder::MeshBuilder(MeshServer &server) : server(server) {
    data = nullptr; // Initialize data pointer to nullptr
    size = 0; // Initialize size to 0
    index_type = VK_INDEX_TYPE_MAX_ENUM; // Set index type to an invalid value
}

MeshBuilder::~MeshBuilder() {
    if (data) {
        free(data); // Free the allocated data if it exists
    }
}

RID MeshBuilder::build() const {
    Mesh mesh;

    RID rid = RIDServer::instance().new_id(RIDServer::MESH); // Generate a new RID for the mesh
    mesh.rid = rid; // Create a new RID for the mesh
    if (mesh.rid == RID_INVALID) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create mesh RID!" << std::endl;
        #endif
        return RID_INVALID; // Return an invalid RID if creation fails
    }
    VkBufferUsageFlagBits usage = (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT); // Set the usage for the buffer
    if (index_type != VK_INDEX_TYPE_MAX_ENUM) {
        usage = (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |  VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT); // If indices are present, also set the index buffer usage
    }
    mesh.buffer = BufferServer::instance().new_buffer()
        .set_usage(usage)
        .set_size(size)
        .set_sharing_mode(VK_SHARING_MODE_EXCLUSIVE)
        .build(); // Create a new buffer for the mesh
    
    BufferServer::instance().upload_buffer(mesh.buffer)
        .upload_data(server.device, server.physical_device, size, data); // Upload the mesh data to the buffer
    
    mesh.offsets = std::move(offsets); // Move the offsets into the mesh
    mesh.index_type = index_type; // Set the index type for the mesh
    
    server.meshes.emplace_back(std::move(mesh)); // Add the mesh to the server's meshes vector

    #ifdef ALCHEMIST_DEBUG
    std::cout << "Created mesh with RID: " << mesh.rid << ", buffer RID: " << mesh.buffer << std::endl;
    #endif

    mesh.rid = RID_INVALID;
    mesh.buffer = RID_INVALID;
    
    return rid; // If the buffer creation fails, it will return an invalid RID
} // Create the mesh and return its RID



MeshServer::MeshServer(VkDevice device, VkPhysicalDevice physical_device) {
    this->device = device; // Set the Vulkan device
    this->physical_device = physical_device; // Set the Vulkan physical device
    meshes = std::vector<Mesh>(); // Initialize the meshes vector
}

MeshBuilder MeshServer::new_mesh() {
    return MeshBuilder(*this); // Return a MeshBuilder instance for creating meshes
}

void MeshServer::bind_mesh(RID mesh, RID memory) {
    for (auto &m : meshes) {
        if (m.rid == mesh) {
            BufferServer::instance().bind_buffer(m.buffer, memory); // Bind the mesh buffer to the specified memory
            return; // Exit after binding
        }
    }
    #ifdef ALCHEMIST_DEBUG
    std::cerr << "Mesh with RID " << mesh << " not found for binding!" << std::endl;
    #endif
}

void MeshServer::get_requirements(RID mesh, VkMemoryRequirements &requirements) const {
    for (const auto &m : meshes) {
        if (m.rid == mesh) {
            BufferServer::instance().get_requirements(m.buffer, requirements); // Get the memory requirements for the mesh buffer
            return; // Exit after getting requirements
        }
    }
    #ifdef ALCHEMIST_DEBUG
    std::cerr << "Mesh with RID " << mesh << " not found for memory requirements!" << std::endl;
    #endif
}

const Mesh &MeshServer::get_mesh(RID mesh) const {
    for (const auto &m : meshes) {
        if (m.rid == mesh) {
            return m; // Return the mesh if found
        }
    }
    #ifdef ALCHEMIST_DEBUG
    std::cerr << "Mesh with RID " << mesh << " not found!" << std::endl;
    #endif
    return *((const Mesh *)nullptr); // Return a null pointer if not found
}

MeshServer &MeshServer::instance() {
    return *__instance; // Return the singleton instance of MeshServer
}

std::unique_ptr<MeshServer> MeshServer::__instance = nullptr; // Singleton instance of MeshServer
