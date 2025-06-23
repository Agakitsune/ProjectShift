
#ifndef ALCHEMIST_SERVER_PIPELINE_HPP
#define ALCHEMIST_SERVER_PIPELINE_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "server/rid.hpp"

#include "math/vector/vec2.hpp"
#include "math/vector/vec3.hpp"
#include "math/vector/vec4.hpp"

template <typename T>
struct VertexInputFormat;

template <>
struct VertexInputFormat<float> {
    static constexpr VkFormat format = VK_FORMAT_R32_SFLOAT; // Format for single float vertex attribute
};

template <>
struct VertexInputFormat<uint32_t> {
    static constexpr VkFormat format = VK_FORMAT_R32_UINT; // Format for single uint32_t vertex attribute
};

template <>
struct VertexInputFormat<vec2> {
    static constexpr VkFormat format = VK_FORMAT_R32G32_SFLOAT; // Format for vec2 vertex attribute
};

template <>
struct VertexInputFormat<vec3> {
    static constexpr VkFormat format = VK_FORMAT_R32G32B32_SFLOAT; // Format for vec3 vertex attribute
};

template <>
struct VertexInputFormat<vec4> {
    static constexpr VkFormat format = VK_FORMAT_R32G32B32A32_SFLOAT; // Format for vec4 vertex attribute
};

struct Pipeline {
    VkPipeline pipeline;
    RID rid = RID_INVALID; // Resource ID for the pipeline
    RID layout = RID_INVALID; // Resource ID for the pipeline layout

    Pipeline() = default;
};

struct PipelineLayout {
    VkPipelineLayout layout; // Vulkan pipeline layout object
    RID rid = RID_INVALID; // Resource ID for the pipeline layout

    PipelineLayout() = default;
};

struct PipelineBuilder;

struct ShaderBuilder {
    VkPipelineShaderStageCreateInfo &stage_info; // Vulkan shader stage creation info

    ShaderBuilder(VkPipelineShaderStageCreateInfo &info);

    ShaderBuilder &set_stage(VkShaderStageFlagBits stage);
    ShaderBuilder &set_module(RID module);
    ShaderBuilder &set_name(const char *name);
};

struct VertexInput {
    std::vector<VkVertexInputBindingDescription> bindings; // Vertex input binding descriptions
    std::vector<VkVertexInputAttributeDescription> attributes; // Vertex input attribute descriptions

    VkPipelineVertexInputStateCreateInfo &create_info;

    VertexInput(VkPipelineVertexInputStateCreateInfo &info);

    VertexInput &add_binding(uint32_t binding, uint32_t stride, VkVertexInputRate input_rate = VK_VERTEX_INPUT_RATE_VERTEX);
    VertexInput &add_attribute(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset);

    template <typename T>
    VertexInput &add_binding(uint32_t binding, VkVertexInputRate input_rate = VK_VERTEX_INPUT_RATE_VERTEX) {
        static_assert(std::is_standard_layout_v<T>, "T must be a standard layout type");
        return add_binding(binding, sizeof(T), input_rate);
    }

    template <typename T>
    VertexInput &add_attribute(uint32_t location, uint32_t binding, uint32_t offset) {
        static_assert(std::is_standard_layout_v<T>, "T must be a standard layout type");
        return add_attribute(location, binding, VertexInputFormat<T>::format, offset);
    }

    void build() const;
};

struct RasterizerBuilder {
    VkPipelineRasterizationStateCreateInfo &rasterizer_info; // Vulkan rasterization state creation info

    RasterizerBuilder(VkPipelineRasterizationStateCreateInfo &info);

    RasterizerBuilder &set_depth_clamp_enable(VkBool32 enable);
    RasterizerBuilder &set_rasterizer_discard_enable(VkBool32 enable);
    RasterizerBuilder &set_polygon_mode(VkPolygonMode mode);
    RasterizerBuilder &set_cull_mode(VkCullModeFlags mode);
    RasterizerBuilder &set_front_face(VkFrontFace front_face);
    RasterizerBuilder &set_depth_bias_enable(VkBool32 enable);
    RasterizerBuilder &set_depth_bias_constant_factor(float factor);
    RasterizerBuilder &set_depth_bias_clamp(float clamp);
    RasterizerBuilder &set_depth_bias_slope_factor(float factor);
};

struct DepthStencilBuilder {
    VkPipelineDepthStencilStateCreateInfo &depth_stencil_info; // Vulkan depth stencil state creation info

    DepthStencilBuilder(VkPipelineDepthStencilStateCreateInfo &info);

    DepthStencilBuilder &set_depth_test_enable(VkBool32 enable);
    DepthStencilBuilder &set_depth_write_enable(VkBool32 enable);
    DepthStencilBuilder &set_depth_compare_op(VkCompareOp compare_op);
    DepthStencilBuilder &set_depth_bounds_test_enable(VkBool32 enable);
    DepthStencilBuilder &set_stencil_test_enable(VkBool32 enable);
    DepthStencilBuilder &set_front_stencil_op(const VkStencilOpState &front);
    DepthStencilBuilder &set_back_stencil_op(const VkStencilOpState &back);
};

struct ColorBlendBuilder {
    std::vector<VkPipelineColorBlendAttachmentState> attachments; // Vector of color blend attachment states
    VkBool32 logic_op_enable = VK_FALSE; // Logic operation enable flag
    VkLogicOp logic_op = VK_LOGIC_OP_COPY; // Default logic operation
    vec4 blend_constants = vec4(0.0f, 0.0f, 0.0f, 0.0f); // Blend constants

    PipelineBuilder &builder; // Reference to the pipeline builder

    ColorBlendBuilder(PipelineBuilder &builder);

    ColorBlendBuilder &add_attachment(const VkPipelineColorBlendAttachmentState &attachment);
    ColorBlendBuilder &set_logic_op_enable(VkBool32 enable);
    ColorBlendBuilder &set_logic_op(VkLogicOp logic_op);
    ColorBlendBuilder &set_blend_constants(float r, float g, float b, float a);

    void build() const;
};

struct DynamicStateBuilder {
    std::vector<VkDynamicState> dynamic_states; // Vector of dynamic states

    PipelineBuilder &builder; // Reference to the pipeline builder

    DynamicStateBuilder(PipelineBuilder &builder);

    DynamicStateBuilder &add_dynamic_state(VkDynamicState state);

    void build() const;
};

struct PipelineLayoutServer;

struct PipelineLayoutBuilder {
    std::vector<VkDescriptorSetLayout> set_layouts; // Vector of descriptor set layouts

    PipelineLayoutServer &server;

    PipelineLayoutBuilder(PipelineLayoutServer &server);

    PipelineLayoutBuilder &add_layout(RID layout);

    RID build() const;
};

struct PipelineServer;

struct PipelineBuilder {
    std::vector<VkPipelineShaderStageCreateInfo> stages;
    VkGraphicsPipelineCreateInfo create_info; // Vulkan graphics pipeline creation info

    PipelineServer &server;

    PipelineBuilder(PipelineServer &server);

    ShaderBuilder add_shader(RID module, VkShaderStageFlagBits stage);
    VertexInput set_vertex_input();
    PipelineBuilder &set_input_assembly(VkPrimitiveTopology topology, VkBool32 primitive_restart = VK_FALSE);
    RasterizerBuilder set_rasterizer();
    DepthStencilBuilder set_depth_stencil();
    ColorBlendBuilder set_color_blend();
    DynamicStateBuilder set_dynamic_state();
    PipelineBuilder &set_layout(RID layout);
    PipelineBuilder &set_render_pass(RID render_pass);
    PipelineBuilder &set_subpass(uint32_t subpass);

    RID build();
};

struct SimplePipelineBuilder {
    std::vector<VkPipelineShaderStageCreateInfo> stages;
    VkGraphicsPipelineCreateInfo create_info; // Vulkan graphics pipeline creation info

    PipelineServer &server;

    SimplePipelineBuilder(PipelineServer &server);

    ShaderBuilder add_shader(RID module, VkShaderStageFlagBits stage);
    VertexInput set_vertex_input();
    SimplePipelineBuilder &set_input_assembly(VkPrimitiveTopology topology, VkBool32 primitive_restart = VK_FALSE);
    DepthStencilBuilder set_depth_stencil();
    SimplePipelineBuilder &set_layout(RID layout);
    SimplePipelineBuilder &set_render_pass(RID render_pass);

    RID build();
};

struct PipelineLayoutServer {
    std::vector<PipelineLayout> pipeline_layouts; // Vector to hold all pipeline layouts

    VkDevice device; // Vulkan device

    PipelineLayoutServer(VkDevice device);
    ~PipelineLayoutServer();

    RID new_pipeline_layout(const VkPipelineLayoutCreateInfo &create_info);
    RID new_pipeline_layout(VkPipelineLayoutCreateInfo &&create_info);

    PipelineLayoutBuilder new_pipeline_layout(); // Create a new pipeline layout builder

    const PipelineLayout &get_pipeline_layout(RID rid) const;

    static PipelineLayoutServer &instance();
    static std::unique_ptr<PipelineLayoutServer> __instance; // Singleton instance of PipelineLayoutServer
};

struct PipelineServer {
    std::vector<Pipeline> pipelines;

    VkDevice device; // Vulkan device

    PipelineServer(VkDevice device);
    ~PipelineServer();

    RID new_pipeline(const VkGraphicsPipelineCreateInfo &create_info);
    RID new_pipeline(VkGraphicsPipelineCreateInfo &&create_info);

    PipelineBuilder new_pipeline(); // Create a new pipeline builder
    SimplePipelineBuilder new_simple_pipeline(); // Create a new simple pipeline builder

    Pipeline &get_pipeline(RID rid);
    const Pipeline &get_pipeline(RID rid) const;

    static PipelineServer &instance();
    static std::unique_ptr<PipelineServer> __instance; // Singleton instance of PipelineServer
};

#endif // ALCHEMIST_SERVER_PIPELINE_HPP
