
// #include <cstring>

#include "graphics/meshes/mesh_server.hpp"
// #include "memory/gpu.hpp"

// static uint32_t find_memory_type(VkPhysicalDevice device, uint32_t type_filter, VkMemoryPropertyFlags properties) {
//     VkPhysicalDeviceMemoryProperties mem_properties;
//     vkGetPhysicalDeviceMemoryProperties(device, &mem_properties);

//     for (uint32_t i = 0; i < mem_properties.memoryTypeCount; ++i) {
//         if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
//             return i;
//         }
//     }
//     return UINT32_MAX; // Not found
// }

// inline VkBuffer create_buffer(VkDevice device, uint32_t size, VkBufferUsageFlags usage) {
//     VkBuffer buffer = VK_NULL_HANDLE;

//     VkBufferCreateInfo buffer_info{};
//     buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//     buffer_info.size = size;
//     buffer_info.usage = usage;
//     buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
//     if (vkCreateBuffer(device, &buffer_info, nullptr, &buffer) != VK_SUCCESS) {
//     #ifdef ALCHEMIST_DEBUG
//         std::cerr << "Failed to create buffer!" << std::endl;
//     #endif
//         return VK_NULL_HANDLE;
//     }

//     return buffer;
// }

// inline VkDeviceMemory create_memory(VkDevice device, VkPhysicalDevice physical_device, VkMemoryPropertyFlags properties, VkBuffer buffer) {
//     VkDeviceMemory memory = VK_NULL_HANDLE;
//     VkMemoryRequirements mem_requirements;
//     vkGetBufferMemoryRequirements(device, buffer, &mem_requirements);

//     VkMemoryAllocateInfo alloc_info{};
//     alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//     alloc_info.allocationSize = mem_requirements.size;
//     alloc_info.memoryTypeIndex = find_memory_type(physical_device, mem_requirements.memoryTypeBits, properties);
    
//     if (vkAllocateMemory(device, &alloc_info, nullptr, &memory) != VK_SUCCESS) {
//     #ifdef ALCHEMIST_DEBUG
//         std::cerr << "Failed to allocate memory for buffer!" << std::endl;
//     #endif
//         return VK_NULL_HANDLE;
//     }

//     vkBindBufferMemory(device, buffer, memory, 0);

//     return memory;
// }

// MeshTableGpu::~MeshTableGpu() {
//     if (buffer != VK_NULL_HANDLE) {
//         vkDestroyBuffer(rendering_device->device, buffer, nullptr);
//         // get_gpu_memory_server().unbind_buffer(buffer, rid);
//     }
// }

// void MeshTableGpu::upload(VkCommandBuffer cmd_buffer, const RenderingDevice &device, const GpuUpdate &update) {
//     VkBuffer staging;
//     uint8_t *data;
//     GpuMemoryServer &gpu_memory_server = get_gpu_memory_server();
//     RID staging_rid;

//     if (buffer != VK_NULL_HANDLE) {
//         gpu_memory_server.unbind_buffer(buffer, rid);
//         vkDestroyBuffer(device.device, buffer, nullptr);
//     }

//     std::cout << "Uploading mesh data to GPU: " << update.size << " bytes at offset " << update.offset << std::endl;

//     buffer = create_buffer(device.device, update.size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
//     staging = create_buffer(device.device, update.size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

//     std::cout << staging << " staging buffer created for mesh data upload." << std::endl;

//     rid = gpu_memory_server.bind_buffer(buffer, update.size, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
//     std::cout << "Uploading mesh data to GPU: " << update.size << " bytes at offset " << update.offset << std::endl;
//     staging_rid = gpu_memory_server.bind_buffer(staging, update.size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

//     std::cout << "Uploading mesh data to GPU: " << update.size << " bytes at offset " << update.offset << std::endl;

//     auto stating_block = gpu_memory_server.get_memory_block<VkBuffer>(staging_rid);
    
//     stating_block.map(device.device, staging, (void**)&data);

//     memcpy(data, update.data, update.size);
//     vkUnmapMemory(device.device, stating_block.device);

//     VkBufferCopy copy{};
//     copy.size = update.size;
//     copy.srcOffset = 0;
//     copy.dstOffset = update.offset;
    
//     vkCmdCopyBuffer(cmd_buffer, staging, buffer, 1, &copy);
// }



// MeshServer::MeshServer(const RenderingDevice &device) {
//     // Create command pool for mesh updates
//     VkCommandPoolCreateInfo pool_info{};
//     pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
//     pool_info.queueFamilyIndex = device.transfer_queue_family_index; // Use the transfer queue family for command pool
//     pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

//     if (vkCreateCommandPool(device.device, &pool_info, nullptr, &command_pool) != VK_SUCCESS) {
//     #ifdef ALCHEMIST_DEBUG
//         std::cerr << "Failed to create command pool!" << std::endl;
//     #endif
//         command_pool = VK_NULL_HANDLE;
//     }

//     add_index_table<uint8_t>(); // Add a default table for uint8_t data
//     add_index_table<uint16_t>(); // Add a default table for uint16_t data
//     add_index_table<uint32_t>(); // Add a default table for uint32_t data
// }

// MeshServer::~MeshServer() {
//     vkDestroyCommandPool(rendering_device->device, command_pool, nullptr);
// }

// Slice MeshServer::load_data(uint64_t hash, void *data, uint32_t count) {
//     uint64_t offset = 0;

//     for (uint32_t i = 0; i < cpu.size; ++i) {
//         if (cpu.data[i].type_hash == hash) {
//             // Found the table, load the data
//             offset = cpu.data[i].table.load(count, data);
//             cpu.data[i].dirty = 1; // Mark the table as dirty
//             return Slice((uint8_t*)cpu.data[i].table.data + offset, offset, count * cpu.data[i].table.stride);
//         }
//     }

//     return Slice();
// }

// void MeshServer::unload_data(uint64_t hash, void *data, uint32_t count) {
//     for (uint32_t i = 0; i < cpu.size; ++i) {
//         if (cpu.data[i].type_hash == hash) {
//             // Found the table, unload the data
//             cpu.data[i].table.unload(reinterpret_cast<uint64_t>(data), count);
//             cpu.data[i].dirty = 1; // Mark the table as clean
//             return;
//         }
//     }
// }

// Slice MeshServer::duplicate_data(uint64_t hash, uint64_t src, uint32_t count) {
//     uint64_t offset = 0;

//     for (uint32_t i = 0; i < cpu.size; ++i) {
//         if (cpu.data[i].type_hash == hash) {
//             // Found the table, duplicate the data
//             offset = cpu.data[i].table.duplicate(src, count);
//             cpu.data[i].dirty = 1; // Mark the table as dirty
//             return Slice(cpu.data[i].table.data, offset, count * cpu.data[i].table.stride);
//         }
//     }
//     return Slice();
// }

// VkCommandBuffer MeshServer::upload_gpu_data(const RenderingDevice &device, uint32_t index) {
//     VkCommandBufferAllocateInfo alloc{};
//     alloc.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//     alloc.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//     alloc.commandPool = command_pool;
//     alloc.commandBufferCount = 1;

//     VkCommandBuffer command_buffer;
//     vkAllocateCommandBuffers(device.device, &alloc, &command_buffer);

//     VkCommandBufferBeginInfo begin_info{};
//     begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//     begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//     vkBeginCommandBuffer(command_buffer, &begin_info);
    
//     cmd_upload_gpu_data(command_buffer, device, index); // Update GPU data for the specified index
    
//     vkEndCommandBuffer(command_buffer);

//     return command_buffer;
// }

// VkCommandBuffer MeshServer::upload_gpu_data(const RenderingDevice &device) {
//     VkCommandBufferAllocateInfo alloc{};
//     alloc.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//     alloc.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//     alloc.commandPool = command_pool;
//     alloc.commandBufferCount = 1;

//     VkCommandBuffer command_buffer;
//     vkAllocateCommandBuffers(device.device, &alloc, &command_buffer);

//     VkCommandBufferBeginInfo begin_info{};
//     begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//     begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//     vkBeginCommandBuffer(command_buffer, &begin_info);

//     cmd_upload_gpu_data(command_buffer, device); // Update all GPU data

//     vkEndCommandBuffer(command_buffer);

//     return command_buffer;
// }

// void MeshServer::cmd_upload_gpu_data(VkCommandBuffer cmd_buffer, const RenderingDevice &device, uint32_t index) {
//     #ifdef ALCHEMIST_DEBUG
//     if (index < gpu.size) {
//         GpuUpdate update = {cpu.data[index].table.data, cpu.data[index].table.count * cpu.data[index].table.stride, 0};
//         gpu.data[index].upload(cmd_buffer, device, update);
//     } else {
//         std::cerr << "Index out of bounds for GPU data update!" << std::endl;
//     }
//     #else
//     GpuUpdate update = {cpu.data[index].table.data, cpu.data[index].table.count * cpu.data[index].table.stride, 0};
//     gpu.data[index].upload(cmd_buffer, device, update);
//     #endif
// }

// void MeshServer::cmd_upload_gpu_data(VkCommandBuffer cmd_buffer, const RenderingDevice &device) {
//     for (uint32_t i = 0; i < gpu.size; ++i) {
//         if (cpu.data[i].dirty != 1) {
//             continue; // Skip if the table is not dirty
//         }
//         if (cpu.data[i].table.count == 0) {
//             continue; // Skip if the table is empty
//         }
//         GpuUpdate update = {cpu.data[i].table.data, cpu.data[i].table.count * cpu.data[i].table.stride, 0};
//         gpu.data[i].upload(cmd_buffer, device, update);
//         cpu.data[i].dirty = 0; // Mark the table as clean after update
//     }
// }

// void MeshServer::cmd_bind_buffers(VkCommandBuffer cmd_buffer, const Mesh &mesh, uint32_t first_binding) {
//     for (uint32_t i = 0; i < mesh.count; ++i) {
//         const Slice &slice = mesh.slices[i];
//         uint32_t table_index = find_table(slice);
        
//         if (table_index != UINT32_MAX) {
//             if (gpu.data[table_index].index_type != VK_INDEX_TYPE_MAX_ENUM) {
//                 #ifdef ALCHEMIST_DEBUG
//                 std::cout << "Binding index buffer for slice at index " << i << std::endl;
//                 #endif
//                 vkCmdBindIndexBuffer(cmd_buffer, gpu.data[table_index].buffer, slice.offset, gpu.data[table_index].index_type);
//             } else {
//                 #ifdef ALCHEMIST_DEBUG
//                 std::cout << "Binding vertex buffer for slice at index " << i << std::endl;
//                 #endif
//                 vkCmdBindVertexBuffers(cmd_buffer, first_binding + i, 1, &gpu.data[table_index].buffer, &slice.offset);
//             }
//         } else {
//             #ifdef ALCHEMIST_DEBUG
//             std::cerr << "Table not found for slice at index " << i << std::endl;
//             #endif
//         }
//     }
// }

// uint32_t MeshServer::find_table(const Slice &slice) const {
//     void *ptr = (uint8_t*)slice.data - slice.offset;

//     for (uint32_t i = 0; i < cpu.size; ++i) {
//         if (cpu.data[i].table.data == ptr) {
//             return i; // Return the index of the table
//         }
//     }
//     return UINT32_MAX; // Not found
// }

// MeshBuilder MeshServer::mesh(uint32_t entries) {
//     return MeshBuilder(*this, entries);
// }

// MeshServer &new_mesh_server(const RenderingDevice &device) {
//     if (mesh_server) {
//         delete mesh_server; // Clean up existing instance
//     }
//     mesh_server = new MeshServer(device);
//     return *mesh_server;
// }

// MeshServer &get_mesh_server() {
//     return *mesh_server;
// }

MeshServer *mesh_server = nullptr;

// MeshBuilder::MeshBuilder(MeshServer &server, uint32_t entries) : data(entries), hashes(entries), server(server) {}

// Mesh MeshBuilder::build() const {
//     Mesh mesh;
//     uint64_t index_hash = 0;
//     uint32_t count = hashes.size;

//     if (indices) {
//         if (index_type == VK_INDEX_TYPE_UINT8) {
//             index_hash = typeid(uint8_t).hash_code();
//         } else if (index_type == VK_INDEX_TYPE_UINT16) {
//             index_hash = typeid(uint16_t).hash_code();
//         } else if (index_type == VK_INDEX_TYPE_UINT32) {
//             index_hash = typeid(uint32_t).hash_code();
//         }

//         mesh.slices = new Slice[count + 1];

//         mesh.slices[count] = server.load_data(index_hash, indices, index_count * sizeof(uint32_t));
//         count++;
//     } else {
//         mesh.slices = new Slice[count];
//     }

//     mesh.count = count;

//     for (uint32_t i = 0; i < hashes.size; ++i) {
//         uint64_t type_hash = hashes.data[i];
//         mesh.slices[i] = server.load_data(type_hash, data.data1[i], data.data2[i]);
//     }

//     return mesh;
// }
