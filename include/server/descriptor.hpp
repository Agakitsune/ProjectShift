
#ifndef ALCHEMIST_SERVER_DESCRIPTOR_HPP
#define ALCHEMIST_SERVER_DESCRIPTOR_HPP

#include <vector>
#include <cstdint>

#include <vulkan/vulkan.h>

#include "server/rid.hpp"

struct DescriptorPool {
    VkDescriptorPool pool; // Vulkan descriptor pool object
    RID rid = RID_INVALID; // Resource ID for the descriptor pool

    DescriptorPool() = default;
};

struct DescriptorLayout {
    VkDescriptorSetLayout layout; // Vulkan descriptor set layout object
    RID rid = RID_INVALID; // Resource ID for the descriptor layout

    DescriptorLayout() = default;
};

struct DescriptorUpdate;

struct Descriptor {
    VkDescriptorSet descriptor_set; // Vulkan descriptor set object
    VkDevice device;
    
    RID rid = RID_INVALID; // Resource ID for the descriptor set
    RID pool_rid = RID_INVALID; // Resource ID for the descriptor pool
    RID layout_rid = RID_INVALID; // Resource ID for the descriptor layout

    Descriptor() = default;

    DescriptorUpdate update() const;
};

struct DescriptorWrite {
    VkWriteDescriptorSet &write_info;

    DescriptorWrite(VkWriteDescriptorSet &info);

    DescriptorWrite &set_descriptor_set(RID descriptor_set);
    DescriptorWrite &set_binding(uint32_t binding);
    DescriptorWrite &set_array_element(uint32_t element);
    DescriptorWrite &set_descriptor_type(VkDescriptorType type);
    DescriptorWrite &set_descriptor_count(uint32_t count);

    DescriptorWrite &set_buffer_info(VkBuffer buffer, uint32_t offset = 0, uint64_t range = VK_WHOLE_SIZE);
    DescriptorWrite &set_image_info(VkImageView image_view, VkSampler sampler = VK_NULL_HANDLE, VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
};

struct DescriptorUpdate {
    std::vector<VkWriteDescriptorSet> writes;

    const Descriptor &descriptor;

    DescriptorUpdate(const Descriptor &descriptor);
    DescriptorUpdate(const Descriptor &descriptor, uint32_t initial_capacity);
    ~DescriptorUpdate();

    DescriptorWrite add_write();

    void update() const;
};

struct DescriptorPoolServer; // Forward declaration

struct DescriptorPoolBuilder {
    std::vector<VkDescriptorPoolSize> pool_sizes;
    uint32_t max_sets = 1000; // Default to 1000 sets

    DescriptorPoolServer &server;

    DescriptorPoolBuilder(DescriptorPoolServer &server);
    DescriptorPoolBuilder(DescriptorPoolServer &server, uint32_t initial_capacity);

    DescriptorPoolBuilder &add_pool_size(VkDescriptorType type, uint32_t count);
    DescriptorPoolBuilder &set_max_sets(uint32_t max_sets);

    RID build() const;
};

struct DescriptorBinding {
    VkDescriptorSetLayoutBinding &binding_info;

    DescriptorBinding(VkDescriptorSetLayoutBinding &info);

    DescriptorBinding &set_binding(uint32_t binding);
    DescriptorBinding &set_descriptor_type(VkDescriptorType type);
    DescriptorBinding &set_descriptor_count(uint32_t count);
    DescriptorBinding &set_stage_flags(VkShaderStageFlags flags);
};

struct DescriptorLayoutServer; // Forward declaration

struct DescriptorLayoutBuilder {
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    DescriptorLayoutServer &server;

    DescriptorLayoutBuilder(DescriptorLayoutServer &server);
    DescriptorLayoutBuilder(DescriptorLayoutServer &server, uint32_t initial_capacity);

    DescriptorBinding add_binding();

    RID build() const;
};

struct DescriptorPoolServer {
    std::vector<DescriptorPool> descriptor_pools; // Vector to hold all descriptor pools

    VkDevice device; // Vulkan device

    DescriptorPoolServer(VkDevice device);
    ~DescriptorPoolServer();

    RID new_descriptor_pool(const VkDescriptorPoolCreateInfo &create_info);
    RID new_descriptor_pool(VkDescriptorPoolCreateInfo &&create_info);

    DescriptorPoolBuilder new_descriptor_pool(); // Create a new descriptor pool builder

    const DescriptorPool &get_descriptor_pool(RID rid);

    static DescriptorPoolServer &instance();
    static std::unique_ptr<DescriptorPoolServer> __instance; // Singleton instance of descriptorPoolServer
};

struct DescriptorLayoutServer {
    std::vector<DescriptorLayout> descriptor_layouts; // Vector to hold all descriptor layouts

    VkDevice device; // Vulkan device

    DescriptorLayoutServer(VkDevice device);
    ~DescriptorLayoutServer();

    RID new_descriptor_layout(const VkDescriptorSetLayoutCreateInfo &create_info);
    RID new_descriptor_layout(VkDescriptorSetLayoutCreateInfo &&create_info);

    DescriptorLayoutBuilder new_descriptor_layout(); // Create a new descriptor layout builder

    const DescriptorLayout &get_descriptor_layout(RID rid);

    static DescriptorLayoutServer &instance();
    static std::unique_ptr<DescriptorLayoutServer> __instance; // Singleton instance of descriptorLayoutServer
};

struct DescriptorServer {
    std::vector<Descriptor> descriptors; // Vector to hold all descriptors

    VkDevice device; // Vulkan device

    DescriptorServer(VkDevice device);
    ~DescriptorServer();

    RID new_descriptor(RID pool, RID layout);
    void emplace_descriptors(std::vector<RID> &descriptors, RID pool, RID layout, uint32_t count);

    const Descriptor &get_descriptor(RID rid) const;

    static DescriptorServer &instance();
    static std::unique_ptr<DescriptorServer> __instance; // Singleton instance of descriptorServer
};

#endif // ALCHEMIST_SERVER_DESCRIPTOR_HPP
