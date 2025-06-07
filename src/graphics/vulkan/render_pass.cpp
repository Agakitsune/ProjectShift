
#include <cstdlib>
#include <utility>

#include "graphics/vulkan/render_pass.hpp"

AttachmentDescription::AttachmentDescription(AttachmentType type,
                                             VkFormat format,
                                             VkImageLayout initial_layout,
                                             VkImageLayout final_layout,
                                             VkImageLayout reference)
    : type(type), format(format), samples(VK_SAMPLE_COUNT_1_BIT),
      loadOp(VK_ATTACHMENT_LOAD_OP_CLEAR),
      storeOp(VK_ATTACHMENT_STORE_OP_STORE),
      stencilLoadOp(VK_ATTACHMENT_LOAD_OP_DONT_CARE),
      stencilStoreOp(VK_ATTACHMENT_STORE_OP_DONT_CARE),
      initialLayout(VK_IMAGE_LAYOUT_UNDEFINED),
      finalLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR), referenceLayout(reference),
      flags(0) // Default flags, can be set later
{}

SubpassDescription::SubpassDescription(VkPipelineBindPoint bind_point)
    : bindPoint(bind_point), inputAttachments(nullptr),
      colorAttachments(nullptr), resolveAttachments(UINT32_MAX),
      depthStencilAttachment(UINT32_MAX), colorAttachmentCount(0),
      inputAttachmentCount(0) {}

SubpassDescription &
SubpassDescription::add_input_attachment(uint32_t input_attachment) {
    inputAttachmentCount++;
    inputAttachments = (uint32_t *)realloc(
        inputAttachments, inputAttachmentCount * sizeof(uint32_t));
    if (!inputAttachments) {
        // Handle memory allocation failure
        return *this;
    }
    inputAttachments[inputAttachmentCount - 1] = input_attachment;

    return *this;
}

SubpassDescription &
SubpassDescription::add_color_attachment(uint32_t color_attachment) {
    colorAttachmentCount++;
    colorAttachments = (uint32_t *)realloc(
        colorAttachments, colorAttachmentCount * sizeof(uint32_t));
    if (!colorAttachments) {
        // Handle memory allocation failure
        return *this;
    }
    colorAttachments[colorAttachmentCount - 1] = color_attachment;

    return *this;
}

SubpassDescription &SubpassDescription::set_depth_stencil_attachment(
    uint32_t depth_stencil_attachment) {
    depthStencilAttachment = depth_stencil_attachment;

    return *this;
}

SubpassDescription &
SubpassDescription::set_resolve_attachment(uint32_t resolve_attachment) {
    resolveAttachments = resolve_attachment;

    return *this;
}

RenderPassInfo &
RenderPassInfo::add_attachment(const AttachmentDescription &description) {
    attachment_count++;
    AttachmentDescription *new_attachments = (AttachmentDescription *)realloc(
        attachments, attachment_count * sizeof(AttachmentDescription));
    if (!new_attachments) {
        // Handle memory allocation failure
        return *this;
    }
    attachments = new_attachments;
    attachments[attachment_count - 1] = description;

    return *this;
}

RenderPassInfo &
RenderPassInfo::add_attachment(AttachmentDescription &&description) {
    attachment_count++;
    AttachmentDescription *new_attachments = (AttachmentDescription *)realloc(
        attachments, attachment_count * sizeof(AttachmentDescription));
    if (!new_attachments) {
        // Handle memory allocation failure
        return *this;
    }
    attachments = new_attachments;
    attachments[attachment_count - 1] = std::move(description);

    return *this;
}

RenderPassInfo &
RenderPassInfo::add_subpass_description(const SubpassDescription &description) {
    description_count++;
    SubpassDescription *new_descriptions = (SubpassDescription *)realloc(
        descriptions, description_count * sizeof(SubpassDescription));
    if (!new_descriptions) {
        // Handle memory allocation failure
        return *this;
    }
    descriptions = new_descriptions;
    descriptions[description_count - 1] = description;

    return *this;
}

RenderPassInfo &
RenderPassInfo::add_subpass_description(SubpassDescription &&description) {
    description_count++;
    SubpassDescription *new_descriptions = (SubpassDescription *)realloc(
        descriptions, description_count * sizeof(SubpassDescription));
    if (!new_descriptions) {
        // Handle memory allocation failure
        return *this;
    }
    descriptions = new_descriptions;
    descriptions[description_count - 1] = std::move(description);

    return *this;
}

RenderPassInfo &
RenderPassInfo::add_subpass_dependecy(const VkSubpassDependency &dependency) {
    dependency_count++;
    VkSubpassDependency *new_dependencies = (VkSubpassDependency *)realloc(
        dependencies, dependency_count * sizeof(VkSubpassDependency));
    if (!new_dependencies) {
        // Handle memory allocation failure
        return *this;
    }
    dependencies = new_dependencies;
    dependencies[dependency_count - 1] = dependency;

    return *this;
}

RenderPassInfo &
RenderPassInfo::add_subpass_dependecy(VkSubpassDependency &&dependency) {
    dependency_count++;
    VkSubpassDependency *new_dependencies = (VkSubpassDependency *)realloc(
        dependencies, dependency_count * sizeof(VkSubpassDependency));
    if (!new_dependencies) {
        // Handle memory allocation failure
        return *this;
    }
    dependencies = new_dependencies;
    dependencies[dependency_count - 1] = std::move(dependency);

    return *this;
}

void populate_subpass_description(VkSubpassDescription &vk_subpass,
                                  const SubpassDescription &subpass,
                                  const AttachmentDescription *attachments) {
    VkAttachmentReference *input_attachments = nullptr;
    VkAttachmentReference *color_attachments = nullptr;
    VkAttachmentReference *resolve_attachment = nullptr;
    VkAttachmentReference *depth_stencil_attachment = nullptr;

    vk_subpass.flags = 0; // Set flags if needed

    vk_subpass.pipelineBindPoint = subpass.bindPoint;
    vk_subpass.inputAttachmentCount = subpass.inputAttachmentCount;
    vk_subpass.colorAttachmentCount = subpass.colorAttachmentCount;

    if (subpass.inputAttachmentCount > 0) {
        input_attachments =
            new VkAttachmentReference[subpass.inputAttachmentCount];
        for (uint32_t i = 0; i < subpass.inputAttachmentCount; i++) {
            input_attachments[i].attachment = subpass.inputAttachments[i];
            input_attachments[i].layout =
                attachments[subpass.inputAttachments[i]].referenceLayout;
        }
        vk_subpass.pInputAttachments = input_attachments;
    } else {
        vk_subpass.pInputAttachments = nullptr;
    }
    if (subpass.colorAttachmentCount > 0) {
        color_attachments =
            new VkAttachmentReference[subpass.colorAttachmentCount];
        for (uint32_t i = 0; i < subpass.colorAttachmentCount; i++) {
            color_attachments[i].attachment = subpass.colorAttachments[i];
            color_attachments[i].layout =
                attachments[subpass.colorAttachments[i]].referenceLayout;
        }
        vk_subpass.pColorAttachments = color_attachments;
    } else {
        vk_subpass.pColorAttachments = nullptr;
    }
    if (subpass.resolveAttachments != UINT32_MAX) {
        resolve_attachment = new VkAttachmentReference();
        resolve_attachment->attachment = subpass.resolveAttachments;
        resolve_attachment->layout =
            attachments[subpass.resolveAttachments].referenceLayout;
        vk_subpass.pResolveAttachments = resolve_attachment;
    } else {
        vk_subpass.pResolveAttachments = nullptr;
    }
    if (subpass.depthStencilAttachment != UINT32_MAX) {
        depth_stencil_attachment = new VkAttachmentReference();
        depth_stencil_attachment->attachment = subpass.depthStencilAttachment;
        depth_stencil_attachment->layout =
            attachments[subpass.depthStencilAttachment].referenceLayout;
        vk_subpass.pDepthStencilAttachment = depth_stencil_attachment;
    } else {
        vk_subpass.pDepthStencilAttachment = nullptr;
    }
}

VkRenderPass create_render_pass(VkDevice device, const RenderPassInfo &info) {
    VkAttachmentReference depth_stencil_reference{};
    VkRenderPassCreateInfo render_pass_info = {};
    VkAttachmentDescription *vk_attachments = nullptr;
    VkSubpassDescription *vk_subpasses = nullptr;
    // VkSubpassDependency *vk_dependencies = nullptr;
    VkRenderPass render_pass;

    vk_attachments = new VkAttachmentDescription[info.attachment_count];
    vk_subpasses = new VkSubpassDescription[info.description_count];
    // vk_dependencies = new VkSubpassDependency[info.dependency_count];

    for (uint32_t i = 0; i < info.attachment_count; i++) {
        vk_attachments[i].flags = info.attachments[i].flags;
        vk_attachments[i].format = info.attachments[i].format;
        vk_attachments[i].samples = info.attachments[i].samples;
        vk_attachments[i].loadOp = info.attachments[i].loadOp;
        vk_attachments[i].storeOp = info.attachments[i].storeOp;
        vk_attachments[i].stencilLoadOp = info.attachments[i].stencilLoadOp;
        vk_attachments[i].stencilStoreOp = info.attachments[i].stencilStoreOp;
        vk_attachments[i].initialLayout = info.attachments[i].initialLayout;
        vk_attachments[i].finalLayout = info.attachments[i].finalLayout;
    }

    for (uint32_t i = 0; i < info.description_count; i++) {
        vk_subpasses[i].flags = 0; // Set flags if needed
        populate_subpass_description(vk_subpasses[i], info.descriptions[i],
                                     info.attachments);
        vk_subpasses[i].pPreserveAttachments =
            nullptr; // No preserve attachments in this example
        vk_subpasses[i].preserveAttachmentCount =
            0; // No preserve attachments in this example
    }

    // for (uint32_t i = 0; i < info.dependency_count; i++) {
    //     vk_dependencies[i] = [i];
    // }

    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = info.attachment_count;
    render_pass_info.subpassCount = info.description_count;
    render_pass_info.dependencyCount = info.dependency_count;

    render_pass_info.pAttachments = vk_attachments;
    render_pass_info.pSubpasses = vk_subpasses;
    render_pass_info.pDependencies = info.dependencies;

    // Define subpasses and dependencies if needed
    // For simplicity, we are not defining any subpasses or dependencies here

    if (vkCreateRenderPass(device, &render_pass_info, nullptr, &render_pass) !=
        VK_SUCCESS) {
        return VK_NULL_HANDLE; // Return null handle on failure
    }

    return render_pass;
}
