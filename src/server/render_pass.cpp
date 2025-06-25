
#ifdef ALCHEMIST_DEBUG
#include <iostream>
#endif // ALCHEMIST_DEBUG

#include "server/render_pass.hpp"
#include "server/framebuffer.hpp"

RenderPassBegin::RenderPassBegin(VkCommandBuffer buffer) :
    cmd_buffer(buffer) {
    begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    begin_info.pNext = nullptr;
    begin_info.renderPass = VK_NULL_HANDLE; // Will be set later
    begin_info.framebuffer = VK_NULL_HANDLE; // Will be set later
    begin_info.renderArea.offset = {0, 0}; // Default offset
    begin_info.renderArea.extent = {0, 0}; // Default size
}

RenderPassBegin &RenderPassBegin::set_render_pass(RID pass) {
    const RenderPassServer &render_pass_server = RenderPassServer::instance();
    const RenderPass &rp = render_pass_server.get_render_pass(pass);
    if (rp.render_pass == VK_NULL_HANDLE) {
#ifdef ALCHEMIST_DEBUG
        std::cerr << "Invalid render pass RID: " << pass << std::endl;
#endif
        return *this; // Return without setting if the RID is invalid
    }

    begin_info.renderPass = rp.render_pass;
    return *this;
}

RenderPassBegin &RenderPassBegin::set_framebuffer(RID fb) {
    const FramebufferServer &framebuffer_server = FramebufferServer::instance();
    const Framebuffer &f = framebuffer_server.get_framebuffer(fb);
    if (f.framebuffer == VK_NULL_HANDLE) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Invalid framebuffer RID: " << fb << std::endl;
        #endif
        return *this; // Return without setting if the RID is invalid
    }
    begin_info.framebuffer = f.framebuffer;
    return *this;
}

RenderPassBegin &RenderPassBegin::set_render_offset(VkOffset2D area) {
    begin_info.renderArea.offset = area;
    return *this;
}

RenderPassBegin &RenderPassBegin::set_render_size(VkExtent2D area) {
    begin_info.renderArea.extent = area;
    return *this;
}

RenderPassBegin &RenderPassBegin::add_clear_color(const color &value) {
    VkClearValue clear_value;
    clear_value.color.float32[0] = value.r;
    clear_value.color.float32[1] = value.g;
    clear_value.color.float32[2] = value.b;
    clear_value.color.float32[3] = value.a;

    clear_values.push_back(clear_value);
    return *this;
}

RenderPassBegin &RenderPassBegin::add_clear_depth(float value) {
    VkClearValue clear_value;
    clear_value.depthStencil.depth = value;
    clear_value.depthStencil.stencil = 0; // Default stencil value

    clear_values.push_back(clear_value);
    return *this;
}

void RenderPassBegin::begin() {
    if (begin_info.renderPass == VK_NULL_HANDLE || begin_info.framebuffer == VK_NULL_HANDLE) {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Render pass or framebuffer not set before beginning render pass!" << std::endl;
        #endif
        return; // Cannot begin render pass without valid render pass and framebuffer
    }

    begin_info.pClearValues = clear_values.data();
    begin_info.clearValueCount = static_cast<uint32_t>(clear_values.size());

    vkCmdBeginRenderPass(cmd_buffer, &begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderPassBegin::end() {
    vkCmdEndRenderPass(cmd_buffer);
}

RenderPassBegin RenderPass::begin(VkCommandBuffer cmd_buffer) const {
    return RenderPassBegin(cmd_buffer).set_render_pass(rid);
}

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
    VkAttachmentReference *depth_stencil_ref;
    
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = color_attachments.size();
    subpass.pColorAttachments = color_refs;
    
    if (depth_stencil_attachment.attachment != VK_ATTACHMENT_UNUSED) {
        depth_stencil_ref = new VkAttachmentReference;
        std::swap(depth_stencil_attachment, *depth_stencil_ref);
        subpass.pDepthStencilAttachment = depth_stencil_ref;
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




RenderingPassBuilder::RenderingPassBuilder(RenderPassServer &server) : server(server) {}

RenderingPassBuilder::RenderingPassBuilder(RenderPassServer &server, 
    uint32_t attachment_capacity, 
    uint32_t subpass_capacity, 
    uint32_t dependency_capacity) : server(server)
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

RID RenderingPassBuilder::build() const {
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

    RID rid = server.new_render_pass(create_info);

    for (const auto &subpass : subpasses) {
        delete[] subpass.pColorAttachments; // Free the dynamically allocated color attachments
        if (subpass.pDepthStencilAttachment) {
            delete subpass.pDepthStencilAttachment; // Free the dynamically allocated depth-stencil attachment
        }
    }

    return rid;
}

RenderPassServer::RenderPassServer(VkDevice device) : device(device) {}

RenderPassServer::~RenderPassServer() {
    for (auto &render_pass : render_passes) {
        #ifdef ALCHEMIST_DEBUG
        std::cout << "Destroying render pass with RID: " << render_pass.rid << std::endl;
        #endif
        vkDestroyRenderPass(device, render_pass.render_pass, nullptr);
    }
}

RID RenderPassServer::new_render_pass(const VkRenderPassCreateInfo &create_info) {
    VkRenderPass render_pass;
    if (vkCreateRenderPass(device, &create_info, nullptr, &render_pass) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create render pass!" << std::endl;
    #endif
        return RID_INVALID; // Return an invalid RID if creation fails
    }

    RenderPass rp;
    rp.render_pass = render_pass;
    rp.rid = RIDServer::instance().new_id(RIDServer::RENDER_PASS);
    
    render_passes.push_back(std::move(rp));
    
    return render_passes.back().rid;
}

RID RenderPassServer::new_render_pass(VkRenderPassCreateInfo &&create_info) {
    VkRenderPass render_pass;
    if (vkCreateRenderPass(device, &create_info, nullptr, &render_pass) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create render pass!" << std::endl;
    #endif
        return RID_INVALID; // Return an invalid RID if creation fails
    }

    RenderPass rp;
    rp.render_pass = render_pass;
    rp.rid = RIDServer::instance().new_id(RIDServer::RENDER_PASS);
    
    render_passes.push_back(std::move(rp));
    
    return render_passes.back().rid;
}

RenderingPassBuilder RenderPassServer::new_render_pass() {
    return RenderingPassBuilder(*this); // Default constructor with no capacities
}

RenderingPassBuilder RenderPassServer::new_render_pass(
    uint32_t attachment_capacity, 
    uint32_t subpass_capacity, 
    uint32_t dependency_capacity
) {
    return RenderingPassBuilder(*this, attachment_capacity, subpass_capacity, dependency_capacity); // Default capacities for attachments, subpasses, and dependencies
}

const RenderPass &RenderPassServer::get_render_pass(RID rid) const {
    if (rid == RID_INVALID) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Invalid RID provided for render pass retrieval!" << std::endl;
    #endif
        throw std::runtime_error("Invalid RID provided for render pass retrieval.");
    }

    for (const auto &render_pass : render_passes) {
        if (render_pass.rid == rid) {
            return render_pass; // Return the found render pass
        }
    }

    #ifdef ALCHEMIST_DEBUG
    std::cerr << "Render pass with RID " << rid << " not found!" << std::endl;
    #endif
    return render_passes.back(); // Return the last render pass as a fallback (could be invalid)
}

RenderPassServer &RenderPassServer::instance() {
    return *__instance;
}

std::unique_ptr<RenderPassServer> RenderPassServer::__instance = nullptr; // Singleton instance of BufferServer

RID default_render_pass(VkFormat image_format, VkFormat depth_format) {
    auto builder = RenderPassServer::instance().new_render_pass(2, 1, 1);
    
    builder.new_attachment()
        .set_format(image_format)
        .set_load_op(VK_ATTACHMENT_LOAD_OP_CLEAR)
        .set_store_op(VK_ATTACHMENT_STORE_OP_STORE)
        .set_initial_layout(VK_IMAGE_LAYOUT_UNDEFINED)
    #ifdef ALCHEMIST_DEBUG
        .set_final_layout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) // For rendering
    #else
        .set_final_layout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) // For final presentation
    #endif // ALCHEMIST_DEBUG
        .build();
    
    builder.new_attachment()
        .set_format(depth_format)
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

    return builder.build();
}

#ifdef ALCHEMIST_DEBUG
RID imgui_render_pass(VkFormat image_format) {
    auto builder = RenderPassServer::instance().new_render_pass(1, 1, 1);

    builder.new_attachment()
        .set_format(image_format)
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

    return builder.build();
}
#endif // ALCHEMIST_DEBUG