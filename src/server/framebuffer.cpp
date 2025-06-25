
#ifdef ALCHEMIST_DEBUG
#include <iostream>
#endif // ALCHEMIST_DEBUG

#include "server/framebuffer.hpp"
#include "server/image.hpp"
#include "server/render_pass.hpp"

FramebufferBuilder::FramebufferBuilder(FramebufferServer &server, uint32_t width, uint32_t height, uint32_t layers) :
    render_pass(VK_NULL_HANDLE), width(width), height(height), layers(layers), server(server) {}

FramebufferBuilder::FramebufferBuilder(FramebufferServer &server, VkExtent2D extent, uint32_t layers) :
    render_pass(VK_NULL_HANDLE), width(extent.width), height(extent.height), layers(layers), server(server) {}

FramebufferBuilder &FramebufferBuilder::set_render_pass(RID render_pass) {
    const RenderPassServer &render_pass_server = RenderPassServer::instance();
    const RenderPass &rp = render_pass_server.get_render_pass(render_pass);
    if (rp.render_pass == VK_NULL_HANDLE) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Invalid render pass RID: " << render_pass << std::endl;
    #endif
        return *this; // Return without setting if the RID is invalid
    }
    this->render_pass = rp.render_pass;

    return *this;
}

FramebufferBuilder &FramebufferBuilder::add_attachment(VkImageView attachment) {
    attachments.push_back(attachment);
    return *this;
}

FramebufferBuilder &FramebufferBuilder::set_attachment(VkImageView attachment, uint32_t index) {
    attachments[index] = attachment;
    return *this;
}

FramebufferBuilder &FramebufferBuilder::add_attachment(RID attachment) {
    const ImageViewServer &image_view_server = ImageViewServer::instance();
    const ImageView &image_view = image_view_server.get_image_view(attachment);
    if (image_view.view == VK_NULL_HANDLE) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Invalid image view RID: " << attachment << std::endl;
    #endif
        return *this; // Return without adding if the RID is invalid
    }
    attachments.push_back(image_view.view);
    return *this;
}

FramebufferBuilder &FramebufferBuilder::set_attachment(RID attachment, uint32_t index) {
    const ImageViewServer &image_view_server = ImageViewServer::instance();
    const ImageView &image_view = image_view_server.get_image_view(attachment);
    if (image_view.view == VK_NULL_HANDLE) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Invalid image view RID: " << attachment << std::endl;
    #endif
        return *this; // Return without setting if the RID is invalid
    }
    attachments[index] = image_view.view;
    return *this;
}

RID FramebufferBuilder::build() const {
    if (render_pass == VK_NULL_HANDLE) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Render pass must be set before building framebuffer." << std::endl;
    #endif
        return RID_INVALID;
    }
    if (attachments.empty()) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "At least one attachment must be added before building framebuffer." << std::endl;
    #endif
        return RID_INVALID;
    }

    VkFramebufferCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    create_info.renderPass = render_pass;
    create_info.attachmentCount = attachments.size();
    create_info.pAttachments = attachments.data();
    create_info.width = width;
    create_info.height = height;
    create_info.layers = layers;

    return server.new_framebuffer(create_info);
}



FramebufferServer::FramebufferServer(VkDevice device) : device(device) {}

FramebufferServer::~FramebufferServer() {
    for (const Framebuffer &fb : framebuffers) {
        #ifdef ALCHEMIST_DEBUG
        std::cout << "Destroying framebuffer with RID: " << fb.rid << std::endl;
        #endif // ALCHEMIST_DEBUG
        vkDestroyFramebuffer(device, fb.framebuffer, nullptr);
    }
}

RID FramebufferServer::new_framebuffer(const VkFramebufferCreateInfo &create_info) {
    VkFramebuffer framebuffer;
    VkResult result = vkCreateFramebuffer(device, &create_info, nullptr, &framebuffer);
    if (result != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create framebuffer: " << result << std::endl;
    #endif
        return RID_INVALID;
    }

    Framebuffer fb;
    fb.framebuffer = framebuffer;
    fb.rid = RIDServer::instance().new_id(RIDServer::FRAMEBUFFER);
    framebuffers.push_back(fb);

    return fb.rid;
}

RID FramebufferServer::new_framebuffer(VkFramebufferCreateInfo &&create_info) {
    VkFramebuffer framebuffer;
    VkResult result = vkCreateFramebuffer(device, &create_info, nullptr, &framebuffer);
    if (result != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create framebuffer: " << result << std::endl;
    #endif
        return RID_INVALID;
    }

    Framebuffer fb;
    fb.framebuffer = framebuffer;
    fb.rid = RIDServer::instance().new_id(RIDServer::FRAMEBUFFER);
    framebuffers.push_back(fb);

    return fb.rid;
}

FramebufferBuilder FramebufferServer::new_framebuffer(uint32_t width, uint32_t height, uint32_t layers) {
    return FramebufferBuilder(*this, width, height, layers);
}

const Framebuffer &FramebufferServer::get_framebuffer(RID rid) const {
    if (rid >= framebuffers.size() || framebuffers[rid].rid == RID_INVALID) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Invalid framebuffer RID: " << rid << std::endl;
    #endif
        return framebuffers[0]; // Return the first framebuffer or handle this case appropriately
    }
    return framebuffers[rid];
}

FramebufferServer &FramebufferServer::instance() {
    return *__instance;
}

std::unique_ptr<FramebufferServer> FramebufferServer::__instance = nullptr; // Singleton instance of Framebuffer
