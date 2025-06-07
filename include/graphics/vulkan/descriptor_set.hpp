
#ifndef ALCHEMIST_GRAPHICS_VULKAN_DESCRIPTOR_SET_HPP
#define ALCHEMIST_GRAPHICS_VULKAN_DESCRIPTOR_SET_HPP

#include <vulkan/vulkan.h>

#include "memory/vector.hpp"

struct DescriptorSetBinding {
    VkDescriptorType type;
    VkShaderStageFlags stageFlags;
    uint32_t descriptorCount;

    DescriptorSetBinding(VkDescriptorType type, VkShaderStageFlags stageFlags,
                         uint32_t descriptorCount = 1);
};

struct DescriptorSetWrite {
    VkDescriptorType type;
    uint32_t binding;
    uint32_t arrayElement;
    uint32_t descriptorCount;

    const VkDescriptorImageInfo *imageInfo;
    const VkDescriptorBufferInfo *bufferInfo;

    DescriptorSetWrite() = default;
    DescriptorSetWrite(VkDescriptorType type, uint32_t binding);

    static DescriptorSetWrite from_image(VkDescriptorType type,
                                         uint32_t binding,
                                         VkDescriptorImageInfo &image);
    static DescriptorSetWrite from_buffer(VkDescriptorType type,
                                          uint32_t binding,
                                          VkDescriptorBufferInfo &buffer);
};

struct UpdateDescriptorSets {
    alchemist::vector<VkDescriptorSet>
        sets; // Using vector for dynamic size management
    alchemist::vector<DescriptorSetWrite>
        writes; // Using vector for dynamic size management

    UpdateDescriptorSets() = default;

    UpdateDescriptorSets &add_set(VkDescriptorSet set);
    UpdateDescriptorSets &add_write(const DescriptorSetWrite &write);

    UpdateDescriptorSets &emplace_image(VkDescriptorType type, uint32_t binding,
                                        VkDescriptorImageInfo &image);
    UpdateDescriptorSets &emplace_buffer(VkDescriptorType type,
                                         uint32_t binding,
                                         VkDescriptorBufferInfo &buffer);
};

VkDescriptorSetLayout
create_descriptor_set_layout(VkDevice device,
                             const DescriptorSetBinding *bindings,
                             uint32_t bindingCount);
VkDescriptorPool create_descriptor_pool(VkDevice device,
                                        const DescriptorSetBinding *bindings,
                                        uint32_t bindingCount,
                                        uint32_t max_sets,
                                        uint32_t multiplier = 1);
VkDescriptorSet *allocate_descriptor_sets(VkDevice device,
                                          VkDescriptorPool pool,
                                          VkDescriptorSetLayout layout,
                                          uint32_t count);

void update_descriptor_sets(VkDevice device,
                            const UpdateDescriptorSets &update);

#endif // ALCHEMIST_GRAPHICS_VULKAN_DESCRIPTOR_SET_HPP
