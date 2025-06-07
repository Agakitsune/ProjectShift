
#include "graphics/vulkan/pipeline.hpp"

#include <iostream>

PipelineLayoutInfo &
PipelineLayoutInfo::add_set_layout(VkDescriptorSetLayout layout) {
    setLayouts.push(layout);
    return *this;
}

GraphicsPipelineInfo::GraphicsPipelineInfo(VkPrimitiveTopology topology,
                                           VkPolygonMode polygon_mode,
                                           VkCullModeFlags cull_mode,
                                           VkFrontFace front_face)
    : topology(topology), polygonMode(polygon_mode), cullMode(cull_mode),
      frontFace(front_face), vertex_input(VertexInput()), depth_testing(true),
      depthWriteEnable(true), depthCompareOp(VK_COMPARE_OP_LESS),
      min_depthBounds(0.0f), max_depthBounds(1.0f) {}

GraphicsPipelineInfo &
GraphicsPipelineInfo::add_shader_module(const ShaderModule &module) {
    if (moduleCount == 0) {
        modules = new ShaderModule[1];
    } else {
        ShaderModule *new_modules = new ShaderModule[moduleCount + 1];
        std::copy(modules, modules + moduleCount, new_modules);
        delete[] modules;
        modules = new_modules;
    }
    modules[moduleCount] = module;
    moduleCount++;
    return *this;
}

GraphicsPipelineInfo &
GraphicsPipelineInfo::set_vertex_input(const VertexInput &input) {
    vertex_input = input;
    return *this;
}

GraphicsPipelineInfo &GraphicsPipelineInfo::set_depth_testing(bool enable) {
    depth_testing = enable;
    return *this;
}

GraphicsPipelineInfo &GraphicsPipelineInfo::set_depth_write(bool enable) {
    depthWriteEnable = enable;
    return *this;
}

GraphicsPipelineInfo &
GraphicsPipelineInfo::set_depth_compare_op(VkCompareOp compare_op) {
    depthCompareOp = compare_op;
    return *this;
}

GraphicsPipelineInfo &GraphicsPipelineInfo::set_depth_bounds(float min_bounds,
                                                             float max_bounds) {
    min_depthBounds = min_bounds;
    max_depthBounds = max_bounds;
    return *this;
}

VkPipelineLayout create_pipeline_layout(VkDevice device,
                                        const PipelineLayoutInfo &info) {
    VkPipelineLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.setLayoutCount = info.setLayouts.size;
    layout_info.pSetLayouts = info.setLayouts.data;

    VkPipelineLayout pipeline_layout;
    if (vkCreatePipelineLayout(device, &layout_info, nullptr, &pipeline_layout) !=
        VK_SUCCESS) {
        return VK_NULL_HANDLE; // Handle pipeline layout creation failure
    }

    return pipeline_layout;
}

VkPipeline create_graphics_pipeline(VkDevice device, VkPipelineLayout layout,
                                    VkRenderPass render_pass,
                                    const GraphicsPipelineInfo &info) {
    VkPipelineShaderStageCreateInfo *shader_stages =
        new VkPipelineShaderStageCreateInfo[info.moduleCount];
    for (uint32_t i = 0; i < info.moduleCount; ++i) {
        shader_stages[i] = {};
        shader_stages[i].sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[i].stage = info.modules[i].stage;
        shader_stages[i].module = info.modules[i].module;
        shader_stages[i].pName = info.modules[i].entry;
    }

    VkPipelineInputAssemblyStateCreateInfo input_assembly{};
    input_assembly.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = info.topology;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewport_state{};
    viewport_state.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = info.polygonMode;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = info.cullMode;
    rasterizer.frontFace = info.frontFace;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    color_blend_attachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo color_blending{};
    color_blending.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment;
    color_blending.blendConstants[0] = 0.0f;
    color_blending.blendConstants[1] = 0.0f;
    color_blending.blendConstants[2] = 0.0f;
    color_blending.blendConstants[3] = 0.0f;

    VkDynamicState dynamic_states[2] = {VK_DYNAMIC_STATE_VIEWPORT,
                                        VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamic_state{};
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = 2;
    dynamic_state.pDynamicStates = dynamic_states;

    VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
    depth_stencil.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = info.depth_testing;
    depth_stencil.depthWriteEnable = info.depthWriteEnable;
    depth_stencil.depthCompareOp = info.depthCompareOp;
    depth_stencil.depthBoundsTestEnable = VK_FALSE;
    depth_stencil.stencilTestEnable = VK_FALSE; // TODO
    depth_stencil.minDepthBounds = info.min_depthBounds;
    depth_stencil.maxDepthBounds = info.max_depthBounds;

    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount =
        info.vertex_input.binding_count;
    vertex_input_info.pVertexBindingDescriptions = info.vertex_input.bindings;
    vertex_input_info.vertexAttributeDescriptionCount =
        info.vertex_input.attribute_count;
    vertex_input_info.pVertexAttributeDescriptions = info.vertex_input.attributes;

    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = info.moduleCount;
    pipeline_info.pStages = shader_stages;
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pMultisampleState = &multisampling;
    pipeline_info.pColorBlendState = &color_blending;
    pipeline_info.pDepthStencilState = &depth_stencil;
    pipeline_info.pDynamicState = &dynamic_state;
    pipeline_info.layout = layout;
    pipeline_info.renderPass = render_pass;
    pipeline_info.subpass = 0;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

    VkPipeline graphics_pipeline;
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info,
                                  nullptr, &graphics_pipeline) != VK_SUCCESS) {
        delete[] shader_stages;
        return VK_NULL_HANDLE; // Handle pipeline creation failure
    }
    delete[] shader_stages;

    std::cout << "Graphics pipeline created successfully." << std::endl;

    return graphics_pipeline;
}
