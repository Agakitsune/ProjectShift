
#include <fstream>

#include "graphics/vulkan/shader.hpp"

VkShaderModule create_shader_module(VkDevice device, const char *path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        return VK_NULL_HANDLE; // Failed to open file
    }

    size_t size = (size_t)file.tellg();
    char *buffer = (char *)malloc(size);

    file.seekg(0);
    file.read(buffer, size);

    file.close();

    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = size;
    create_info.pCode = reinterpret_cast<const uint32_t *>(buffer);

    VkShaderModule shader_module;
    if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shader_module;
}

ShaderModule::ShaderModule(VkShaderStageFlagBits stage, const char *entry)
    : stage(stage), entry(entry), module(VK_NULL_HANDLE) {}

ShaderModule ShaderModule::from_file(VkDevice device, const char *path,
                                     VkShaderStageFlagBits stage,
                                     const char *entry) {
    ShaderModule shader_module(stage, entry);
    shader_module.module = create_shader_module(device, path);
    if (shader_module.module == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to create shader module from file: " +
                                 std::string(path));
    }
    return shader_module;
}
