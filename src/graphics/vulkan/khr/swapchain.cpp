
#include <algorithm>

#include "graphics/vulkan/khr/swapchain.hpp"

#ifdef ALCHEMIST_DEBUG
#include <iostream>
#endif

int query_swapchain_support(VkPhysicalDevice device, VkSurfaceKHR surface,
                            SwapchainSupportDetails &details) {
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                              &details.capabilities);

    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.format_count,
                                         nullptr);
    if (details.format_count != 0) {
        details.formats = new VkSurfaceFormatKHR[details.format_count];
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            device, surface, &details.format_count, details.formats);
    } else {
        details.formats = nullptr;
    }

    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &details.present_mode_count, nullptr);
    if (details.present_mode_count != 0) {
        details.present_modes =
            new VkPresentModeKHR[details.present_mode_count];
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
                                                  &details.present_mode_count,
                                                  details.present_modes);
    } else {
        details.present_modes = nullptr;
    }

    return 0; // Success
}

void create_swapchain(Swapchain &swapchain, GLFWwindow *window, VkDevice device,
                      VkPhysicalDevice physical_device, VkSurfaceKHR surface,
                      const FamilyIndices &indices) {
    VkSwapchainCreateInfoKHR swapchain_create_info{};
    SwapchainSupportDetails swapchain_details;
    int width, height;

    swapchain.swapchain = VK_NULL_HANDLE; // Initialize swapchain handle to null
    swapchain.format = {};
    swapchain.present_mode =
        VK_PRESENT_MODE_FIFO_KHR; // Default to FIFO present mode

    if (query_swapchain_support(physical_device, surface, swapchain_details) !=
        0) {
#ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to query swapchain support." << std::endl;
#endif
        return; // Failed to query swapchain support
    }

    for (uint32_t i = 0; i < swapchain_details.format_count; ++i) {
        if (swapchain_details.formats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
            swapchain_details.formats[i].colorSpace ==
                VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            swapchain.format = swapchain_details.formats[i];
            break;
        }
    }

    for (uint32_t i = 0; i < swapchain_details.present_mode_count; ++i) {
        if (swapchain_details.present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            swapchain.present_mode = swapchain_details.present_modes[i];
            break;
        }
    }

    delete[] swapchain_details.formats;
    delete[] swapchain_details.present_modes;

    glfwGetFramebufferSize(window, &width, &height);
    swapchain.extent.width = static_cast<uint32_t>(width);
    swapchain.extent.height = static_cast<uint32_t>(height);
    if (swapchain.extent.width == 0 || swapchain.extent.height == 0) {
#ifdef ALCHEMIST_DEBUG
        std::cerr << "Invalid swapchain extent." << std::endl;
#endif
        return; // Invalid swapchain extent
    }

    swapchain.extent.width =
        std::clamp(swapchain.extent.width,
                   swapchain_details.capabilities.minImageExtent.width,
                   swapchain_details.capabilities.maxImageExtent.width);
    swapchain.extent.height =
        std::clamp(swapchain.extent.height,
                   swapchain_details.capabilities.minImageExtent.height,
                   swapchain_details.capabilities.maxImageExtent.height);

    swapchain.image_count = swapchain_details.capabilities.minImageCount + 1;

    if (swapchain_details.capabilities.maxImageCount > 0 &&
        swapchain.image_count > swapchain_details.capabilities.maxImageCount) {
        swapchain.image_count = swapchain_details.capabilities.maxImageCount;
    }

    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface = surface;
    swapchain_create_info.minImageCount = swapchain.image_count;
    swapchain_create_info.imageFormat = swapchain.format.format;
    swapchain_create_info.imageColorSpace = swapchain.format.colorSpace;
    swapchain_create_info.imageExtent = swapchain.extent;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t swap_queue_family_indices[2] = {indices.graphics, indices.present};
    if (indices.graphics != indices.present) {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices = swap_queue_family_indices;
    } else {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_create_info.queueFamilyIndexCount = 0;
        swapchain_create_info.pQueueFamilyIndices = nullptr;
    }

    swapchain_create_info.preTransform =
        swapchain_details.capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = swapchain.present_mode;
    swapchain_create_info.clipped = VK_TRUE;
    swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device, &swapchain_create_info, nullptr,
                             &swapchain.swapchain) != VK_SUCCESS) {
#ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to create swapchain." << std::endl;
#endif
        return; // Failed to create swapchain
    }

    return;
}
