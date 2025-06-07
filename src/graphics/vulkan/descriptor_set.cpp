
#include <algorithm>
#include <iostream>

#include "graphics/vulkan/descriptor_set.hpp"

DescriptorSetBinding::DescriptorSetBinding(VkDescriptorType type,
                                           VkShaderStageFlags stage_flags,
                                           uint32_t descriptor_count)
    : type(type), stageFlags(stage_flags), descriptorCount(descriptor_count) {}

DescriptorSetWrite::DescriptorSetWrite(VkDescriptorType type, uint32_t binding)
    : type(type), binding(binding),
      arrayElement(0),    // Default to first element in array
      descriptorCount(1), // Default to single descriptor
      imageInfo{},        // Initialize to default empty image info
      bufferInfo{}        // Initialize to default empty buffer info
{}

DescriptorSetWrite
DescriptorSetWrite::from_image(VkDescriptorType type, uint32_t binding,
                               VkDescriptorImageInfo &image) {
    DescriptorSetWrite write(type, binding);
    write.imageInfo = &image;
    write.descriptorCount = 1; // Assuming single descriptor for image
    write.arrayElement = 0;    // Default to first element in array
    return write;
}

DescriptorSetWrite
DescriptorSetWrite::from_buffer(VkDescriptorType type, uint32_t binding,
                                VkDescriptorBufferInfo &buffer) {
    DescriptorSetWrite write(type, binding);
    write.bufferInfo = &buffer;
    write.descriptorCount = 1; // Assuming single descriptor for buffer
    write.arrayElement = 0;    // Default to first element in array
    return write;
}

UpdateDescriptorSets &UpdateDescriptorSets::add_set(VkDescriptorSet set) {
    sets.push(set);
    return *this;
}

UpdateDescriptorSets &
UpdateDescriptorSets::add_write(const DescriptorSetWrite &write) {
    writes.push(write);
    return *this;
}

UpdateDescriptorSets &
UpdateDescriptorSets::emplace_image(VkDescriptorType type, uint32_t binding,
                                    VkDescriptorImageInfo &image) {
    DescriptorSetWrite write =
        DescriptorSetWrite::from_image(type, binding, image);
    return add_write(write);
}

UpdateDescriptorSets &
UpdateDescriptorSets::emplace_buffer(VkDescriptorType type, uint32_t binding,
                                     VkDescriptorBufferInfo &buffer) {
    DescriptorSetWrite write =
        DescriptorSetWrite::from_buffer(type, binding, buffer);
    return add_write(write);
}

VkDescriptorSetLayout
create_descriptor_set_layout(VkDevice device,
                             const DescriptorSetBinding *bindings,
                             uint32_t binding_count) {
    VkDescriptorSetLayoutCreateInfo layout_info{};
    VkDescriptorSetLayoutBinding *vk_bindings =
        new VkDescriptorSetLayoutBinding[binding_count];

    for (uint32_t i = 0; i < binding_count; ++i) {
        vk_bindings[i].binding = i;
        vk_bindings[i].descriptorType = bindings[i].type;
        vk_bindings[i].descriptorCount = bindings[i].descriptorCount;
        vk_bindings[i].stageFlags = bindings[i].stageFlags;
        vk_bindings[i].pImmutableSamplers =
            nullptr; // Optional, can be set if needed
    }

    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = binding_count;
    layout_info.pBindings = vk_bindings;

    VkDescriptorSetLayout descriptor_set_layout;
    if (vkCreateDescriptorSetLayout(device, &layout_info, nullptr,
                                    &descriptor_set_layout) != VK_SUCCESS) {
        return VK_NULL_HANDLE; // Failed to create descriptor set layout
    }

    return descriptor_set_layout;
}

VkDescriptorPool create_descriptor_pool(VkDevice device,
                                        const DescriptorSetBinding *bindings,
                                        uint32_t binding_count,
                                        uint32_t max_sets,
                                        uint32_t multiplier) {
    uint32_t count[VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT + 1] = {0};
    VkDescriptorPoolSize *pool_sizes = nullptr;
    uint32_t unique_types = 0;

    for (uint32_t i = 0; i < binding_count; ++i) {
        count[bindings[i].type] += bindings[i].descriptorCount;
    }

    for (uint32_t i = 0; i < VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT + 1; ++i) {
        if (count[i] > 0) {
            unique_types++;
        }
    }
    std::cout << "Unique descriptor types: " << unique_types << std::endl;
    pool_sizes = new VkDescriptorPoolSize[unique_types];
    for (uint32_t i = 0, j = 0; i < VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT + 1;
         ++i) {
        if (count[i] > 0) {
            std::cout << "Descriptor type: " << i << ", count: " << count[i]
                      << std::endl;
            pool_sizes[j].type = static_cast<VkDescriptorType>(i);
            pool_sizes[j].descriptorCount = count[i] * multiplier * max_sets;
            j++;
        }
    }

    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = unique_types;
    pool_info.pPoolSizes = pool_sizes;
    pool_info.maxSets = max_sets; // Maximum number of descriptor sets that can
                                 // be allocated from this pool
    pool_info.flags =
        VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; // Optional, allows
                                                           // freeing individual
                                                           // descriptor sets

    VkDescriptorPool descriptor_pool;
    if (vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptor_pool) !=
        VK_SUCCESS) {
        delete[] pool_sizes;    // Clean up allocated memory for pool sizes
        return VK_NULL_HANDLE; // Failed to create descriptor pool
    }
    delete[] pool_sizes; // Clean up allocated memory for pool sizes

    return descriptor_pool;
}

VkDescriptorSet *allocate_descriptor_sets(VkDevice device,
                                          VkDescriptorPool pool,
                                          VkDescriptorSetLayout layout,
                                          uint32_t count) {
    VkDescriptorSetAllocateInfo alloc_info{};
    VkDescriptorSetLayout *layouts = new VkDescriptorSetLayout[count];

    for (uint32_t i = 0; i < count; ++i) {
        layouts[i] = layout; // All sets will use the same layout
    }

    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = pool;
    alloc_info.descriptorSetCount = count;
    alloc_info.pSetLayouts = layouts; // Single layout for all sets

    VkDescriptorSet *descriptor_sets = new VkDescriptorSet[count];
    if (vkAllocateDescriptorSets(device, &alloc_info, descriptor_sets) !=
        VK_SUCCESS) {
        delete[] descriptor_sets; // Clean up allocated memory on failure
        return nullptr;          // Failed to allocate descriptor sets
    }

    return descriptor_sets;
}

void update_descriptor_sets(VkDevice device,
                            const UpdateDescriptorSets &update) {
    VkWriteDescriptorSet *vk_writes =
        new VkWriteDescriptorSet[update.writes.size];

    for (uint32_t i = 0; i < update.writes.size; ++i) {
        vk_writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        std::cout << "Updating descriptor set: " << i
                  << ", binding: " << update.writes.data[i].binding
                  << ", type: " << update.writes.data[i].type << std::endl;
        vk_writes[i].dstBinding = update.writes.data[i].binding;
        vk_writes[i].dstArrayElement = update.writes.data[i].arrayElement;
        vk_writes[i].descriptorCount = update.writes.data[i].descriptorCount;
        vk_writes[i].descriptorType = update.writes.data[i].type;

        vk_writes[i].pNext = nullptr; // No additional structure
        vk_writes[i].dstSet = VK_NULL_HANDLE; // Will be set later for each set
        vk_writes[i].pTexelBufferView = nullptr; // Not used in this case
        if (update.writes.data[i].type == VK_DESCRIPTOR_TYPE_SAMPLER ||
            update.writes.data[i].type ==
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
            update.writes.data[i].type == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE) {
            vk_writes[i].pImageInfo = update.writes.data[i].imageInfo;
            vk_writes[i].pBufferInfo = nullptr; // Not used for image types
        } else if (update.writes.data[i].type ==
                       VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ||
                   update.writes.data[i].type ==
                       VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
            vk_writes[i].pBufferInfo = update.writes.data[i].bufferInfo;
            vk_writes[i].pImageInfo = nullptr; // Not used for buffer types
        } else {
            vk_writes[i].pImageInfo = nullptr;  // Not used for other types
            vk_writes[i].pBufferInfo = nullptr; // Not used for other types           
        }
    }

    for (uint32_t j = 0; j < update.sets.size; ++j) {
        for (uint32_t i = 0; i < update.writes.size; ++i) {
            vk_writes[i].dstSet =
                update.sets.data[j]; // Set the destination set for each write
        }

        vkUpdateDescriptorSets(device, update.writes.size, vk_writes, 0,
                               nullptr);
    }

    delete[] vk_writes; // Clean up allocated memory for write descriptor sets
}
