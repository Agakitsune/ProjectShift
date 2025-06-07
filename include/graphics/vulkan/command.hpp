
#ifndef ALCHEMIST_GRAPHICS_VULKAN_COMMAND_HPP
#define ALCHEMIST_GRAPHICS_VULKAN_COMMAND_HPP

#include <vulkan/vulkan.h>

#include "graphics/color.hpp"
#include "math/vector/vec2.hpp"
#include "memory/vector.hpp"

enum ClearFlags {
    CLEAR_NONE = 0x0,          // No clear operation
    CLEAR_COLOR = 0x1,         // Clear color attachment
    CLEAR_DEPTH_STENCIL = 0x2, // Clear depth attachment
    CLEAR_COLOR_DEPTH_STENCIL =
        CLEAR_COLOR |
        CLEAR_DEPTH_STENCIL // Clear both color and depth attachments
};

struct RenderPassBeginInfo {
    ClearFlags flag = CLEAR_NONE;

    VkRenderPass render_pass;
    VkFramebuffer framebuffer;

    color clear_color;
    float clear_depth = 1.0f;   // Default depth value to clear
    uint32_t clear_stencil = 0; // Default stencil value to clear

    VkOffset2D offset = {0, 0}; // Offset for the render area
    VkExtent2D extent = {0, 0}; // Extent of the render area

    RenderPassBeginInfo() = default;
    RenderPassBeginInfo(const color &clear_color, float clear_depth = 1.0f,
                        uint32_t clear_stencil = 0);

    RenderPassBeginInfo &set_clear_flags(ClearFlags flags);
    RenderPassBeginInfo &set_render_pass(VkRenderPass render_pass);
    RenderPassBeginInfo &set_framebuffer(VkFramebuffer framebuffer);
    RenderPassBeginInfo &set_clear_color(const color &color);
    RenderPassBeginInfo &set_clear_depth(float depth);
    RenderPassBeginInfo &set_clear_stencil(uint32_t stencil);
    RenderPassBeginInfo &set_offset(uint32_t offset_x, uint32_t offset_y);
    RenderPassBeginInfo &set_extent(uint32_t extent_x, uint32_t extent_y);
    RenderPassBeginInfo &set_extent(VkExtent2D extent);
};

struct Viewport {
    vec2 offset;
    vec2 extent;
    float min_depth = 0.0f; // Default minimum depth value
    float max_depth = 1.0f; // Default maximum depth value
};

struct Scissor {
    uint32_t offset_x = 0; // X offset of the scissor rectangle
    uint32_t offset_y = 0; // Y offset of the scissor rectangle
    uint32_t extent_x = 0; // Width of the scissor rectangle
    uint32_t extent_y = 0; // Height of the scissor rectangle
};

struct BindVertexBuffers {
    alchemist::dual_vector<VkBuffer, VkDeviceSize>
        vertex_buffers;         // Vector of vertex buffers and their offsets
    uint32_t first_binding = 0; // First binding index for the vertex buffers

    BindVertexBuffers() = default;
    BindVertexBuffers(uint32_t initial_capacity);

    BindVertexBuffers &add_buffer(VkBuffer buffer, VkDeviceSize offset = 0);
    BindVertexBuffers &set_first_binding(uint32_t first_binding);
};

struct BindDescriptorSets {
    VkPipelineBindPoint bind_point =
        VK_PIPELINE_BIND_POINT_GRAPHICS; // Bind point for the descriptor sets

    VkPipelineLayout
        pipeline_layout; // Pipeline layout to bind the descriptor sets to

    uint32_t first_set = 0; // First set index

    alchemist::vector<VkDescriptorSet> sets; // Array of descriptor sets to bind
    alchemist::vector<uint32_t>
        dynamic_offsets; // Dynamic offsets for the descriptor sets

    BindDescriptorSets() = default;

    BindDescriptorSets(
        VkPipelineLayout pipeline_layout, uint32_t first_set = 0,
        VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS);

    BindDescriptorSets &add_set(VkDescriptorSet set);
    BindDescriptorSets &add_dynamic_offset(uint32_t dynamic_offset);

    BindDescriptorSets &set_first_set(uint32_t first_set);
    BindDescriptorSets &set_pipeline_layout(VkPipelineLayout pipeline_layout);
    BindDescriptorSets &set_bind_point(
        VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS);
};

void begin_render_pass(VkCommandBuffer command_buffer,
                       const RenderPassBeginInfo &begin_info);
void end_render_pass(VkCommandBuffer command_buffer);

void bind_pipeline(
    VkCommandBuffer command_buffer, VkPipeline pipeline,
    VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS);

void set_viewport(VkCommandBuffer command_buffer, const Viewport &viewport);
void set_scissor(VkCommandBuffer command_buffer, const Scissor &scissor);

void bind_vertex_buffers(VkCommandBuffer command_buffer,
                         const BindVertexBuffers &bind_buffers);
void bind_index_buffer(VkCommandBuffer command_buffer, VkBuffer buffer,
                       VkDeviceSize offset = 0,
                       VkIndexType index_type = VK_INDEX_TYPE_UINT32);

void bind_descriptor_sets(VkCommandBuffer command_buffer,
                          const BindDescriptorSets &bind_sets);

void draw(VkCommandBuffer command_buffer, uint32_t vertex_count,
          uint32_t instance_count = 1, uint32_t first_vertex = 0,
          uint32_t first_instance = 0);
void draw_indexed(VkCommandBuffer command_buffer, uint32_t index_count,
                  uint32_t instance_count = 1, uint32_t first_index = 0,
                  int32_t vertex_offset = 0, uint32_t first_instance = 0);

void copy_buffer(VkCommandBuffer command_buffer, VkBuffer src_buffer,
                 VkBuffer dst_buffer, const VkBufferCopy &region);

#endif // ALCHEMIST_GRAPHICS_VULKAN_COMMAND_HPP
