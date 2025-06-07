
#include "graphics/vulkan/instance.hpp"

#ifdef ALCHEMIST_DEBUG
#include <iostream>

static const char *validation_layers[] = {"VK_LAYER_KHRONOS_validation"};

static const char *
get_severity_string(VkDebugUtilsMessageSeverityFlagBitsEXT severity) {
    switch (severity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        return "VERBOSE";
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        return "INFO";
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        return "WARNING";
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}

static const char *
get_ansi_color(VkDebugUtilsMessageSeverityFlagBitsEXT severity) {
    switch (severity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        return "\033[0;37m"; // White
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        return "\033[0;36m"; // Cyan
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        return "\033[0;33m"; // Yellow
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        return "\033[0;31m"; // Red
    default:
        return "\033[0m"; // Reset
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
               VkDebugUtilsMessageTypeFlagsEXT message_type,
               const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
               void *user_data) {
    // Ansi escape codes for colored output
    std::cerr << get_ansi_color(message_severity)
              << "Vulkan Debug: " << get_severity_string(message_severity)
              << " - " << callback_data->pMessageIdName << " ("
              << callback_data->messageIdNumber
              << "): " << callback_data->pMessage << "\033[0m" << std::endl;

    return VK_FALSE;
}

#endif

InstanceCreateInfo::InstanceCreateInfo(const char *app_name,
                                       uint32_t app_version)
    : application_name(app_name), engine_name("Alchemist Engine"),
      application_version(app_version),
      engine_version(VK_MAKE_API_VERSION(0, 1, 0, 0)) // Default engine version
{}

VkInstance create_instance(const InstanceCreateInfo &create_info,
                           VkDebugUtilsMessengerEXT *debug_messenger) {
    VkInstanceCreateInfo create_info_vk{};
    VkApplicationInfo app_info{};

#ifdef ALCHEMIST_DEBUG

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};

#endif

    const char **glfw_extensions = nullptr;
    const char **extensions = nullptr;
    uint32_t glfw_extension_count = 0;
    uint32_t count = 0;

    VkInstance instance;

    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

#ifdef ALCHEMIST_DEBUG
    extensions = new const char *[glfw_extension_count + 1];
    count = glfw_extension_count + 1;
    for (uint32_t i = 0; i < glfw_extension_count; ++i) {
        extensions[i] = glfw_extensions[i];
    }
    extensions[glfw_extension_count] =
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME; // Add debug utils extension
#else
    extensions = new const char *[glfw_extension_count];
    count = glfw_extension_count;
    for (uint32_t i = 0; i < glfw_extension_count; ++i) {
        extensions[i] = glfw_extensions[i];
    }
#endif

    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = create_info.application_name;
    app_info.applicationVersion = create_info.application_version;
    app_info.pEngineName = create_info.engine_name;
    app_info.engineVersion = create_info.engine_version;
    app_info.apiVersion = VK_API_VERSION_1_4;

    create_info_vk.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info_vk.pApplicationInfo = &app_info;
    create_info_vk.enabledExtensionCount = count;
    create_info_vk.ppEnabledExtensionNames = extensions;

#ifdef ALCHEMIST_DEBUG
    // Enable validation layers if in debug mode
    create_info_vk.enabledLayerCount =
        sizeof(validation_layers) /
        sizeof(const char *); // Enable validation layers
    create_info_vk.ppEnabledLayerNames = validation_layers;

    debug_create_info.sType =
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_create_info.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_create_info.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_create_info.pfnUserCallback = debug_callback;

    create_info_vk.pNext = &debug_create_info; // Attach debug create info

#else

    create_info_vk.enabledLayerCount =
        0; // No validation layers in this example

#endif

    if (vkCreateInstance(&create_info_vk, nullptr, &instance) != VK_SUCCESS) {
#ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create Vulkan instance." << std::endl;
#endif
        return VK_NULL_HANDLE; // Failed to create instance
    }

#ifdef ALCHEMIST_DEBUG

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, &debug_create_info, nullptr, debug_messenger);
    }

#endif

    return instance;
}
