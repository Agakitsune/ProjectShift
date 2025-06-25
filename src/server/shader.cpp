
#ifdef ALCHEMIST_DEBUG
#include <iostream> // Include for debug output
#endif // ALCHEMIST_DEBUG

#include <fstream>

#include "server/shader.hpp"

ShaderServer::ShaderServer(VkDevice device) : device(device) {}
ShaderServer::~ShaderServer() {
    for (auto &shader : shaders) {
        #ifdef ALCHEMIST_DEBUG
        std::cout << "Destroying shader with RID: " << shader.rid << std::endl;
        #endif
        vkDestroyShaderModule(device, shader.shader_module, nullptr); // Clean up each shader module
    }
}

RID ShaderServer::new_shader(const VkShaderModuleCreateInfo &create_info) {
    VkShaderModule shader_module;
    RID rid = RIDServer::instance().new_id(RIDServer::SHADER);

    if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create shader module!" << std::endl;
    #endif
        return RID_INVALID; // Return an invalid RID on failure
    }

    Shader shader;
    shader.shader_module = shader_module;
    shader.rid = rid;

    shaders.emplace_back(std::move(shader)); // Add the created shader to the server's shaders vector

    return rid; // Return the RID of the newly created shader
}

RID ShaderServer::new_shader(VkShaderModuleCreateInfo &&create_info) {
    VkShaderModule shader_module;
    RID rid = RIDServer::instance().new_id(RIDServer::SHADER);

    if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create shader module!" << std::endl;
    #endif
        return RID_INVALID; // Return an invalid RID on failure
    }

    Shader shader;
    shader.shader_module = shader_module;
    shader.rid = rid;

    shaders.emplace_back(std::move(shader)); // Add the created shader to the server's shaders vector

    return rid; // Return the RID of the newly created shader
}

RID ShaderServer::from_file(const char *file_path) {
    std::ifstream file(file_path, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to open shader file: " << file_path << std::endl;
    #endif
        return RID_INVALID; // Return an invalid RID if the file cannot be opened
    }

    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> shader_code(file_size);
    file.read(reinterpret_cast<char *>(shader_code.data()), file_size);
    file.close();
    if (file.gcount() != file_size) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to read shader file: " << file_path << std::endl;
    #endif
        return RID_INVALID; // Return an invalid RID if the file cannot be read completely
    }

    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = shader_code.size();
    create_info.pCode = reinterpret_cast<const uint32_t *>(shader_code.data());
    RID rid = new_shader(create_info); // Create a new shader from the file

    if (rid == RID_INVALID) {
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create shader from file: " << file_path << std::endl;
    #endif
        return RID_INVALID; // Return an invalid RID if shader creation fails
    }

    #ifdef ALCHEMIST_DEBUG
    std::cout << "Shader created from file: " << file_path << " with RID: " << rid << std::endl;
    #endif
    return rid; // Return the RID of the newly created shader
}

const Shader &ShaderServer::get_shader(RID rid) const {
    for (const auto &shader : shaders) {
        if (shader.rid == rid) {
            return shader; // Return the shader if the RID matches
        }
    }
    #ifdef ALCHEMIST_DEBUG
    std::cerr << "Shader with RID: " << rid << " not found!" << std::endl;
    #endif
    throw std::runtime_error("Shader not found"); // Throw an error if the shader is not found
}

ShaderServer &ShaderServer::instance() {
    return *__instance; // Return the singleton instance of ShaderServer
}

std::unique_ptr<ShaderServer> ShaderServer::__instance = nullptr; // Singleton instance of ShaderServer
