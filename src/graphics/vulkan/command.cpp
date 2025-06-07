
#include "graphics/vulkan/command.hpp"

RenderPassBeginInfo::RenderPassBeginInfo(const color &clear_color,
                                         float clear_depth,
                                         uint32_t clear_stencil)
    : clear_color(clear_color), clear_depth(clear_depth),
      clear_stencil(clear_stencil) {
    // Default constructor initializes the clear color, depth, and stencil
    // values
}

RenderPassBeginInfo &RenderPassBeginInfo::set_clear_flags(ClearFlags flags) {
    this->flag = flags;
    return *this;
}

RenderPassBeginInfo &
RenderPassBeginInfo::set_render_pass(VkRenderPass render_pass) {
    this->render_pass = render_pass;
    return *this;
}

RenderPassBeginInfo &
RenderPassBeginInfo::set_framebuffer(VkFramebuffer framebuffer) {
    this->framebuffer = framebuffer;
    return *this;
}

RenderPassBeginInfo &RenderPassBeginInfo::set_clear_color(const color &color) {
    this->clear_color = color;
    return *this;
}

RenderPassBeginInfo &RenderPassBeginInfo::set_clear_depth(float depth) {
    this->clear_depth = depth;
    return *this;
}

RenderPassBeginInfo &RenderPassBeginInfo::set_clear_stencil(uint32_t stencil) {
    this->clear_stencil = stencil;
    return *this;
}

RenderPassBeginInfo &RenderPassBeginInfo::set_offset(uint32_t offset_x,
                                                     uint32_t offset_y) {
    this->offset.x = offset_x;
    this->offset.y = offset_y;
    return *this;
}

RenderPassBeginInfo &RenderPassBeginInfo::set_extent(uint32_t extent_x,
                                                     uint32_t extent_y) {
    this->extent.width = extent_x;
    this->extent.height = extent_y;
    return *this;
}

RenderPassBeginInfo &RenderPassBeginInfo::set_extent(VkExtent2D extent) {
    this->extent = extent;
    return *this;
}

BindVertexBuffers::BindVertexBuffers(uint32_t initial_capacity)
    : vertex_buffers(initial_capacity) {
    // Constructor initializes the dual vector with the specified initial
    // capacity
}

BindVertexBuffers &BindVertexBuffers::add_buffer(VkBuffer buffer,
                                                 VkDeviceSize offset) {
    vertex_buffers.push(buffer, offset);
    return *this;
}

BindVertexBuffers &
BindVertexBuffers::set_first_binding(uint32_t first_binding) {
    this->first_binding = first_binding;
    return *this;
}

BindDescriptorSets::BindDescriptorSets(VkPipelineLayout pipeline_layout,
                                       uint32_t first_set,
                                       VkPipelineBindPoint bind_point)
    : pipeline_layout(pipeline_layout), first_set(first_set),
      bind_point(bind_point) {
    // Constructor initializes the bind point, pipeline layout, and first set
    // index
}

BindDescriptorSets &BindDescriptorSets::add_set(VkDescriptorSet set) {
    sets.push(set);
    return *this;
}

BindDescriptorSets &
BindDescriptorSets::add_dynamic_offset(uint32_t dynamic_offset) {
    dynamic_offsets.push(dynamic_offset);
    return *this;
}

BindDescriptorSets &BindDescriptorSets::set_first_set(uint32_t first_set) {
    this->first_set = first_set;
    return *this;
}

BindDescriptorSets &
BindDescriptorSets::set_pipeline_layout(VkPipelineLayout pipeline_layout) {
    this->pipeline_layout = pipeline_layout;
    return *this;
}

BindDescriptorSets &
BindDescriptorSets::set_bind_point(VkPipelineBindPoint bind_point) {
    this->bind_point = bind_point;
    return *this;
}

void begin_render_pass(VkCommandBuffer command_buffer,
                       const RenderPassBeginInfo &begin_info) {
    VkRenderPassBeginInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = begin_info.render_pass;
    render_pass_info.framebuffer = begin_info.framebuffer;
    render_pass_info.renderArea.offset = begin_info.offset;
    render_pass_info.renderArea.extent = begin_info.extent;

    VkClearValue clear_values[2];
    uint32_t clear_value_count = 0;

    if (begin_info.flag & CLEAR_COLOR) {
        clear_values[clear_value_count].color = {
            begin_info.clear_color.r, begin_info.clear_color.g,
            begin_info.clear_color.b, begin_info.clear_color.a};
        clear_value_count++;
    }
    if (begin_info.flag & CLEAR_DEPTH_STENCIL) {
        clear_values[clear_value_count].depthStencil = {
            begin_info.clear_depth, begin_info.clear_stencil};
        clear_value_count++;
    }
    render_pass_info.clearValueCount = clear_value_count;
    render_pass_info.pClearValues = clear_values;

    vkCmdBeginRenderPass(command_buffer, &render_pass_info,
                         VK_SUBPASS_CONTENTS_INLINE);
}

void end_render_pass(VkCommandBuffer command_buffer) {
    vkCmdEndRenderPass(command_buffer);
}

void bind_pipeline(VkCommandBuffer command_buffer, VkPipeline pipeline,
                   VkPipelineBindPoint bind_point) {
    vkCmdBindPipeline(command_buffer, bind_point, pipeline);
}

void set_viewport(VkCommandBuffer command_buffer, const Viewport &viewport) {
    VkViewport vk_viewport = {};
    vk_viewport.x = viewport.offset.x;
    vk_viewport.y = viewport.offset.y;
    vk_viewport.width = viewport.extent.x;
    vk_viewport.height = viewport.extent.y;
    vk_viewport.minDepth = viewport.min_depth;
    vk_viewport.maxDepth = viewport.max_depth;

    vkCmdSetViewport(command_buffer, 0, 1, &vk_viewport);
}

void set_scissor(VkCommandBuffer command_buffer, const Scissor &scissor) {
    VkRect2D vk_scissor = {};
    vk_scissor.offset.x = scissor.offset_x;
    vk_scissor.offset.y = scissor.offset_y;
    vk_scissor.extent.width = scissor.extent_x;
    vk_scissor.extent.height = scissor.extent_y;

    vkCmdSetScissor(command_buffer, 0, 1, &vk_scissor);
}

void bind_vertex_buffers(VkCommandBuffer command_buffer,
                         const BindVertexBuffers &bind_buffers) {
    VkBuffer *buffers = bind_buffers.vertex_buffers.data1;
    VkDeviceSize *offsets = bind_buffers.vertex_buffers.data2;

    vkCmdBindVertexBuffers(command_buffer, bind_buffers.first_binding,
                           bind_buffers.vertex_buffers.size, buffers, offsets);
}

void bind_index_buffer(VkCommandBuffer command_buffer, VkBuffer buffer,
                       VkDeviceSize offset, VkIndexType index_type) {
    vkCmdBindIndexBuffer(command_buffer, buffer, offset, index_type);
}

void bind_descriptor_sets(VkCommandBuffer command_buffer,
                          const BindDescriptorSets &bind_sets) {
    vkCmdBindDescriptorSets(
        command_buffer, bind_sets.bind_point, bind_sets.pipeline_layout,
        bind_sets.first_set, bind_sets.sets.size, bind_sets.sets.data,
        bind_sets.dynamic_offsets.size, bind_sets.dynamic_offsets.data);
}

void draw(VkCommandBuffer command_buffer, uint32_t vertex_count,
          uint32_t instance_count, uint32_t first_vertex,
          uint32_t first_instance) {
    vkCmdDraw(command_buffer, vertex_count, instance_count, first_vertex,
              first_instance);
}

void draw_indexed(VkCommandBuffer command_buffer, uint32_t index_count,
                  uint32_t instance_count, uint32_t first_index,
                  int32_t vertex_offset, uint32_t first_instance) {
    vkCmdDrawIndexed(command_buffer, index_count, instance_count, first_index,
                     vertex_offset, first_instance);
}

void copy_buffer(VkCommandBuffer command_buffer, VkBuffer src_buffer,
                 VkBuffer dst_buffer, const VkBufferCopy &region) {
    vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &region);
}
