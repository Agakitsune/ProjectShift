
#include "graphics/rendering_pass.hpp"

AttachmentBuilder::AttachmentBuilder(RenderingPassBuilder &pass)
    : rendering_pass(pass), format(VK_FORMAT_UNDEFINED),
        load_op(VK_ATTACHMENT_LOAD_OP_CLEAR), store_op(VK_ATTACHMENT_STORE_OP_STORE),
        initial_layout(VK_IMAGE_LAYOUT_UNDEFINED), final_layout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {}

AttachmentBuilder &AttachmentBuilder::set_format(VkFormat fmt) {
    format = fmt;
    return *this;
}

AttachmentBuilder &AttachmentBuilder::set_samples(VkSampleCountFlagBits sample_count) {
    samples = sample_count;
    return *this;
}

AttachmentBuilder &AttachmentBuilder::set_load_op(VkAttachmentLoadOp op) {
    load_op = op;
    return *this;
}

AttachmentBuilder &AttachmentBuilder::set_store_op(VkAttachmentStoreOp op) {
    store_op = op;
    return *this;
}

AttachmentBuilder &AttachmentBuilder::set_initial_layout(VkImageLayout layout) {
    initial_layout = layout;
    return *this;
}

AttachmentBuilder &AttachmentBuilder::set_final_layout(VkImageLayout layout) {
    final_layout = layout;
    return *this;
}

RenderingPassBuilder &AttachmentBuilder::build() const {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = format;
    colorAttachment.samples = samples;
    colorAttachment.loadOp = load_op;
    colorAttachment.storeOp = store_op;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = initial_layout;
    colorAttachment.finalLayout = final_layout;

    rendering_pass.attachments.emplace_back(std::move(colorAttachment));

    return rendering_pass;
}



SubpassBuilder::SubpassBuilder(RenderingPassBuilder &pass)
    : rendering_pass(pass) {}
    

SubpassBuilder &SubpassBuilder::set_bind_point(VkPipelineBindPoint point) {
    bind_point = point;
    return *this;
}

SubpassBuilder &SubpassBuilder::add_color_attachment(uint32_t attachment, VkImageLayout layout) {
    VkAttachmentReference ref{};
    ref.attachment = attachment;
    ref.layout = layout;
    color_attachments.emplace_back(std::move(ref));
    return *this;
}

SubpassBuilder &SubpassBuilder::set_depth_stencil_attachment(uint32_t attachment, VkImageLayout layout) {
    depth_stencil_attachment.attachment = attachment;
    depth_stencil_attachment.layout = layout;
    return *this;
}

RenderingPassBuilder &SubpassBuilder::build() {
    VkSubpassDescription subpass{};
    VkAttachmentReference *color_refs = new VkAttachmentReference[color_attachments.size()];
    
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = color_attachments.size();
    subpass.pColorAttachments = color_refs;
    
    if (depth_stencil_attachment.attachment != VK_ATTACHMENT_UNUSED) {
        subpass.pDepthStencilAttachment = &depth_stencil_attachment;
    } else {
        subpass.pDepthStencilAttachment = nullptr;
    }

    for (size_t i = 0; i < color_attachments.size(); ++i) {
        std::swap(color_refs[i], color_attachments[i]);
    }

    rendering_pass.subpasses.emplace_back(std::move(subpass));
    
    return rendering_pass;
}



SubpassDependecyBuilder::SubpassDependecyBuilder(RenderingPassBuilder &pass)
    : rendering_pass(pass), src_subpass(VK_SUBPASS_EXTERNAL), dst_subpass(0),
        src_stage_mask(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT),
        dst_stage_mask(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT),
        src_access_mask(0), dst_access_mask(VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT) {}
    
SubpassDependecyBuilder &SubpassDependecyBuilder::set_src_subpass(uint32_t subpass) {
    src_subpass = subpass;
    return *this;
}
SubpassDependecyBuilder &SubpassDependecyBuilder::set_dst_subpass(uint32_t subpass) {
    dst_subpass = subpass;
    return *this;
}
SubpassDependecyBuilder &SubpassDependecyBuilder::set_src_stage_mask(VkPipelineStageFlags mask) {
    src_stage_mask = mask;
    return *this;
}
SubpassDependecyBuilder &SubpassDependecyBuilder::set_dst_stage_mask(VkPipelineStageFlags mask) {
    dst_stage_mask = mask;
    return *this;
}
SubpassDependecyBuilder &SubpassDependecyBuilder::set_src_access_mask(VkAccessFlags mask) {
    src_access_mask = mask;
    return *this;
}
SubpassDependecyBuilder &SubpassDependecyBuilder::set_dst_access_mask(VkAccessFlags mask) {
    dst_access_mask = mask;
    return *this;
}

RenderingPassBuilder &SubpassDependecyBuilder::build() const {
    VkSubpassDependency dependency{};
    dependency.srcSubpass = src_subpass;
    dependency.dstSubpass = dst_subpass;
    dependency.srcStageMask = src_stage_mask;
    dependency.dstStageMask = dst_stage_mask;
    dependency.srcAccessMask = src_access_mask;
    dependency.dstAccessMask = dst_access_mask;
    dependency.dependencyFlags = dependency_flags;

    rendering_pass.dependencies.emplace_back(std::move(dependency));
    
    return rendering_pass;
}



RenderingPassBuilder::RenderingPassBuilder(uint32_t attachment_capacity, 
                        uint32_t subpass_capacity, 
                        uint32_t dependency_capacity)
{
    attachments.reserve(attachment_capacity);
    subpasses.reserve(subpass_capacity);
    dependencies.reserve(dependency_capacity);
}

AttachmentBuilder RenderingPassBuilder::new_attachment() {
    return AttachmentBuilder(*this);
}

SubpassBuilder RenderingPassBuilder::new_subpass() {
    return SubpassBuilder(*this);
}

SubpassDependecyBuilder RenderingPassBuilder::new_dependency() {
    return SubpassDependecyBuilder(*this);
}

VkRenderPass RenderingPassBuilder::build(VkDevice device) const {
    VkRenderPassCreateInfo create_info{};

    create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    create_info.attachmentCount = attachments.size();
    create_info.pAttachments = attachments.data();
    create_info.subpassCount = subpasses.size();
    create_info.pSubpasses = subpasses.data();
    create_info.dependencyCount = dependencies.size();
    create_info.pDependencies = dependencies.data();

    #ifdef ALCHEMIST_DEBUG
    std::cout << "Creating render pass with " << attachments.size() 
              << " attachments, " << subpasses.size() 
              << " subpasses, and " << dependencies.size() 
              << " dependencies." << std::endl;
    #endif // ALCHEMIST_DEBUG

    VkRenderPass render_pass;
    if (vkCreateRenderPass(device, &create_info, nullptr, &render_pass) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create render pass!" << std::endl;
    #endif
        return VK_NULL_HANDLE;
    }

    for (const auto &subpass : subpasses) {
        delete[] subpass.pColorAttachments; // Free the dynamically allocated color attachments
    }

    return render_pass;
}

VkRenderPass default_render_pass(const RenderingDevice &device) {
    RenderingPassBuilder builder(2, 1, 1);
    
    builder.new_attachment()
        .set_format(device.surface_format.format)
        .set_load_op(VK_ATTACHMENT_LOAD_OP_CLEAR)
        .set_store_op(VK_ATTACHMENT_STORE_OP_STORE)
        .set_initial_layout(VK_IMAGE_LAYOUT_UNDEFINED)
    #ifdef ALCHEMIST_DEBUG
        .set_final_layout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) // For imgui rendering
    #else
        .set_final_layout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) // For final presentation
    #endif // ALCHEMIST_DEBUG
        .build();
    
    builder.new_attachment()
        .set_format(device.depth_format)
        .set_samples(VK_SAMPLE_COUNT_1_BIT)
        .set_load_op(VK_ATTACHMENT_LOAD_OP_CLEAR)
        .set_store_op(VK_ATTACHMENT_STORE_OP_DONT_CARE)
        .set_initial_layout(VK_IMAGE_LAYOUT_UNDEFINED)
        .set_final_layout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        .build();

    builder.new_subpass()
        .add_color_attachment(0)
        .set_depth_stencil_attachment(1)
        .build();

    builder.new_dependency()
        .set_src_subpass(VK_SUBPASS_EXTERNAL)
        .set_dst_subpass(0)
    #ifdef ALCHEMIST_DEBUG
        .set_src_stage_mask(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT)
        .set_src_access_mask(0)
    #else
        .set_src_stage_mask(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT)
        .set_src_access_mask(VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)
    #endif // ALCHEMIST_DEBUG
        
        .set_dst_stage_mask(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT)
        .set_dst_access_mask(VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)
        .build();

    return builder.build(device.device);
}

#ifdef ALCHEMIST_DEBUG
VkRenderPass imgui_render_pass(const RenderingDevice &device) {
    RenderingPassBuilder builder(1, 1, 1);

    builder.new_attachment()
        .set_format(device.surface_format.format)
        .set_load_op(VK_ATTACHMENT_LOAD_OP_DONT_CARE)
        .set_store_op(VK_ATTACHMENT_STORE_OP_STORE)
        .set_initial_layout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        .set_final_layout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) // For final presentation
        .build();

    builder.new_subpass()
        .add_color_attachment(0)
        .build();

    builder.new_dependency()
        .set_src_subpass(VK_SUBPASS_EXTERNAL)
        .set_dst_subpass(0)
        .set_src_stage_mask(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
        .set_dst_stage_mask(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
        .set_src_access_mask(0)
        .set_dst_access_mask(VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)
        .build();

    return builder.build(device.device);
}
#endif // ALCHEMIST_DEBUG