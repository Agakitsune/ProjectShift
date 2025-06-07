
#include "graphics/vulkan/device.hpp"

#ifdef ALCHEMIST_DEBUG
#include <iostream>

const char *validation_layers[] = {"VK_LAYER_KHRONOS_validation"};

#endif

const char *device_features[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

static uint32_t is_data_in_set(uint32_t value, const uint32_t *set,
                               uint32_t count) {
    for (uint32_t i = 0; i < count; ++i) {
        if (set[i] == value) {
            return 1; // Value is in the set
        }
    }
    return 0; // Value is not in the set
}

VkDevice create_device(VkPhysicalDevice physical_device,
                       const VkPhysicalDeviceFeatures &featuress,
                       const FamilyIndices &indices) {
    VkDeviceCreateInfo device_create_info{};
    VkDeviceQueueCreateInfo queue_create_info[4]{};
    uint32_t set[4];
    uint32_t count = 0;

    // Set up queue create info
    if (indices.graphics != UINT32_MAX) {
        queue_create_info[count].sType =
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info[count].queueFamilyIndex = indices.graphics;
        queue_create_info[count].queueCount = 1;
        queue_create_info[count].pQueuePriorities = (float[]){1.0f};
        queue_create_info[count].flags = 0; // No flags
        queue_create_info[count].pNext = nullptr;
        set[count++] = indices.graphics; // Add graphics queue to the set
    }

    if (indices.compute != UINT32_MAX) {
        if (is_data_in_set(indices.compute, set, count) == 0) {
            queue_create_info[count].sType =
                VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info[count].queueFamilyIndex = indices.compute;
            queue_create_info[count].queueCount = 1;
            queue_create_info[count].pQueuePriorities = (float[]){1.0f};
            queue_create_info[count].flags = 0; // No flags
            queue_create_info[count].pNext = nullptr;
            set[count++] = indices.compute; // Add compute queue to the set
        }
    }

    if (indices.transfer != UINT32_MAX) {
        if (is_data_in_set(indices.transfer, set, count) == 0) {
            queue_create_info[count].sType =
                VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info[count].queueFamilyIndex = indices.transfer;
            queue_create_info[count].queueCount = 1;
            queue_create_info[count].pQueuePriorities = (float[]){1.0f};
            queue_create_info[count].flags = 0; // No flags
            queue_create_info[count].pNext = nullptr;
            set[count++] = indices.transfer; // Add transfer queue to the set
        }
    }

    if (indices.present != UINT32_MAX) {
        if (is_data_in_set(indices.present, set, count) == 0) {
            queue_create_info[count].sType =
                VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info[count].queueFamilyIndex = indices.present;
            queue_create_info[count].queueCount = 1;
            queue_create_info[count].pQueuePriorities = (float[]){1.0f};
            queue_create_info[count].flags = 0; // No flags
            queue_create_info[count].pNext = nullptr;
            set[count++] = indices.present; // Add present queue to the set
        }
    }

    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.queueCreateInfoCount =
        count; // Set the number of queue create infos
    device_create_info.pQueueCreateInfos =
        queue_create_info; // Set the queue create infos
    device_create_info.pEnabledFeatures =
        &featuress; // Set the enabled features
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

    VkDevice device = VK_NULL_HANDLE;
    if (vkCreateDevice(physical_device, &device_create_info, nullptr,
                       &device) != VK_SUCCESS) {
#ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create Vulkan device." << std::endl;
#endif
        return VK_NULL_HANDLE; // Failed to create device
    }

    return device; // Successfully created Vulkan device
}
