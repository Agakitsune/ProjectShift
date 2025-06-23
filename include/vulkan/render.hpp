
#ifndef ALCHEMIST_VULKAN_RENDER_HPP
#define ALCHEMIST_VULKAN_RENDER_HPP

#include <vulkan/vulkan.h>

#include "server/rid.hpp"

void bind_pipeline(VkCommandBuffer cmd_buffer, RID pipeline, VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS);
void viewport(VkCommandBuffer cmd_buffer, VkExtent2D extent);
void scissor(VkCommandBuffer cmd_buffer, VkRect2D rect);
void bind_descriptor_sets(VkCommandBuffer cmd_buffer, RID pipeline_layout, RID descriptor_set, VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS, uint32_t *offset = nullptr);

#endif // ALCHEMIST_VULKAN_RENDER_HPP
