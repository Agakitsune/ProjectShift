
#ifdef ALCHEMIST_DEBUG
#include <iostream>
#endif 

#include <cstring>

#include "server/pipeline.hpp"
#include "server/shader.hpp"
#include "server/descriptor.hpp"
#include "server/render_pass.hpp"

ShaderBuilder::ShaderBuilder(VkPipelineShaderStageCreateInfo &info) : stage_info(info) {
    stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO; // Initialize the structure type
    stage_info.pName = "main"; // Default entry point name, can be changed later
    stage_info.stage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM; // Default stage, can be set later
    stage_info.module = VK_NULL_HANDLE; // Default module, must be set before use
}

ShaderBuilder &ShaderBuilder::set_stage(VkShaderStageFlagBits stage) {
    stage_info.stage = stage;
    return *this; // Return the current instance for method chaining
}

ShaderBuilder &ShaderBuilder::set_module(RID module) {
    const Shader &shader = ShaderServer::instance().get_shader(module);
    if (shader.shader_module == VK_NULL_HANDLE) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Shader module is not valid!" << std::endl;
        #endif
        return *this; // If the shader module is invalid, do not set it
    }
    stage_info.module = shader.shader_module; // Set the shader module for the stage
    return *this; // Return the current instance for method chaining
}

ShaderBuilder &ShaderBuilder::set_name(const char *name) {
    if (name != nullptr) {
        stage_info.pName = name; // Set the entry point name for the shader stage
    } else {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Shader entry point name cannot be null!" << std::endl;
        #endif
    }
    return *this; // Return the current instance for method chaining
}



VertexInput::VertexInput(VkPipelineVertexInputStateCreateInfo &info) : create_info(info) {
    // Initialize the vertex input with empty bindings and attributes
}

VertexInput &VertexInput::add_binding(uint32_t binding, uint32_t stride, VkVertexInputRate input_rate) {
    VkVertexInputBindingDescription binding_desc{};
    binding_desc.binding = binding;
    binding_desc.stride = stride;
    binding_desc.inputRate = input_rate;

    bindings.push_back(binding_desc); // Add the binding description to the vector
    return *this; // Return the current instance for method chaining
}

VertexInput &VertexInput::add_attribute(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset) {
    VkVertexInputAttributeDescription attribute_desc{};
    attribute_desc.location = location;
    attribute_desc.binding = binding;
    attribute_desc.format = format;
    attribute_desc.offset = offset;

    attributes.push_back(attribute_desc); // Add the attribute description to the vector
    return *this; // Return the current instance for method chaining
}

void VertexInput::build() const {
    if (bindings.empty() && attributes.empty()) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "No vertex input bindings or attributes defined!" << std::endl;
        #endif
        return; // If no bindings or attributes are defined, do nothing
    }

    VkVertexInputBindingDescription *binding_descs = new VkVertexInputBindingDescription[bindings.size()]; // Allocate memory for binding descriptions
    VkVertexInputAttributeDescription *attribute_descs = new VkVertexInputAttributeDescription[attributes.size()]; // Allocate memory for attribute descriptions

    for (size_t i = 0; i < bindings.size(); ++i) {
        binding_descs[i] = std::move(bindings[i]); // Copy binding descriptions to the allocated array
    }

    for (size_t i = 0; i < attributes.size(); ++i) {
        attribute_descs[i] = std::move(attributes[i]); // Copy attribute descriptions to the allocated array
    }

    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO; // Initialize the structure type
    create_info.pNext = nullptr; // No additional structures
    create_info.flags = 0; // No flags
    create_info.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size());
    create_info.pVertexBindingDescriptions = binding_descs; // Set the vertex binding descriptions
    create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
    create_info.pVertexAttributeDescriptions = attribute_descs; // Set the vertex attribute descriptions
}



RasterizerBuilder::RasterizerBuilder(VkPipelineRasterizationStateCreateInfo &info) : rasterizer_info(info) {
    rasterizer_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO; // Initialize the structure type
    rasterizer_info.pNext = nullptr; // No additional structures
    rasterizer_info.flags = 0; // No flags
    rasterizer_info.depthClampEnable = VK_FALSE; // Default depth clamp disabled
    rasterizer_info.rasterizerDiscardEnable = VK_FALSE; // Default rasterizer discard disabled
    rasterizer_info.polygonMode = VK_POLYGON_MODE_FILL; // Default polygon mode is fill
    rasterizer_info.cullMode = VK_CULL_MODE_BACK_BIT; // Default cull mode is back face culling
    rasterizer_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // Default front face is counter-clockwise
    rasterizer_info.depthBiasEnable = VK_FALSE; // Default depth bias disabled
    rasterizer_info.depthBiasConstantFactor = 0.0f; // Default depth bias constant factor
    rasterizer_info.depthBiasClamp = 0.0f; // Default depth bias clamp
    rasterizer_info.depthBiasSlopeFactor = 0.0f; // Default depth bias slope factor
}

RasterizerBuilder &RasterizerBuilder::set_depth_clamp_enable(VkBool32 enable) {
    rasterizer_info.depthClampEnable = enable; // Set depth clamp enable
    return *this; // Return the current instance for method chaining
}

RasterizerBuilder &RasterizerBuilder::set_rasterizer_discard_enable(VkBool32 enable) {
    rasterizer_info.rasterizerDiscardEnable = enable; // Set rasterizer discard enable
    return *this; // Return the current instance for method chaining
}

RasterizerBuilder &RasterizerBuilder::set_polygon_mode(VkPolygonMode mode) {
    rasterizer_info.polygonMode = mode; // Set the polygon mode
    return *this; // Return the current instance for method chaining
}

RasterizerBuilder &RasterizerBuilder::set_cull_mode(VkCullModeFlags mode) {
    rasterizer_info.cullMode = mode; // Set the cull mode
    return *this; // Return the current instance for method chaining
}

RasterizerBuilder &RasterizerBuilder::set_front_face(VkFrontFace front_face) {
    rasterizer_info.frontFace = front_face; // Set the front face orientation
    return *this; // Return the current instance for method chaining
}

RasterizerBuilder &RasterizerBuilder::set_depth_bias_enable(VkBool32 enable) {
    rasterizer_info.depthBiasEnable = enable; // Set depth bias enable
    return *this; // Return the current instance for method chaining
}

RasterizerBuilder &RasterizerBuilder::set_depth_bias_constant_factor(float factor) {
    rasterizer_info.depthBiasConstantFactor = factor; // Set depth bias constant factor
    return *this; // Return the current instance for method chaining
}

RasterizerBuilder &RasterizerBuilder::set_depth_bias_clamp(float clamp) {
    rasterizer_info.depthBiasClamp = clamp; // Set depth bias clamp
    return *this; // Return the current instance for method chaining
}

RasterizerBuilder &RasterizerBuilder::set_depth_bias_slope_factor(float factor) {
    rasterizer_info.depthBiasSlopeFactor = factor; // Set depth bias slope factor
    return *this; // Return the current instance for method chaining
}



DepthStencilBuilder::DepthStencilBuilder(VkPipelineDepthStencilStateCreateInfo &info) : depth_stencil_info(info) {
    depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO; // Initialize the structure type
    depth_stencil_info.pNext = nullptr; // No additional structures
    depth_stencil_info.flags = 0; // No flags
    depth_stencil_info.depthTestEnable = VK_FALSE; // Default depth test disabled
    depth_stencil_info.depthWriteEnable = VK_FALSE; // Default depth write disabled
    depth_stencil_info.depthCompareOp = VK_COMPARE_OP_ALWAYS; // Default compare operation is always
    depth_stencil_info.depthBoundsTestEnable = VK_FALSE; // Default depth bounds test disabled
    depth_stencil_info.stencilTestEnable = VK_FALSE; // Default stencil test disabled
    depth_stencil_info.front = {}; // Default front stencil operation state
    depth_stencil_info.back = {}; // Default back stencil operation state
}

DepthStencilBuilder &DepthStencilBuilder::set_depth_test_enable(VkBool32 enable) {
    depth_stencil_info.depthTestEnable = enable; // Set depth test enable
    return *this; // Return the current instance for method chaining
}

DepthStencilBuilder &DepthStencilBuilder::set_depth_write_enable(VkBool32 enable) {
    depth_stencil_info.depthWriteEnable = enable; // Set depth write enable
    return *this; // Return the current instance for method chaining
}

DepthStencilBuilder &DepthStencilBuilder::set_depth_compare_op(VkCompareOp compare_op) {
    depth_stencil_info.depthCompareOp = compare_op; // Set the depth compare operation
    return *this; // Return the current instance for method chaining
}

DepthStencilBuilder &DepthStencilBuilder::set_depth_bounds_test_enable(VkBool32 enable) {
    depth_stencil_info.depthBoundsTestEnable = enable; // Set depth bounds test enable
    return *this; // Return the current instance for method chaining
}

DepthStencilBuilder &DepthStencilBuilder::set_stencil_test_enable(VkBool32 enable) {
    depth_stencil_info.stencilTestEnable = enable; // Set stencil test enable
    return *this; // Return the current instance for method chaining
}

DepthStencilBuilder &DepthStencilBuilder::set_front_stencil_op(const VkStencilOpState &front) {
    depth_stencil_info.front = front; // Set the front stencil operation state
    return *this; // Return the current instance for method chaining
}

DepthStencilBuilder &DepthStencilBuilder::set_back_stencil_op(const VkStencilOpState &back) {
    depth_stencil_info.back = back; // Set the back stencil operation state
    return *this; // Return the current instance for method chaining
}



ColorBlendBuilder::ColorBlendBuilder(PipelineBuilder &builder) : builder(builder) {
    
}

ColorBlendBuilder &ColorBlendBuilder::add_attachment(const VkPipelineColorBlendAttachmentState &attachment) {
    attachments.push_back(attachment); // Add the color blend attachment state to the vector
    return *this; // Return the current instance for method chaining
}

ColorBlendBuilder &ColorBlendBuilder::set_logic_op_enable(VkBool32 enable) {
    logic_op_enable = enable; // Set the logic operation enable flag for the color blend state
    return *this; // Return the current instance for method chaining
}

ColorBlendBuilder &ColorBlendBuilder::set_logic_op(VkLogicOp logic_op) {
    this->logic_op = logic_op; // Set the logic operation for the color blend state
    return *this; // Return the current instance for method chaining
}

ColorBlendBuilder &ColorBlendBuilder::set_blend_constants(float r, float g, float b, float a) {
    blend_constants = vec4(r, g, b, a); // Set the blend constants for the color blend state
    return *this; // Return the current instance for method chaining
}

void ColorBlendBuilder::build() const {
    if (attachments.empty()) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "No color blend attachments defined!" << std::endl;
        #endif
        return; // If no attachments are defined, do nothing
    }

    VkPipelineColorBlendStateCreateInfo *color_blend_state = new VkPipelineColorBlendStateCreateInfo; // Allocate memory for color blend state
    VkPipelineColorBlendAttachmentState *attachment_states = new VkPipelineColorBlendAttachmentState[attachments.size()]; // Allocate memory for attachment states

    for (size_t i = 0; i < attachments.size(); ++i) {
        attachment_states[i] = std::move(attachments[i]); // Copy attachment states to the allocated array
    }
    
    color_blend_state->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO; // Initialize the structure type
    color_blend_state->pNext = nullptr; // No additional structures
    color_blend_state->flags = 0; // No flags
    color_blend_state->logicOpEnable = logic_op_enable; // Default logic operation disabled
    color_blend_state->logicOp = logic_op; // Default logic operation is copy
    color_blend_state->attachmentCount = static_cast<uint32_t>(attachments.size());
    color_blend_state->pAttachments = attachment_states; // Set the color blend attachments
    color_blend_state->blendConstants[0] = blend_constants[0];
    color_blend_state->blendConstants[1] = blend_constants[1];
    color_blend_state->blendConstants[2] = blend_constants[2];
    color_blend_state->blendConstants[3] = blend_constants[3];

    builder.create_info.pColorBlendState = color_blend_state; // Set the color blend state in the pipeline create info
}



DynamicStateBuilder::DynamicStateBuilder(PipelineBuilder &builder) : builder(builder) {
    // Initialize the dynamic state builder with an empty vector of dynamic states
}

DynamicStateBuilder &DynamicStateBuilder::add_dynamic_state(VkDynamicState state) {
    dynamic_states.push_back(state); // Add the dynamic state to the vector
    return *this; // Return the current instance for method chaining
}

void DynamicStateBuilder::build() const {
    if (dynamic_states.empty()) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "No dynamic states defined!" << std::endl;
        #endif
        return; // If no dynamic states are defined, do nothing
    }

    VkPipelineDynamicStateCreateInfo *dynamic_state_info = new VkPipelineDynamicStateCreateInfo; // Allocate memory for dynamic state info
    VkDynamicState *dynamic_states_array = new VkDynamicState[dynamic_states.size()]; // Allocate memory for dynamic states

    for (size_t i = 0; i < dynamic_states.size(); ++i) {
        dynamic_states_array[i] = dynamic_states[i]; // Copy dynamic states to the allocated array
    }
    
    dynamic_state_info->sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO; // Initialize the structure type
    dynamic_state_info->pNext = nullptr; // No additional structures
    dynamic_state_info->flags = 0; // No flags
    dynamic_state_info->dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
    dynamic_state_info->pDynamicStates = dynamic_states_array; // Set the dynamic states

    builder.create_info.pDynamicState = dynamic_state_info; // Set the dynamic state info in the pipeline create info
}



PipelineLayoutBuilder::PipelineLayoutBuilder(PipelineLayoutServer &server) : server(server) {
    // Initialize the pipeline layout builder with an empty vector of descriptor set layouts
}

PipelineLayoutBuilder &PipelineLayoutBuilder::add_layout(RID layout) {
    const DescriptorLayout &desc_layout = DescriptorLayoutServer::instance().get_descriptor_layout(layout);
    if (desc_layout.layout == VK_NULL_HANDLE) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Descriptor set layout is not valid!" << std::endl;
        #endif
        return *this; // If the descriptor set layout is invalid, do not add it
    }
    set_layouts.push_back(desc_layout.layout); // Add the descriptor set layout to the vector
    return *this; // Return the current instance for method chaining
}

RID PipelineLayoutBuilder::build() const {
    if (set_layouts.empty()) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "No descriptor set layouts defined!" << std::endl;
        #endif
        return RID_INVALID; // If no descriptor set layouts are defined, return an invalid RID
    }

    VkDescriptorSetLayout *set_layouts_array = new VkDescriptorSetLayout[set_layouts.size()]; // Allocate memory for descriptor set layouts

    for (size_t i = 0; i < set_layouts.size(); ++i) {
        set_layouts_array[i] = set_layouts[i]; // Copy descriptor set layouts to the allocated array
    }

    VkPipelineLayoutCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO; // Initialize the structure type
    create_info.pNext = nullptr; // No additional structures
    create_info.flags = 0; // No flags
    create_info.setLayoutCount = static_cast<uint32_t>(set_layouts.size());
    create_info.pSetLayouts = set_layouts_array; // Set the descriptor set layouts

    return server.new_pipeline_layout(create_info); // Create a new pipeline layout and return its RID
}



PipelineBuilder::PipelineBuilder(PipelineServer &server) : server(server) {
    VkPipelineMultisampleStateCreateInfo *multisample_info = new VkPipelineMultisampleStateCreateInfo; // Allocate memory for multisample state

    multisample_info->sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO; // Initialize the structure type
    multisample_info->rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; // Default rasterization samples is 1
    multisample_info->sampleShadingEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo *viewport_state = new VkPipelineViewportStateCreateInfo;
    
    viewport_state->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state->viewportCount = 1;
    viewport_state->scissorCount = 1;

    create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO; // Initialize the structure type
    create_info.pNext = nullptr; // No additional structures
    create_info.flags = 0; // No flags
    create_info.stageCount = 0; // No shader stages defined yet
    create_info.pStages = nullptr; // Pointer to shader stages, will be set later
    create_info.pVertexInputState = nullptr; // Vertex input state, will be set later
    create_info.pInputAssemblyState = nullptr; // Input assembly state, will be set later
    create_info.pViewportState = viewport_state;
    create_info.pRasterizationState = nullptr; // Rasterization state, will be set later
    create_info.pColorBlendState = nullptr; // Color blend state, will be set later
    create_info.pMultisampleState = multisample_info; // Multisample state, will be set later
    create_info.pDepthStencilState = nullptr; // Depth stencil state, will be set later
    create_info.pDynamicState = nullptr; // Dynamic state, will be set later
    create_info.layout = VK_NULL_HANDLE; // Pipeline layout, must be set before use
    create_info.renderPass = VK_NULL_HANDLE; // Render pass, must be set before use
    create_info.subpass = 0; // Subpass index, default is 0
}

ShaderBuilder PipelineBuilder::add_shader(RID module, VkShaderStageFlagBits stage) {
    stages.push_back(VkPipelineShaderStageCreateInfo{}); // Add a new shader stage to the stages vector
    VkPipelineShaderStageCreateInfo &stage_info = stages.back(); // Get a reference to the last added shader stage info
    
    stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO; // Initialize the structure type
    stage_info.pNext = nullptr; // No additional structures
    stage_info.flags = 0; // No flags
    stage_info.stage = stage; // Set the shader stage
    stage_info.module = VK_NULL_HANDLE; // Default module, must be set later
    stage_info.pName = "main"; // Default entry point name, can be changed later

    ShaderBuilder shader_builder(stage_info); // Create a new shader builder
    shader_builder.set_stage(stage); // Set the shader stage
    shader_builder.set_module(module); // Set the shader module
    return shader_builder; // Return the shader builder for further configuration
}

VertexInput PipelineBuilder::set_vertex_input() {
    VkPipelineVertexInputStateCreateInfo *vertex_input_state = new VkPipelineVertexInputStateCreateInfo;
    VertexInput vertex_input(*vertex_input_state); // Create a new vertex input builder
    create_info.pVertexInputState = vertex_input_state;
    return vertex_input; // Return the vertex input builder for further configuration
}

PipelineBuilder &PipelineBuilder::set_input_assembly(VkPrimitiveTopology topology, VkBool32 primitive_restart) {
    VkPipelineInputAssemblyStateCreateInfo *input_assembly = new VkPipelineInputAssemblyStateCreateInfo; // Allocate memory for input assembly state
    input_assembly->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO; // Initialize the structure type
    input_assembly->pNext = nullptr; // No additional structures
    input_assembly->flags = 0; // No flags
    input_assembly->topology = topology; // Set the primitive topology
    input_assembly->primitiveRestartEnable = primitive_restart; // Set primitive restart enable

    create_info.pInputAssemblyState = input_assembly; // Set the input assembly state in the pipeline create info
    return *this; // Return the current instance for method chaining
}

RasterizerBuilder PipelineBuilder::set_rasterizer() {
    VkPipelineRasterizationStateCreateInfo *rasterizer_info = new VkPipelineRasterizationStateCreateInfo; // Allocate memory for rasterization state
    RasterizerBuilder rasterizer_builder(*rasterizer_info); // Create a new rasterizer builder
    create_info.pRasterizationState = rasterizer_info; // Set the rasterization state in the pipeline create info
    return rasterizer_builder; // Return the rasterizer builder for further configuration
}

DepthStencilBuilder PipelineBuilder::set_depth_stencil() {
    VkPipelineDepthStencilStateCreateInfo *depth_stencil_info = new VkPipelineDepthStencilStateCreateInfo; // Allocate memory for depth stencil state
    DepthStencilBuilder depth_stencil_builder(*depth_stencil_info); // Create a new depth stencil builder
    create_info.pDepthStencilState = depth_stencil_info; // Set the depth stencil state in the pipeline create info
    return depth_stencil_builder; // Return the depth stencil builder for further configuration
}

ColorBlendBuilder PipelineBuilder::set_color_blend() {
    ColorBlendBuilder color_blend_builder(*this); // Create a new color blend builder
    return color_blend_builder; // Return the color blend builder for further configuration
}

DynamicStateBuilder PipelineBuilder::set_dynamic_state() {
    DynamicStateBuilder dynamic_state_builder(*this); // Create a new dynamic state builder
    return dynamic_state_builder; // Return the dynamic state builder for further configuration
}

PipelineBuilder &PipelineBuilder::set_layout(RID layout) {
    const PipelineLayout &pipeline_layout = PipelineLayoutServer::instance().get_pipeline_layout(layout);
    if (pipeline_layout.layout == VK_NULL_HANDLE) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Pipeline layout is not valid!" << std::endl;
        #endif
        return *this; // If the pipeline layout is invalid, do not set it
    }
    create_info.layout = pipeline_layout.layout; // Set the pipeline layout in the create info
    return *this; // Return the current instance for method chaining
}

PipelineBuilder &PipelineBuilder::set_render_pass(RID render_pass) {
    const RenderPass &pass = RenderPassServer::instance().get_render_pass(render_pass);
    if (pass.render_pass == VK_NULL_HANDLE) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Render pass is not valid!" << std::endl;
        #endif
        return *this; // If the render pass is invalid, do not set it
    }
    create_info.renderPass = pass.render_pass; // Set the render pass in the create info
    return *this; // Return the current instance for method chaining
}

PipelineBuilder &PipelineBuilder::set_subpass(uint32_t subpass) {
    create_info.subpass = subpass; // Set the subpass index in the pipeline create info
    return *this; // Return the current instance for method chaining
}

RID PipelineBuilder::build() {
    if (create_info.layout == VK_NULL_HANDLE) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Pipeline layout must be set before building!" << std::endl;
        #endif
        return RID_INVALID; // Return an invalid RID if layout is not set
    }

    if (create_info.renderPass == VK_NULL_HANDLE) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Render pass must be set before building!" << std::endl;
        #endif
        return RID_INVALID; // Return an invalid RID if render pass is not set
    }

    create_info.stageCount = static_cast<uint32_t>(stages.size());
    create_info.pStages = stages.data(); // Set the shader stages in the pipeline create info

    return server.new_pipeline(create_info); // Create a new pipeline and return its RID
}



SimplePipelineBuilder::SimplePipelineBuilder(PipelineServer &server) : server(server) {
    VkPipelineMultisampleStateCreateInfo *multisample_info = new VkPipelineMultisampleStateCreateInfo; // Allocate memory for multisample state

    std::memset(multisample_info, 0, sizeof(VkPipelineMultisampleStateCreateInfo));

    multisample_info->sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO; // Initialize the structure type
    multisample_info->rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; // Default rasterization samples is 1
    multisample_info->sampleShadingEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo *rasterization_state = new VkPipelineRasterizationStateCreateInfo; // Allocate memory for rasterization state

    std::memset(rasterization_state, 0, sizeof(VkPipelineRasterizationStateCreateInfo));

    rasterization_state->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO; // Initialize the structure type
    rasterization_state->pNext = nullptr; // No additional structures
    rasterization_state->flags = 0; // No flags
    rasterization_state->depthClampEnable = VK_FALSE; // Default depth clamp disabled
    rasterization_state->rasterizerDiscardEnable = VK_FALSE; // Default rasterizer discard disabled
    rasterization_state->polygonMode = VK_POLYGON_MODE_FILL; // Default polygon mode is fill
    rasterization_state->cullMode = VK_CULL_MODE_BACK_BIT; // Default cull mode is back face culling
    rasterization_state->frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // Default front face is counter-clockwise
    rasterization_state->depthBiasEnable = VK_FALSE; // Default depth bias disabled
    rasterization_state->depthBiasConstantFactor = 0.0f; // Default depth bias constant factor
    rasterization_state->depthBiasClamp = 0.0f; // Default depth bias clamp
    rasterization_state->depthBiasSlopeFactor = 0.0f; // Default depth bias slope factor

    VkPipelineViewportStateCreateInfo *viewport_state = new VkPipelineViewportStateCreateInfo;

    std::memset(viewport_state, 0, sizeof(VkPipelineViewportStateCreateInfo));

    viewport_state->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state->viewportCount = 1;
    viewport_state->scissorCount = 1;

    VkPipelineColorBlendAttachmentState *color_blend_attachment = new VkPipelineColorBlendAttachmentState; // Allocate memory for color blend attachment state
    
    std::memset(color_blend_attachment, 0, sizeof(VkPipelineColorBlendAttachmentState));
    
    color_blend_attachment->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment->blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo *color_blend_state = new VkPipelineColorBlendStateCreateInfo; // Allocate memory for color blend state

    std::memset(color_blend_state, 0, sizeof(VkPipelineColorBlendStateCreateInfo));

    color_blend_state->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO; // Initialize the structure type
    color_blend_state->logicOpEnable = VK_FALSE; // Default logic operation disabled
    color_blend_state->logicOp = VK_LOGIC_OP_COPY; // Default logic operation
    color_blend_state->attachmentCount = 1; // No color blend attachments defined yet
    color_blend_state->pAttachments = color_blend_attachment; // Pointer to color blend attachments, will

    VkDynamicState *dynamic_states = new VkDynamicState[2]; // Allocate memory for dynamic states, initially empty

    dynamic_states[0] = VK_DYNAMIC_STATE_VIEWPORT; // Add viewport dynamic state
    dynamic_states[1] = VK_DYNAMIC_STATE_SCISSOR; // Add scissor dynamic

    VkPipelineDynamicStateCreateInfo *dynamic_state_info = new VkPipelineDynamicStateCreateInfo; // Allocate memory for dynamic state info

    std::memset(dynamic_state_info, 0, sizeof(VkPipelineDynamicStateCreateInfo));

    dynamic_state_info->sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO; // Initialize the structure type
    dynamic_state_info->pNext = nullptr; // No additional structures
    dynamic_state_info->flags = 0; // No flags
    dynamic_state_info->dynamicStateCount = 2; // Number of dynamic states
    dynamic_state_info->pDynamicStates = dynamic_states; // Pointer to dynamic states


    create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO; // Initialize the structure type
    create_info.pNext = nullptr; // No additional structures
    create_info.flags = 0; // No flags
    create_info.stageCount = 0; // No shader stages defined yet
    create_info.pStages = nullptr; // Pointer to shader stages, will be set later
    create_info.pVertexInputState = nullptr; // Vertex input state, will be set later
    create_info.pInputAssemblyState = nullptr; // Input assembly state, will be set later
    create_info.pViewportState = viewport_state;
    create_info.pRasterizationState = rasterization_state; // Rasterization state, will be set later
    create_info.pColorBlendState = color_blend_state; // Color blend state, will be set later
    create_info.pMultisampleState = multisample_info; // Multisample state, will be set later
    create_info.pDepthStencilState = nullptr; // Depth stencil state, will be set later
    create_info.pDynamicState = dynamic_state_info; // Dynamic state, will be set later
    create_info.layout = VK_NULL_HANDLE; // Pipeline layout, must be set before use
    create_info.renderPass = VK_NULL_HANDLE; // Render pass, must be set before use
    create_info.subpass = 0; // Subpass index, default is 0
}

ShaderBuilder SimplePipelineBuilder::add_shader(RID module, VkShaderStageFlagBits stage) {
    stages.push_back(VkPipelineShaderStageCreateInfo{}); // Add a new shader stage to the stages vector
    VkPipelineShaderStageCreateInfo &stage_info = stages.back(); // Get a reference to the last added shader stage info
    
    stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO; // Initialize the structure type
    stage_info.pNext = nullptr; // No additional structures
    stage_info.flags = 0; // No flags
    stage_info.stage = stage; // Set the shader stage
    stage_info.module = VK_NULL_HANDLE; // Default module, must be set later
    stage_info.pName = "main"; // Default entry point name, can be changed later

    ShaderBuilder shader_builder(stage_info); // Create a new shader builder
    shader_builder.set_stage(stage); // Set the shader stage
    shader_builder.set_module(module); // Set the shader module
    return shader_builder; // Return the shader builder for further configuration
}

VertexInput SimplePipelineBuilder::set_vertex_input() {
    VkPipelineVertexInputStateCreateInfo *vertex_input_state = new VkPipelineVertexInputStateCreateInfo;
    std::memset(vertex_input_state, 0, sizeof(VkPipelineVertexInputStateCreateInfo));
    VertexInput vertex_input(*vertex_input_state); // Create a new vertex input builder
    create_info.pVertexInputState = vertex_input_state;
    return vertex_input; // Return the vertex input builder for further configuration
}

SimplePipelineBuilder &SimplePipelineBuilder::set_input_assembly(VkPrimitiveTopology topology, VkBool32 primitive_restart) {
    VkPipelineInputAssemblyStateCreateInfo *input_assembly = new VkPipelineInputAssemblyStateCreateInfo; // Allocate memory for input assembly state
    std::memset(input_assembly, 0, sizeof(VkPipelineInputAssemblyStateCreateInfo));
    input_assembly->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO; // Initialize the structure type
    input_assembly->pNext = nullptr; // No additional structures
    input_assembly->flags = 0; // No flags
    input_assembly->topology = topology; // Set the primitive topology
    input_assembly->primitiveRestartEnable = primitive_restart; // Set primitive restart enable

    create_info.pInputAssemblyState = input_assembly; // Set the input assembly state in the pipeline create info
    return *this; // Return the current instance for method chaining
}

DepthStencilBuilder SimplePipelineBuilder::set_depth_stencil() {
    VkPipelineDepthStencilStateCreateInfo *depth_stencil_info = new VkPipelineDepthStencilStateCreateInfo; // Allocate memory for depth stencil state
    std::memset(depth_stencil_info, 0, sizeof(VkPipelineDepthStencilStateCreateInfo));
    DepthStencilBuilder depth_stencil_builder(*depth_stencil_info); // Create a new depth stencil builder
    create_info.pDepthStencilState = depth_stencil_info; // Set the depth stencil state in the pipeline create info
    return depth_stencil_builder; // Return the depth stencil builder for further configuration
}

SimplePipelineBuilder &SimplePipelineBuilder::set_layout(RID layout) {
    const PipelineLayout &pipeline_layout = PipelineLayoutServer::instance().get_pipeline_layout(layout);
    if (pipeline_layout.layout == VK_NULL_HANDLE) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Pipeline layout is not valid!" << std::endl;
        #endif
        return *this; // If the pipeline layout is invalid, do not set it
    }
    create_info.layout = pipeline_layout.layout; // Set the pipeline layout in the create info
    return *this; // Return the current instance for method chaining
}

SimplePipelineBuilder &SimplePipelineBuilder::set_render_pass(RID render_pass) {
    const RenderPass &pass = RenderPassServer::instance().get_render_pass(render_pass);
    if (pass.render_pass == VK_NULL_HANDLE) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Render pass is not valid!" << std::endl;
        #endif
        return *this; // If the render pass is invalid, do not set it
    }
    create_info.renderPass = pass.render_pass;
    return *this; // Return the current instance for method chaining
}

RID SimplePipelineBuilder::build() {
    if (create_info.layout == VK_NULL_HANDLE) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Pipeline layout must be set before building!" << std::endl;
        #endif
        return RID_INVALID; // Return an invalid RID if layout is not set
    }

    if (create_info.renderPass == VK_NULL_HANDLE) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Render pass must be set before building!" << std::endl;
        #endif
        return RID_INVALID; // Return an invalid RID if render pass is not set
    }

    VkPipelineShaderStageCreateInfo *stage_info = new VkPipelineShaderStageCreateInfo[stages.size()];

    for (uint32_t i = 0; i < stages.size(); i++) {
        stage_info[i] = stages[i];
    }

    create_info.stageCount = static_cast<uint32_t>(stages.size());
    create_info.pStages = stage_info; // Set the shader stages in the pipeline create info

    return server.new_pipeline(create_info); // Create a new pipeline and return its RID
}



PipelineLayoutServer::PipelineLayoutServer(VkDevice device) : device(device) {
    // Initialize the pipeline layout server with the Vulkan device
}

PipelineLayoutServer::~PipelineLayoutServer() {
    for (auto &layout : pipeline_layouts) {
        #ifdef ALCHEMIST_DEBUG
        std::cout << "Destroying pipeline layout with RID: " << layout.rid << std::endl;
        #endif
        vkDestroyPipelineLayout(device, layout.layout, nullptr); // Destroy each pipeline layout
    }
    pipeline_layouts.clear(); // Clear the vector of pipeline layouts
}

RID PipelineLayoutServer::new_pipeline_layout(const VkPipelineLayoutCreateInfo &create_info) {
    VkPipelineLayout layout;
    if (vkCreatePipelineLayout(device, &create_info, nullptr, &layout) != VK_SUCCESS) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create pipeline layout!" << std::endl;
        #endif
        return RID_INVALID; // Return an invalid RID if creation fails
    }

    PipelineLayout pl;
    pl.layout = layout;
    pl.rid = RIDServer::instance().new_id(RIDServer::PIPELINE_LAYOUT); // Generate a new RID for the pipeline layout

    pipeline_layouts.push_back(std::move(pl)); // Add the new pipeline layout to the vector

    return pipeline_layouts.back().rid; // Return the RID of the newly created pipeline layout
}

RID PipelineLayoutServer::new_pipeline_layout(VkPipelineLayoutCreateInfo &&create_info) {
    VkPipelineLayout layout;
    if (vkCreatePipelineLayout(device, &create_info, nullptr, &layout) != VK_SUCCESS) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create pipeline layout!" << std::endl;
        #endif
        return RID_INVALID; // Return an invalid RID if creation fails
    }

    PipelineLayout pl;
    pl.layout = layout;
    pl.rid = RIDServer::instance().new_id(RIDServer::PIPELINE_LAYOUT); // Generate a new RID for the pipeline layout

    pipeline_layouts.push_back(std::move(pl)); // Add the new pipeline layout to the vector

    return pipeline_layouts.back().rid; // Return the RID of the newly created pipeline layout
}

PipelineLayoutBuilder PipelineLayoutServer::new_pipeline_layout() {
    return PipelineLayoutBuilder(*this); // Create a new pipeline layout builder
} // Create a new pipeline layout builder

const PipelineLayout &PipelineLayoutServer::get_pipeline_layout(RID rid) const {
    if (rid == RID_INVALID) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Invalid RID for pipeline layout!" << std::endl;
        #endif
        throw std::runtime_error("Invalid RID for pipeline layout"); // Throw an error if the RID is invalid
    }

    for (const auto &layout : pipeline_layouts) {
        if (layout.rid == rid) {
            return layout; // Return the pipeline layout if found
        }
    }

    #ifdef ALCHEMIST_DEBUG
    std::cerr << "Pipeline layout with RID " << rid << " not found!" << std::endl;
    #endif
    throw std::runtime_error("Pipeline layout not found"); // Throw an error if the layout is not found
}

PipelineLayoutServer &PipelineLayoutServer::instance() {
    return *__instance; // Return the singleton instance
}

std::unique_ptr<PipelineLayoutServer> PipelineLayoutServer::__instance = nullptr; // Singleton instance of PipelineLayoutServer



PipelineServer::PipelineServer(VkDevice device) : device(device) {
    // Initialize the pipeline server with the Vulkan device
}

PipelineServer::~PipelineServer() {
    for (auto &pipeline : pipelines) {
        #ifdef ALCHEMIST_DEBUG
        std::cout << "Destroying pipeline with RID: " << pipeline.rid << std::endl;
        #endif
        vkDestroyPipeline(device, pipeline.pipeline, nullptr); // Destroy each pipeline
    }
    pipelines.clear(); // Clear the vector of pipelines
}

RID PipelineServer::new_pipeline(const VkGraphicsPipelineCreateInfo &create_info) {
    VkPipeline pipeline;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &create_info, nullptr, &pipeline) != VK_SUCCESS) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create graphics pipeline!" << std::endl;
        #endif
        return RID_INVALID; // Return an invalid RID if creation fails
    }

    Pipeline p;
    p.pipeline = pipeline;
    p.rid = RIDServer::instance().new_id(RIDServer::PIPELINE); // Generate a new RID for the pipeline

    pipelines.push_back(std::move(p)); // Add the new pipeline to the vector

    return pipelines.back().rid; // Return the RID of the newly created pipeline
}

RID PipelineServer::new_pipeline(VkGraphicsPipelineCreateInfo &&create_info) {
    VkPipeline pipeline;
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &create_info, nullptr, &pipeline) != VK_SUCCESS) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create graphics pipeline!" << std::endl;
        #endif
        return RID_INVALID; // Return an invalid RID if creation fails
    }

    Pipeline p;
    p.pipeline = pipeline;
    p.rid = RIDServer::instance().new_id(RIDServer::PIPELINE); // Generate a new RID for the pipeline

    pipelines.push_back(std::move(p)); // Add the new pipeline to the vector

    return pipelines.back().rid; // Return the RID of the newly created pipeline
}

PipelineBuilder PipelineServer::new_pipeline() {
    return PipelineBuilder(*this); // Create a new pipeline builder
} // Create a new pipeline builder

SimplePipelineBuilder PipelineServer::new_simple_pipeline() {
    return SimplePipelineBuilder(*this); // Create a new pipeline builder
} // Create a new pipeline builder

Pipeline &PipelineServer::get_pipeline(RID rid) {
    if (rid == RID_INVALID) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Invalid RID for pipeline!" << std::endl;
        #endif
        return *pipelines.end();
    }

    for (auto &pipeline : pipelines) {
        if (pipeline.rid == rid) {
            return pipeline; // Return the pipeline if found
        }
    }

    #ifdef ALCHEMIST_DEBUG
    std::cerr << "Pipeline with RID " << rid << " not found!" << std::endl;
    #endif
    return *pipelines.end();
}


const Pipeline &PipelineServer::get_pipeline(RID rid) const {
    if (rid == RID_INVALID) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Invalid RID for pipeline!" << std::endl;
        #endif
        return *pipelines.cend();
    }

    for (const auto &pipeline : pipelines) {
        if (pipeline.rid == rid) {
            return pipeline; // Return the pipeline if found
        }
    }

    #ifdef ALCHEMIST_DEBUG
    std::cerr << "Pipeline with RID " << rid << " not found!" << std::endl;
    #endif
    return *pipelines.cend();
}

PipelineServer &PipelineServer::instance() {
    return *__instance; // Return the singleton instance
}

std::unique_ptr<PipelineServer> PipelineServer::__instance = nullptr; // Singleton instance of PipelineServer
