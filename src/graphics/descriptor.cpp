
#ifdef ALCHEMIST_DEBUG
#include <iostream>
#endif // ALCHEMIST_DEBUG

#include "graphics/descriptor.hpp"

DescriptorPoolBuilder::DescriptorPoolBuilder(uint32_t initial_capacity) {
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

VkDescriptorPool DescriptorPoolBuilder::build(VkDevice device) const {
    if (pool_sizes.empty()) {
        // If no pool sizes are added, return an empty pool
        VkDescriptorPool empty_pool;
        vkCreateDescriptorPool(device, nullptr, nullptr, &empty_pool);
        return empty_pool;
    }

    VkDescriptorPoolCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    create_info.poolSizeCount = pool_sizes.size();
    create_info.pPoolSizes = pool_sizes.data();
    create_info.maxSets = max_sets; // Maximum number of descriptor sets that can be allocated from this pool
    create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    VkDescriptorPool descriptor_pool;
    if (vkCreateDescriptorPool(device, &create_info, nullptr, &descriptor_pool) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create descriptor pool." << std::endl;
    #endif
    }

    return descriptor_pool;
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



DescriptorLayoutBuilder::DescriptorLayoutBuilder(uint32_t initial_capacity) {
    bindings.reserve(initial_capacity); // Reserve space for bindings if initial capacity is provided
}

DescriptorBinding DescriptorLayoutBuilder::add_binding() {
    VkDescriptorSetLayoutBinding binding_info{};
    bindings.emplace_back(std::move(binding_info));
    return DescriptorBinding(bindings[bindings.size() - 1]);
}

VkDescriptorSetLayout DescriptorLayoutBuilder::build(VkDevice device) const {
    VkDescriptorSetLayoutCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    create_info.bindingCount = bindings.size();
    create_info.pBindings = bindings.data();

    VkDescriptorSetLayout descriptor_set_layout;
    if (vkCreateDescriptorSetLayout(device, &create_info, nullptr, &descriptor_set_layout) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create descriptor set layout." << std::endl;
    #endif
    }

    return descriptor_set_layout;
}

DescriptorAllocation::DescriptorAllocation(VkDescriptorPool pool, VkDescriptorSetLayout layout) : pool(pool), layout(layout) {}

VkDescriptorSet DescriptorAllocation::allocate(VkDevice device) const {
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &layout;

    VkDescriptorSet descriptor_set;
    if (vkAllocateDescriptorSets(device, &alloc_info, &descriptor_set) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to allocate descriptor set." << std::endl;
    #endif
    }

    return descriptor_set;
}

VkDescriptorSet *DescriptorAllocation::allocate(VkDevice device, uint32_t count) const {
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = pool;
    alloc_info.descriptorSetCount = count;
    alloc_info.pSetLayouts = &layout;

    VkDescriptorSet *descriptor_sets = new VkDescriptorSet[count];
    if (vkAllocateDescriptorSets(device, &alloc_info, descriptor_sets) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to allocate descriptor sets." << std::endl;
    #endif
        delete[] descriptor_sets;
        return nullptr;
    }

    return descriptor_sets;
}



DescriptorWrite::DescriptorWrite(VkWriteDescriptorSet &info) : write_info(info) {}

DescriptorWrite &DescriptorWrite::set_descriptor_set(VkDescriptorSet descriptor_set) {
    write_info.dstSet = descriptor_set;
    return *this;
}

DescriptorWrite &DescriptorWrite::set_binding(uint32_t binding) {
    write_info.dstBinding = binding;
    return *this;
}

DescriptorWrite &DescriptorWrite::set_array_element(uint32_t element) {
    write_info.dstArrayElement = element;
    return *this;
}

DescriptorWrite &DescriptorWrite::set_descriptor_type(VkDescriptorType type) {
    write_info.descriptorType = type;
    return *this;
}

DescriptorWrite &DescriptorWrite::set_descriptor_count(uint32_t count) {
    write_info.descriptorCount = count;
    return *this;
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



DescriptorUpdate::DescriptorUpdate(uint32_t initial_capacity) {
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
    writes.emplace_back(std::move(write_info));
    return DescriptorWrite(writes[writes.size() - 1]);
}

void DescriptorUpdate::apply(VkDevice device) const {
    if (writes.empty()) {
        return; // No writes to apply
    }

    vkUpdateDescriptorSets(device, writes.size(), writes.data(), 0, nullptr);
}
