
#ifndef ALCHEMIST_GRAPHICS_VULKAN_PIPELINE_HPP
#define ALCHEMIST_GRAPHICS_VULKAN_PIPELINE_HPP

#include <vulkan/vulkan.h>

#include "graphics/vulkan/descriptor_set.hpp"
#include "graphics/vulkan/shader.hpp"
#include "graphics/vulkan/vertex_input.hpp"
#include "memory/vector.hpp"

struct PipelineLayoutInfo {
    alchemist::vector<VkDescriptorSetLayout> setLayouts;

    PipelineLayoutInfo() = default;

    PipelineLayoutInfo &add_set_layout(VkDescriptorSetLayout layout);
};

struct GraphicsPipelineInfo {
    VkPrimitiveTopology topology;
    VkPolygonMode polygonMode;
    VkCullModeFlags cullMode;
    VkFrontFace frontFace;

    ShaderModule *modules = nullptr;
    uint32_t moduleCount = 0;

    VertexInput vertex_input;

    bool depth_testing = true;
    bool depthWriteEnable = true;

    VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS;
    float min_depthBounds = 0.0f;
    float max_depthBounds = 1.0f;

    GraphicsPipelineInfo(VkPrimitiveTopology topology,
                         VkPolygonMode polygonMode, VkCullModeFlags cullMode,
                         VkFrontFace frontFace);

    GraphicsPipelineInfo &add_shader_module(const ShaderModule &module);
    GraphicsPipelineInfo &set_vertex_input(const VertexInput &input);
    GraphicsPipelineInfo &set_depth_testing(bool enable);
    GraphicsPipelineInfo &set_depth_write(bool enable);
    GraphicsPipelineInfo &set_depth_compare_op(VkCompareOp compareOp);
    GraphicsPipelineInfo &set_depth_bounds(float minBounds, float maxBounds);
};

VkPipelineLayout create_pipeline_layout(VkDevice device,
                                        const PipelineLayoutInfo &info);

VkPipeline create_graphics_pipeline(VkDevice device, VkPipelineLayout layout,
                                    VkRenderPass render_pass,
                                    const GraphicsPipelineInfo &info);

#endif // ALCHEMIST_GRAPHICS_VULKAN_PIPELINE_HPP
