
// #include "graphics/meshes/mesh_server.hpp"

// MeshBuilder::MeshBuilder(MeshServer &server, uint64_t initial_capacity) : server(server) {
//     this->capacity = initial_capacity;
//     this->data = malloc(initial_capacity);
//     if (!this->data) {
//         #ifdef ALCHEMIST_DEBUG
//         std::cerr << "Failed to allocate memory for mesh builder!" << std::endl;
//         #endif
//         this->capacity = 0; // Set capacity to 0 if allocation fails
//     }
//     this->size = 0;
//     this->index_offset = 0;
//     this->index_type = VK_INDEX_TYPE_MAX_ENUM; // Default index type
// }

// RID MeshBuilder::build() const {
//     VkBufferCreateInfo buffer_info = {};
//     buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//     buffer_info.size = size;
//     buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
//     buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

//     VkBuffer vertex_buffer;
//     if (vkCreateBuffer(server.device, &buffer_info, nullptr, &vertex_buffer) != VK_SUCCESS) {
//         #ifdef ALCHEMIST_DEBUG
//         std::cerr << "Failed to create vertex buffer!" << std::endl;
//         #endif
//         return RID_INVALID; // Return an invalid RID on failure
//     }

//     Mesh mesh;
//     mesh.buffer = vertex_buffer;
//     mesh.index_offset = index_offset;
//     mesh.index_type = index_type;
//     mesh.rid = RIDServer::instance().new_id(RIDServer::MESH);
//     mesh.gpu_rid = 0;

//     server.meshes.emplace_back(std::move(mesh));

//     return server.meshes.back().rid;
// }
