
#ifndef ALCHEMIST_GRAPHICS_VULKAN_SHADER_H
#define ALCHEMIST_GRAPHICS_VULKAN_SHADER_H

#include <vulkan/vulkan.h>

VkShaderModule create_shader_module(VkDevice device, const char *path);

struct ShaderModule {
    const char *entry;
    VkShaderModule module;
    VkShaderStageFlagBits stage;

    ShaderModule() = default;
    ShaderModule(VkShaderStageFlagBits stage, const char *entry = "main");

    static ShaderModule from_file(VkDevice device, const char *path,
                                  VkShaderStageFlagBits stage,
                                  const char *entry = "main");
};

#endif // ALCHEMIST_GRAPHICS_VULKAN_SHADER_H
