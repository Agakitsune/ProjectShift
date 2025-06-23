
#include "vulkan/render.hpp"

#include "server/pipeline.hpp"
#include "server/descriptor.hpp"

void bind_pipeline(VkCommandBuffer cmd_buffer, RID pipeline, VkPipelineBindPoint bind_point) {
    const PipelineServer &pipeline_server = PipelineServer::instance();
    const Pipeline &pipe = pipeline_server.get_pipeline(pipeline);
    if (pipe.pipeline == VK_NULL_HANDLE) {
#ifdef ALCHEMIST_DEBUG
        std::cerr << "Invalid pipeline RID: " << pipeline << std::endl;
#endif
        return; // Return without binding if the RID is invalid
    }

    vkCmdBindPipeline(cmd_buffer, bind_point, pipe.pipeline); // Bind the pipeline to the command buffer
}

void viewport(VkCommandBuffer cmd_buffer, VkExtent2D extent) {
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(cmd_buffer, 0, 1, &viewport); // Set the viewport for the command buffer
}

void scissor(VkCommandBuffer cmd_buffer, VkRect2D rect) {
    VkRect2D scissor = {};
    scissor.offset = rect.offset; // Set the offset for the scissor rectangle
    scissor.extent = rect.extent; // Set the extent for the scissor rectangle

    vkCmdSetScissor(cmd_buffer, 0, 1, &scissor); // Set the scissor rectangle for the command buffer
}

void bind_descriptor_sets(VkCommandBuffer cmd_buffer, RID pipeline_layout, RID descriptor_set, VkPipelineBindPoint bind_point, uint32_t *offset) {
    const PipelineLayoutServer &pipeline_server = PipelineLayoutServer::instance();
    const PipelineLayout &layout = pipeline_server.get_pipeline_layout(pipeline_layout);
    if (layout.layout == VK_NULL_HANDLE) {
#ifdef ALCHEMIST_DEBUG
        std::cerr << "Invalid pipeline layout RID: " << pipeline_layout << std::endl;
#endif
        return; // Return without binding if the RID is invalid
    }

    const DescriptorServer &descriptor_set_server = DescriptorServer::instance();
    const Descriptor &set = descriptor_set_server.get_descriptor(descriptor_set);

    if (set.descriptor_set == VK_NULL_HANDLE) {
#ifdef ALCHEMIST_DEBUG
        std::cerr << "Invalid descriptor set RID: " << descriptor_set << std::endl;
#endif
        return; // Return without binding if the RID is invalid
    }

    vkCmdBindDescriptorSets(cmd_buffer, bind_point, layout.layout, 0, 1, &set.descriptor_set, 1, offset); // Bind the descriptor sets to the command buffer
}
