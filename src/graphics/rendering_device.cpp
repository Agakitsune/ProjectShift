
#include <algorithm>

#include "graphics/rendering_device.hpp"

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

static const char *device_features[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

static uint32_t is_data_in_set(uint32_t value, const uint32_t *set,
                               uint32_t count) {
    for (uint32_t i = 0; i < count; ++i) {
        if (set[i] == value) {
            return 1; // Value is in the set
        }
    }
    return 0; // Value is not in the set
}

static void find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface, Indices &indices) {
    VkQueueFamilyProperties *queue_families = nullptr;
    uint32_t queue_family_count = 0;
    VkBool32 present_support = false;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                             nullptr);
    queue_families = new VkQueueFamilyProperties[queue_family_count];

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                             queue_families);

    for (uint32_t i = 0; i < queue_family_count; ++i) {
        present_support = false;

        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics = i;

            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface,
                                                 &present_support);
            if (present_support) {
                indices.present = i;
            }
        }
        if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            indices.compute = i;
        }
        if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            indices.transfer = i;
        }
    }

    delete[] queue_families;
}

static uint32_t rate_physical_device(VkPhysicalDevice device, Indices &indices) {
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;

    // uint32_t set[4];
    // uint32_t count = 0;

    uint32_t score = 0;
    // uint32_t family_combined = 0;
    // uint32_t family_separate = 0;

    vkGetPhysicalDeviceProperties(device, &properties);
    vkGetPhysicalDeviceFeatures(device, &features);

    if (!features.geometryShader) {
        return 0; // Geometry shaders are required
    }

    if (indices.graphics == UINT32_MAX
        || indices.present == UINT32_MAX
        || indices.compute == UINT32_MAX
        || indices.transfer == UINT32_MAX
    ) {
        return 0; // Graphics and presentation queues are required
    }

    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000; // Discrete GPUs are preferred
    }

    score += properties.limits
                 .maxImageDimension2D; // Higher resolution support is better

    return score;
}

void pick_physical_device(RenderingDevice &device) {
    Indices *indices = nullptr;
    VkPhysicalDevice *physical_devices = nullptr;
    uint32_t device_count = 0;
    uint32_t score = 0;
    uint32_t best_score = 0;
    uint32_t best_index = 0;

    vkEnumeratePhysicalDevices(device.instance, &device_count, nullptr);

    if (device_count == 0) {
#ifdef ALCHEMIST_DEBUG
        std::cerr << "No Vulkan-compatible physical devices found."
                  << std::endl;
#endif
        return; // No Vulkan-compatible physical devices found
    }

    physical_devices = new VkPhysicalDevice[device_count];
    indices = new Indices[device_count];
    if (vkEnumeratePhysicalDevices(device.instance, &device_count, physical_devices) !=
        VK_SUCCESS) {
#ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to enumerate physical devices." << std::endl;
#endif
        delete[] physical_devices;
        delete[] indices;
        return; // Failed to enumerate physical devices
    }

    for (uint32_t i = 0; i < device_count; ++i) {
        find_queue_families(physical_devices[i], device.surface, indices[i]);
        score = rate_physical_device(physical_devices[i], indices[i]);
        if (score > best_score) {
            best_score = score;
            best_index = i;
        }
    }

    device.physical_device = physical_devices[best_index];
    device.graphics_queue_family_index = indices[best_index].graphics;
    device.present_queue_family_index = indices[best_index].present;
    device.compute_queue_family_index = indices[best_index].compute;
    device.transfer_queue_family_index = indices[best_index].transfer;
    
    // vkGetPhysicalDeviceProperties(device.device, &device.properties);
    // vkGetPhysicalDeviceFeatures(device.device, &device.features);

    delete[] physical_devices;
    delete[] indices;
#ifdef ALCHEMIST_DEBUG
    // std::cout << "Selected physical device: " << device.properties.deviceName
            //   << std::endl;
    std::cout << "Graphics queue family index: " << device.graphics_queue_family_index
              << std::endl;
    std::cout << "Present queue family index: " << device.present_queue_family_index
              << std::endl;
    std::cout << "Compute queue family index: " << device.compute_queue_family_index
              << std::endl;
    std::cout << "Transfer queue family index: " << device.transfer_queue_family_index
              << std::endl;
#endif

    return;
}

void create_device(RenderingDevice &device) {
    VkDeviceCreateInfo device_create_info{};
    VkPhysicalDeviceFeatures features{};
    VkDeviceQueueCreateInfo queue_create_info[4]{};
    float priorities = 1.f;
    uint32_t count = 1;

    vkGetPhysicalDeviceFeatures(device.physical_device, &features);

    // Set up queue create info
    queue_create_info[0].sType =
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info[0].queueFamilyIndex = device.graphics_queue_family_index;
    queue_create_info[0].queueCount = 1;
    queue_create_info[0].pQueuePriorities = &priorities;
    queue_create_info[0].flags = 0; // No flags
    queue_create_info[0].pNext = nullptr;

    if (device.graphics_queue_family_index !=
        device.present_queue_family_index) {
        // If graphics and present queues are separate, create a separate queue
        queue_create_info[count].sType =
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info[count].queueFamilyIndex = device.present_queue_family_index;
        queue_create_info[count].queueCount = 1;
        queue_create_info[count].pQueuePriorities = &priorities;
        queue_create_info[count].flags = 0; // No flags
        queue_create_info[count].pNext = nullptr;
        count++;
    }

    if (device.compute_queue_family_index !=
        device.graphics_queue_family_index &&
        device.compute_queue_family_index !=
            device.present_queue_family_index) {
        // If compute queue is separate, create a separate queue
        queue_create_info[count].sType =
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info[count].queueFamilyIndex = device.compute_queue_family_index;
        queue_create_info[count].queueCount = 1;
        queue_create_info[count].pQueuePriorities = &priorities;
        queue_create_info[count].flags = 0; // No flags
        queue_create_info[count].pNext = nullptr;
        count++;
    }

    if (device.transfer_queue_family_index !=
        device.graphics_queue_family_index &&
        device.transfer_queue_family_index !=
            device.present_queue_family_index &&
        device.transfer_queue_family_index !=
            device.compute_queue_family_index) {
        // If transfer queue is separate, create a separate queue
        queue_create_info[count].sType =
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info[count].queueFamilyIndex = device.transfer_queue_family_index;
        queue_create_info[count].queueCount = 1;
        queue_create_info[count].pQueuePriorities = &priorities;
        queue_create_info[count].flags = 0; // No flags
        queue_create_info[count].pNext = nullptr;
        count++;
    }

    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.queueCreateInfoCount = count; // Number of queue create infos
    device_create_info.pQueueCreateInfos = queue_create_info; // Set the queue create infos
    device_create_info.pEnabledFeatures =
        &features; // Set the enabled features
    device_create_info.enabledExtensionCount =
        sizeof(device_features) / sizeof(const char *);
    device_create_info.ppEnabledExtensionNames =
        device_features; // Device features

#ifdef ALCHEMIST_DEBUG

    device_create_info.enabledLayerCount =
        sizeof(validation_layers) / sizeof(const char *);
    device_create_info.ppEnabledLayerNames =
        validation_layers; // Enable validation layers

#else

    device_create_info.enabledLayerCount =
        0; // No validation layers in this example

#endif

    if (vkCreateDevice(device.physical_device, &device_create_info, nullptr,
                       &device.device) != VK_SUCCESS) {
#ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create Vulkan device." << std::endl;
#endif
    }
}

int query_swapchain_support(VkPhysicalDevice device, VkSurfaceKHR surface) {
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR *formats = nullptr;
    VkPresentModeKHR *present_modes = nullptr;

    uint32_t format_count = 0;
    uint32_t present_mode_count = 0;
    
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities);

    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count,
                                         nullptr);
    if (format_count != 0) {
        formats = new VkSurfaceFormatKHR[format_count];
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            device, surface, &format_count, formats);
    } else {
        formats = nullptr;
    }

    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &present_mode_count, nullptr);
    if (present_mode_count != 0) {
        present_modes =
            new VkPresentModeKHR[present_mode_count];
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
                                                  &present_mode_count,
                                                  present_modes);
    } else {
        present_modes = nullptr;
    }

    return 0; // Success
}

void create_swapchain(RenderingDevice &device, GLFWwindow *window) {
    VkSwapchainCreateInfoKHR swapchain_create_info{};
    // SwapchainSupportDetails swapchain_details;

    // swapchain.swapchain = VK_NULL_HANDLE; // Initialize swapchain handle to null
    // swapchain.format = {};
    // swapchain.present_mode =
    //     VK_PRESENT_MODE_FIFO_KHR; // Default to FIFO present mode

    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR *formats = nullptr;
    VkPresentModeKHR *present_modes = nullptr;

    uint32_t format_count = 0;
    uint32_t present_mode_count = 0;

    int width, height;
    
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.physical_device, device.surface, &capabilities);

    vkGetPhysicalDeviceSurfaceFormatsKHR(device.physical_device, device.surface, &format_count,
                                         nullptr);
    if (format_count != 0) {
        formats = new VkSurfaceFormatKHR[format_count];
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            device.physical_device, device.surface, &format_count, formats);
    } else {
        formats = nullptr;
    }

    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device.physical_device, device.surface, &present_mode_count, nullptr);
    if (present_mode_count != 0) {
        present_modes =
            new VkPresentModeKHR[present_mode_count];
        vkGetPhysicalDeviceSurfacePresentModesKHR(device.physical_device, device.surface,
                                                  &present_mode_count,
                                                  present_modes);
    } else {
        present_modes = nullptr;
    }

    for (uint32_t i = 0; i < format_count; ++i) {
        if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
            formats[i].colorSpace ==
                VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            device.surface_format = formats[i];
            break;
        }
    }

    device.present_mode = VK_PRESENT_MODE_FIFO_KHR; // Default to FIFO present mode

    for (uint32_t i = 0; i < present_mode_count; ++i) {
        if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            device.present_mode = present_modes[i];
            break;
        }
    }

    delete[] formats;
    delete[] present_modes;

    glfwGetFramebufferSize(window, &width, &height);
    device.swapchain_extent.width = static_cast<uint32_t>(width);
    device.swapchain_extent.height = static_cast<uint32_t>(height);
    if (device.swapchain_extent.width == 0 || device.swapchain_extent.height == 0) {
#ifdef ALCHEMIST_DEBUG
        std::cerr << "Invalid swapchain extent." << std::endl;
#endif
        return; // Invalid swapchain extent
    }

    device.swapchain_extent.width =
        std::clamp(device.swapchain_extent.width,
                   capabilities.minImageExtent.width,
                   capabilities.maxImageExtent.width);
                   device.swapchain_extent.height =
        std::clamp(device.swapchain_extent.height,
                   capabilities.minImageExtent.height,
                   capabilities.maxImageExtent.height);

    device.swapchain_image_count = capabilities.minImageCount + 1;

    if (capabilities.maxImageCount > 0 &&
        device.swapchain_image_count > capabilities.maxImageCount) {
        device.swapchain_image_count = capabilities.maxImageCount;
    }

    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface = device.surface;
    swapchain_create_info.minImageCount = device.swapchain_image_count;
    swapchain_create_info.imageFormat = device.surface_format.format;
    swapchain_create_info.imageColorSpace = device.surface_format.colorSpace;
    swapchain_create_info.imageExtent = device.swapchain_extent;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t swap_queue_family_indices[2] = {device.graphics_queue_family_index, device.present_queue_family_index};
    if (device.graphics_queue_family_index != device.present_queue_family_index) {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices = swap_queue_family_indices;
    } else {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_create_info.queueFamilyIndexCount = 0;
        swapchain_create_info.pQueueFamilyIndices = nullptr;
    }

    swapchain_create_info.preTransform =
        capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = device.present_mode;
    swapchain_create_info.clipped = VK_TRUE;
    swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device.device, &swapchain_create_info, nullptr,
                             &device.swapchain) != VK_SUCCESS) {
#ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create swapchain." << std::endl;
#endif
        return; // Failed to create swapchain
    }

    return;
}

RenderingDevice::RenderingDevice(const ApplicationInfo &info) {
    VkInstanceCreateInfo create_info_vk{};
    VkApplicationInfo app_info{};

    #ifdef ALCHEMIST_DEBUG

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};

    #endif

    const char **glfw_extensions = nullptr;
    const char **extensions = nullptr;
    uint32_t glfw_extension_count = 0;
    uint32_t count = 0;

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
    app_info.pApplicationName = info.application_name;
    app_info.applicationVersion = info.application_version;
    app_info.pEngineName = info.engine_name;
    app_info.engineVersion = info.engine_version;
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

    create_info_vk.enabledLayerCount = 0; // No validation layers in this example

    #endif

    if (vkCreateInstance(&create_info_vk, nullptr, &instance) != VK_SUCCESS) {
    
    #ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create Vulkan instance." << std::endl;
    #endif
        
        return;
    }

    delete[] extensions; // Clean up the extensions array

    #ifdef ALCHEMIST_DEBUG

        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, &debug_create_info, nullptr, &messenger);
        }

    #endif

    if (glfwCreateWindowSurface(instance, info.window, nullptr, &surface) !=
        VK_SUCCESS) {
    
    #ifdef ALCHEMIST_DEBUG
            std::cerr << "Failed to create Vulkan surface." << std::endl;
    #endif

        return; // Failed to create Vulkan surface
    }

    pick_physical_device(*this);
    create_device(*this);
    create_swapchain(*this, info.window);

    vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, nullptr);
    swapchain_images = new VkImage[swapchain_image_count];
    swapchain_image_views = new VkImageView[swapchain_image_count];
    if (vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, swapchain_images) != VK_SUCCESS) {
    #ifdef ALCHEMSIT_DEBUG
        std::cerr << "Failed to create Images." << std::endl;
    #endif
    }

    for (uint32_t i = 0; i < swapchain_image_count; i++) {
        VkImageViewCreateInfo view_info{};

        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = swapchain_images[i];
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = surface_format.format;
        view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

        if (vkCreateImageView(device, &view_info, nullptr, &swapchain_image_views[i]) !=
            VK_SUCCESS) {
        #ifdef ALCHEMSIT_DEBUG
                std::cerr << "Failed to create Image Views." << std::endl;
        #endif
        }
    }

    for (VkFormat format : {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}) {
        VkFormatProperties format_props;
        vkGetPhysicalDeviceFormatProperties(physical_device, format, &format_props);

        if (format_props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            depth_format = format;
            break; // Found a suitable depth format
        }
    }

    vkGetDeviceQueue(device, graphics_queue_family_index, 0, &graphics_queue);
    vkGetDeviceQueue(device, present_queue_family_index, 0, &present_queue);
    vkGetDeviceQueue(device, compute_queue_family_index, 0, &compute_queue);
    vkGetDeviceQueue(device, transfer_queue_family_index, 0, &transfer_queue);
}

RenderingDevice::~RenderingDevice() {
    for (uint32_t i = 0; i < swapchain_image_count; i++) {
        vkDestroyImageView(device, swapchain_image_views[i], nullptr);
    }
    delete[] swapchain_image_views;

    delete[] swapchain_images;

    vkDestroySwapchainKHR(device, swapchain, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);

    vkDestroyDevice(device, nullptr);
    #ifdef ALCHEMIST_DEBUG
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, messenger, nullptr);
    }
    #endif
    vkDestroyInstance(instance, nullptr);
}

RenderingDevice &new_rendering_device(const ApplicationInfo &info) {
    if (rendering_device != nullptr) {
        delete rendering_device; // Clean up existing device
    }
    rendering_device = new RenderingDevice(info);
    return *rendering_device;
}

RenderingDevice &get_rendering_device() {
    return *rendering_device;
}

RenderingDevice *rendering_device = nullptr;
