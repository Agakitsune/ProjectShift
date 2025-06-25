
#ifdef ALCHEMIST_DEBUG
#include <iostream> // Include for debug output
#endif // ALCHEMIST_DEBUG

#include "server/descriptor.hpp"

DescriptorUpdate Descriptor::update() const {
    return DescriptorUpdate(*this);
}

DescriptorWrite::DescriptorWrite(VkWriteDescriptorSet &info) : write_info(info) {
    write_info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET; // Initialize the structure type
    write_info.dstSet = VK_NULL_HANDLE; // Set the destination descriptor set
    write_info.dstBinding = 0; // Default binding, can be changed later
    write_info.dstArrayElement = 0; // Default array element, can be changed later
    write_info.descriptorCount = 1; // Default count, can be changed later
    write_info.descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM; // Default type, can be changed later
    write_info.pBufferInfo = nullptr; // Initialize buffer info pointer to nullptr
    write_info.pImageInfo = nullptr; // Initialize image info pointer to nullptr
    write_info.pTexelBufferView = nullptr; // Initialize texel buffer view pointer to nullptr
}

DescriptorWrite &DescriptorWrite::set_descriptor_set(RID descriptor_set) {
    const Descriptor &desc = DescriptorServer::instance().get_descriptor(descriptor_set);
    write_info.dstSet = desc.descriptor_set;
    return *this; // Return the current instance for method chaining
}

DescriptorWrite &DescriptorWrite::set_binding(uint32_t binding) {
    write_info.dstBinding = binding;
    return *this; // Return the current instance for method chaining
}

DescriptorWrite &DescriptorWrite::set_array_element(uint32_t element) {
    write_info.dstArrayElement = element;
    return *this; // Return the current instance for method chaining
}

DescriptorWrite &DescriptorWrite::set_descriptor_type(VkDescriptorType type) {
    write_info.descriptorType = type;
    return *this; // Return the current instance for method chaining
}

DescriptorWrite &DescriptorWrite::set_descriptor_count(uint32_t count) {
    write_info.descriptorCount = count;
    return *this; // Return the current instance for method chaining
}

DescriptorWrite &DescriptorWrite::set_buffer_info(VkBuffer buffer, uint32_t offset, uint64_t range) {
    VkDescriptorBufferInfo *buffer_info;

    if (write_info.pBufferInfo != nullptr) {
        buffer_info = const_cast<VkDescriptorBufferInfo*>(write_info.pBufferInfo); // Reuse existing buffer info if it exists
    } else {
        buffer_info = new VkDescriptorBufferInfo; // Create new buffer info
    }

    buffer_info->buffer = buffer;
    buffer_info->offset = offset;
    buffer_info->range = range;

    if (write_info.pImageInfo != nullptr) {
        delete write_info.pImageInfo; // Clear existing image info if it exists
    }

    write_info.pBufferInfo = buffer_info;
    write_info.pImageInfo = nullptr; // Clear image info if not used
    write_info.pTexelBufferView = nullptr; // Clear texel buffer view if not used

    return *this;
}

DescriptorWrite &DescriptorWrite::set_image_info(VkImageView image_view, VkSampler sampler, VkImageLayout layout) {
    VkDescriptorImageInfo *image_info;
    if (write_info.pImageInfo != nullptr) {
        image_info = const_cast<VkDescriptorImageInfo*>(write_info.pImageInfo); // Reuse existing image info if it exists
    } else {
        image_info = new VkDescriptorImageInfo; // Create new image info
    }
    
    image_info->imageView = image_view;
    image_info->imageLayout = layout;
    image_info->sampler = sampler;

    if (write_info.pBufferInfo != nullptr) {
        delete write_info.pBufferInfo; // Clear existing buffer info if it exists
    }
    
    write_info.pImageInfo = image_info;
    write_info.pBufferInfo = nullptr; // Clear buffer info if not used
    write_info.pTexelBufferView = nullptr; // Clear texel buffer view if not used

    return *this;
}


DescriptorUpdate::DescriptorUpdate(const Descriptor &descriptor) : descriptor(descriptor) {}

DescriptorUpdate::DescriptorUpdate(const Descriptor &descriptor, uint32_t initial_capacity) : descriptor(descriptor) {
    writes.reserve(initial_capacity); // Reserve space for writes if initial capacity is provided
}

DescriptorUpdate::~DescriptorUpdate() {
    for (const auto &write : writes) {
        if (write.pBufferInfo != nullptr) {
            delete write.pBufferInfo; // Clean up buffer info if it exists
        }
        if (write.pImageInfo != nullptr) {
            delete write.pImageInfo; // Clean up image info if it exists
        }
    }
}

DescriptorWrite DescriptorUpdate::add_write() {
    VkWriteDescriptorSet write_info{};
    write_info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_info.dstSet = descriptor.descriptor_set; // Set the destination descriptor set
    write_info.dstBinding = 0; // Default binding, can be changed later
    write_info.dstArrayElement = 0; // Default array element, can be changed later
    write_info.descriptorCount = 1; // Default count, can be changed later
    write_info.descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM; // Default type, can be changed later

    writes.emplace_back(std::move(write_info)); // Add the new write info to the vector
    return DescriptorWrite(writes.back()); // Return a DescriptorWrite object for further configuration
}

void DescriptorUpdate::update() const {
    if (writes.empty()) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "No writes to update in descriptor update." << std::endl;
        #endif
        return; // If there are no writes, do nothing
    }

    vkUpdateDescriptorSets(descriptor.device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
    // Update the descriptor sets with the accumulated writes
}



DescriptorPoolBuilder::DescriptorPoolBuilder(DescriptorPoolServer &server) : server(server) {}

DescriptorPoolBuilder::DescriptorPoolBuilder(DescriptorPoolServer &server, uint32_t initial_capacity) : server(server) {
    pool_sizes.reserve(initial_capacity); // Reserve space for pool sizes if initial capacity is provided
}

DescriptorPoolBuilder &DescriptorPoolBuilder::add_pool_size(VkDescriptorType type, uint32_t count) {
    VkDescriptorPoolSize pool_size{};
    pool_size.type = type;
    pool_size.descriptorCount = count;
    pool_sizes.emplace_back(std::move(pool_size));
    return *this;
}

DescriptorPoolBuilder &DescriptorPoolBuilder::set_max_sets(uint32_t max_sets) {
    this->max_sets = max_sets;
    return *this;
}

RID DescriptorPoolBuilder::build() const {
    if (pool_sizes.empty()) {
        // If no pool sizes are added, return an empty pool
        VkDescriptorPool empty_pool;
        vkCreateDescriptorPool(server.device, nullptr, nullptr, &empty_pool);
        return RID_INVALID; // Return an invalid RID for an empty pool
    }

    VkDescriptorPoolCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    create_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    create_info.pPoolSizes = pool_sizes.data();
    create_info.maxSets = max_sets; // Maximum number of descriptor sets that can be allocated from this pool
    create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    VkDescriptorPool descriptor_pool;
    if (vkCreateDescriptorPool(server.device, &create_info, nullptr, &descriptor_pool) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create descriptor pool." << std::endl;
    #endif
        return RID_INVALID; // Return an invalid RID if creation fails
    }

    DescriptorPool pool;
    pool.pool = descriptor_pool;
    pool.rid = RIDServer::instance().new_id(RIDServer::DESCRIPTOR_POOL); // Create a new RID for the descriptor pool

    server.descriptor_pools.emplace_back(std::move(pool)); // Add the pool to the server's pools vector

    return pool.rid; // Return the RID of the newly created descriptor pool
}



DescriptorBinding::DescriptorBinding(VkDescriptorSetLayoutBinding &info) : binding_info(info) {}

DescriptorBinding &DescriptorBinding::set_binding(uint32_t binding) {
    binding_info.binding = binding;
    return *this;
}

DescriptorBinding &DescriptorBinding::set_descriptor_type(VkDescriptorType type) {
    binding_info.descriptorType = type;
    return *this;
}

DescriptorBinding &DescriptorBinding::set_descriptor_count(uint32_t count) {
    binding_info.descriptorCount = count;
    return *this;
}

DescriptorBinding &DescriptorBinding::set_stage_flags(VkShaderStageFlags flags) {
    binding_info.stageFlags = flags;
    return *this;
}



DescriptorLayoutBuilder::DescriptorLayoutBuilder(DescriptorLayoutServer &server) : server(server) {}

DescriptorLayoutBuilder::DescriptorLayoutBuilder(DescriptorLayoutServer &server, uint32_t initial_capacity) : server(server) {
    bindings.reserve(initial_capacity); // Reserve space for bindings if initial capacity is provided
}

DescriptorBinding DescriptorLayoutBuilder::add_binding() {
    VkDescriptorSetLayoutBinding binding_info{};
    bindings.emplace_back(std::move(binding_info));
    return DescriptorBinding(bindings[bindings.size() - 1]);
}

RID DescriptorLayoutBuilder::build() const {
    if (bindings.empty()) {
        // If no bindings are added, return an empty layout
        VkDescriptorSetLayout empty_layout;
        vkCreateDescriptorSetLayout(server.device, nullptr, nullptr, &empty_layout);
        return RID_INVALID; // Return an invalid RID for an empty layout
    }

    VkDescriptorSetLayoutCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    create_info.bindingCount = static_cast<uint32_t>(bindings.size());
    create_info.pBindings = bindings.data();

    VkDescriptorSetLayout descriptor_set_layout;
    if (vkCreateDescriptorSetLayout(server.device, &create_info, nullptr, &descriptor_set_layout) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create descriptor set layout." << std::endl;
    #endif
        return RID_INVALID; // Return an invalid RID if creation fails
    }

    DescriptorLayout layout;
    layout.layout = descriptor_set_layout;
    layout.rid = RIDServer::instance().new_id(RIDServer::DESCRIPTOR_LAYOUT); // Create a new RID for the descriptor layout

    server.descriptor_layouts.emplace_back(std::move(layout)); // Add the layout to the server's layouts vector

    return layout.rid; // Return the RID of the newly created descriptor layout
}

DescriptorPoolServer::DescriptorPoolServer(VkDevice device) : device(device) {}

DescriptorPoolServer::~DescriptorPoolServer() {
    for (auto &pool : descriptor_pools) {
        #ifdef ALCHEMIST_DEBUG
        std::cout << "Destroying descriptor pool with RID: " << pool.rid << std::endl;
        #endif
        vkDestroyDescriptorPool(device, pool.pool, nullptr); // Clean up each descriptor pool
    }
}

RID DescriptorPoolServer::new_descriptor_pool(const VkDescriptorPoolCreateInfo &create_info) {
    VkDescriptorPool descriptor_pool;
    RID rid = RIDServer::instance().new_id(RIDServer::DESCRIPTOR_POOL);

    if (vkCreateDescriptorPool(device, &create_info, nullptr, &descriptor_pool) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create descriptor pool." << std::endl;
    #endif
        return RID_INVALID; // Return an invalid RID on failure
    }

    DescriptorPool pool;
    pool.pool = descriptor_pool;
    pool.rid = rid;

    descriptor_pools.emplace_back(std::move(pool)); // Add the created pool to the pools vector

    return rid; // Return the RID of the newly created descriptor pool
}

RID DescriptorPoolServer::new_descriptor_pool(VkDescriptorPoolCreateInfo &&create_info) {
    VkDescriptorPool descriptor_pool;
    RID rid = RIDServer::instance().new_id(RIDServer::DESCRIPTOR_POOL);

    if (vkCreateDescriptorPool(device, &create_info, nullptr, &descriptor_pool) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create descriptor pool." << std::endl;
    #endif
        return RID_INVALID; // Return an invalid RID on failure
    }

    DescriptorPool pool;
    pool.pool = descriptor_pool;
    pool.rid = rid;

    descriptor_pools.emplace_back(std::move(pool)); // Add the created pool to the pools vector

    return rid; // Return the RID of the newly created descriptor pool
}

DescriptorPoolBuilder DescriptorPoolServer::new_descriptor_pool() {
    return DescriptorPoolBuilder(*this); // Return a DescriptorPoolBuilder instance for creating descriptor pools
}

const DescriptorPool &DescriptorPoolServer::get_descriptor_pool(RID rid) {
    for (const auto &pool : descriptor_pools) {
        if (pool.rid == rid) {
            return pool; // Return the descriptor pool if found
        }
    }
    #ifdef ALCHEMIST_DEBUG
    std::cerr << "Descriptor pool with RID " << rid << " not found!" << std::endl;
    #endif
    return descriptor_pools.front(); // Return the first pool as a fallback (should be handled better)
}

DescriptorPoolServer &DescriptorPoolServer::instance() {
    return *__instance; // Return the singleton instance
}

std::unique_ptr<DescriptorPoolServer> DescriptorPoolServer::__instance = nullptr; // Singleton instance of descriptorPoolServer



DescriptorLayoutServer::DescriptorLayoutServer(VkDevice device) : device(device) {}

DescriptorLayoutServer::~DescriptorLayoutServer() {
    for (auto &layout : descriptor_layouts) {
        #ifdef ALCHEMIST_DEBUG
        std::cout << "Destroying descriptor layout with RID: " << layout.rid << std::endl;
        #endif
        vkDestroyDescriptorSetLayout(device, layout.layout, nullptr); // Clean up each descriptor layout
    }
}

RID DescriptorLayoutServer::new_descriptor_layout(const VkDescriptorSetLayoutCreateInfo &create_info) {
    VkDescriptorSetLayout descriptor_set_layout;
    RID rid = RIDServer::instance().new_id(RIDServer::DESCRIPTOR_LAYOUT);

    if (vkCreateDescriptorSetLayout(device, &create_info, nullptr, &descriptor_set_layout) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create descriptor set layout." << std::endl;
    #endif
        return RID_INVALID; // Return an invalid RID on failure
    }

    DescriptorLayout layout;
    layout.layout = descriptor_set_layout;
    layout.rid = rid;

    descriptor_layouts.emplace_back(std::move(layout)); // Add the layout to the server's layouts vector

    return rid; // Return the RID of the newly created descriptor layout
}

RID DescriptorLayoutServer::new_descriptor_layout(VkDescriptorSetLayoutCreateInfo &&create_info) {
    VkDescriptorSetLayout descriptor_set_layout;
    RID rid = RIDServer::instance().new_id(RIDServer::DESCRIPTOR_LAYOUT);

    if (vkCreateDescriptorSetLayout(device, &create_info, nullptr, &descriptor_set_layout) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create descriptor set layout." << std::endl;
    #endif
        return RID_INVALID; // Return an invalid RID on failure
    }

    DescriptorLayout layout;
    layout.layout = descriptor_set_layout;
    layout.rid = rid;

    descriptor_layouts.emplace_back(std::move(layout)); // Add the layout to the server's layouts vector

    return rid; // Return the RID of the newly created descriptor layout
}

DescriptorLayoutBuilder DescriptorLayoutServer::new_descriptor_layout() {
    return DescriptorLayoutBuilder(*this); // Create a new descriptor layout builder
} // Create a new descriptor layout builder

const DescriptorLayout &DescriptorLayoutServer::get_descriptor_layout(RID rid) {
    for (const auto &layout : descriptor_layouts) {
        if (layout.rid == rid) {
            return layout; // Return the descriptor layout if found
        }
    }
    #ifdef ALCHEMIST_DEBUG
    std::cerr << "Descriptor layout with RID " << rid << " not found!" << std::endl;
    #endif
    return descriptor_layouts.front(); // Return the first layout as a fallback (should be handled better)
}

DescriptorLayoutServer &DescriptorLayoutServer::instance() {
    return *__instance; // Return the singleton instance
}

std::unique_ptr<DescriptorLayoutServer> DescriptorLayoutServer::__instance = nullptr; // Singleton instance of descriptorLayoutServer



DescriptorServer::DescriptorServer(VkDevice device) : device(device) {}

DescriptorServer::~DescriptorServer() {
    for (auto &descriptor : descriptors) {
        const DescriptorPool &pool = DescriptorPoolServer::instance().get_descriptor_pool(descriptor.pool_rid); // Get the descriptor pool for the descriptor set
        #ifdef ALCHEMIST_DEBUG
        std::cout << "Destroying descriptor with RID: " << descriptor.rid << std::endl;
        #endif
        vkFreeDescriptorSets(device, pool.pool, 1, &descriptor.descriptor_set); // Clean up each descriptor set
    }
}

RID DescriptorServer::new_descriptor(RID pool, RID layout) {
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = DescriptorPoolServer::instance().get_descriptor_pool(pool).pool; // Get the descriptor pool
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &DescriptorLayoutServer::instance().get_descriptor_layout(layout).layout; // Get the descriptor layout

    VkDescriptorSet descriptor_set;
    if (vkAllocateDescriptorSets(device, &alloc_info, &descriptor_set) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to allocate descriptor set." << std::endl;
    #endif
        return RID_INVALID; // Return an invalid RID on failure
    }

    Descriptor descriptor;
    descriptor.descriptor_set = descriptor_set;
    descriptor.device = device; // Set the Vulkan device for the descriptor
    descriptor.rid = RIDServer::instance().new_id(RIDServer::DESCRIPTOR_SET); // Create a new RID for the descriptor set
    descriptor.pool_rid = pool; // Set the pool RID
    descriptor.layout_rid = layout; // Set the layout RID

    #ifdef ALCHEMIST_DEBUG
    std::cout << "Created descriptor set with Pool " << pool << " and " << layout << std::endl;
    #endif

    descriptors.push_back(descriptor); // Add the created descriptor to the server's descriptors vector

    return descriptors.back().rid; // Return the RID of the newly created descriptor set
}

void DescriptorServer::emplace_descriptors(std::vector<RID> &descriptors, RID pool, RID layout, uint32_t count) {
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = DescriptorPoolServer::instance().get_descriptor_pool(pool).pool; // Get the descriptor pool
    alloc_info.descriptorSetCount = count;
    alloc_info.pSetLayouts = &DescriptorLayoutServer::instance().get_descriptor_layout(layout).layout; // Get the descriptor layout

    std::vector<VkDescriptorSet> descriptor_sets(count);
    if (vkAllocateDescriptorSets(device, &alloc_info, descriptor_sets.data()) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to allocate descriptor sets." << std::endl;
    #endif
        return; // Return if allocation fails
    }

    for (const auto &descriptor_set : descriptor_sets) {
        Descriptor descriptor;
        descriptor.descriptor_set = descriptor_set;
        descriptor.rid = RIDServer::instance().new_id(RIDServer::DESCRIPTOR_SET); // Create a new RID for the descriptor set
        descriptor.pool_rid = pool; // Set the pool RID
        descriptor.layout_rid = layout; // Set the layout RID

        this->descriptors.push_back(descriptor); // Add the created descriptor to the vector
        descriptors.push_back(descriptor.rid);
    }
}

const Descriptor &DescriptorServer::get_descriptor(RID rid) const {
    for (const auto &descriptor : descriptors) {
        if (descriptor.rid == rid) {
            return descriptor; // Return the descriptor if found
        }
    }
    #ifdef ALCHEMIST_DEBUG
    std::cerr << "Descriptor with RID " << rid << " not found!" << std::endl;
    #endif
    return descriptors.front(); // Return the first descriptor as a fallback (should be handled better)
}

DescriptorServer &DescriptorServer::instance() {
    return *__instance; // Return the singleton instance
}

std::unique_ptr<DescriptorServer> DescriptorServer::__instance = nullptr; // Singleton instance of descriptorServer
