
#ifndef ALCHEMIST_SERVER_SHADER_HPP
#define ALCHEMIST_SERVER_SHADER_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "server/rid.hpp"

struct Shader {
    VkShaderModule shader_module; // Vulkan shader module object
    RID rid = RID_INVALID; // Resource ID for the shader

    Shader() = default;
};

struct ShaderServer {
    std::vector<Shader> shaders; // Vector to hold all shaders

    VkDevice device; // Vulkan device

    ShaderServer(VkDevice device);
    ~ShaderServer();

    RID new_shader(const VkShaderModuleCreateInfo &create_info);
    RID new_shader(VkShaderModuleCreateInfo &&create_info);

    RID from_file(const char *file_path);

    const Shader &get_shader(RID rid) const;

    static ShaderServer &instance();
    static std::unique_ptr<ShaderServer> __instance; // Singleton instance of ShaderServer
};

#endif // ALCHEMIST_SERVER_SHADER_HPP
