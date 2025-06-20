
#ifndef ALCHEMIST_GRAPHICS_DESCRIPTOR_HPP
#define ALCHEMIST_GRAPHICS_DESCRIPTOR_HPP

#include <vector>

#include <vulkan/vulkan.h>

struct DescriptorPoolBuilder {
    std::vector<VkDescriptorPoolSize> pool_sizes;
    uint32_t max_sets = 1000; // Default to 1000 sets

    DescriptorPoolBuilder(uint32_t initial_capacity = 0);

    DescriptorPoolBuilder &add_pool_size(VkDescriptorType type, uint32_t count);
    DescriptorPoolBuilder &set_max_sets(uint32_t max_sets);

    VkDescriptorPool build(VkDevice device) const;
};

struct DescriptorBinding {
    VkDescriptorSetLayoutBinding &binding_info;

    DescriptorBinding(VkDescriptorSetLayoutBinding &info);

    DescriptorBinding &set_binding(uint32_t binding);
    DescriptorBinding &set_descriptor_type(VkDescriptorType type);
    DescriptorBinding &set_descriptor_count(uint32_t count);
    DescriptorBinding &set_stage_flags(VkShaderStageFlags flags);
};

struct DescriptorLayoutBuilder {
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    DescriptorLayoutBuilder(uint32_t initial_capacity = 0);

    DescriptorBinding add_binding();

    VkDescriptorSetLayout build(VkDevice device) const;
};

struct DescriptorAllocation {
    VkDescriptorPool pool;
    VkDescriptorSetLayout layout;

    DescriptorAllocation(VkDescriptorPool pool, VkDescriptorSetLayout layout);

    VkDescriptorSet allocate(VkDevice device) const;
    VkDescriptorSet *allocate(VkDevice device, uint32_t count) const;
};

struct DescriptorWrite {
    VkWriteDescriptorSet &write_info;

    DescriptorWrite(VkWriteDescriptorSet &info);

    DescriptorWrite &set_descriptor_set(VkDescriptorSet descriptor_set);
    DescriptorWrite &set_binding(uint32_t binding);
    DescriptorWrite &set_array_element(uint32_t element);
    DescriptorWrite &set_descriptor_type(VkDescriptorType type);
    DescriptorWrite &set_descriptor_count(uint32_t count);

    DescriptorWrite &set_buffer_info(VkBuffer buffer, uint32_t offset = 0, uint64_t range = VK_WHOLE_SIZE);
    DescriptorWrite &set_image_info(VkImageView image_view, VkSampler sampler = VK_NULL_HANDLE, VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
};

struct DescriptorUpdate {
    std::vector<VkWriteDescriptorSet> writes;

    DescriptorUpdate(uint32_t initial_capacity = 0);
    ~DescriptorUpdate();

    DescriptorWrite add_write();

    void apply(VkDevice device) const;
};

#endif // ALCHEMIST_GRAPHICS_DESCRIPTOR_HPP
