
#include <algorithm>

#include "graphics/vulkan/base.hpp"
#include "graphics/vulkan/depth.hpp"
#include "graphics/vulkan/device.hpp"
#include "graphics/vulkan/framebuffer.hpp"
#include "graphics/vulkan/image_view.hpp"
#include "graphics/vulkan/instance.hpp"
#include "graphics/vulkan/khr/surface.hpp"
#include "graphics/vulkan/khr/swapchain.hpp"
#include "graphics/vulkan/physical_device.hpp"

#ifdef ALCHEMIST_DEBUG
#include <iostream>

#endif

int vulkan_base_init(VulkanBase &base, GLFWwindow *window,
                     const VulkanBaseCreateInfo &create_info) {
    VkPhysicalDevice *physical_devices = nullptr;
    uint32_t queue_family_indices[4];
    VkDeviceQueueCreateInfo *queue_create_info = nullptr;
    uint32_t device_count = 0;
    uint32_t unique_queue_families = 0;

    InstanceCreateInfo instance_create_info(create_info.application_name,
                                            create_info.application_version);
    instance_create_info.engine_name = create_info.engine_name;
    instance_create_info.engine_version = create_info.engine_version;

#ifdef ALCHEMIST_DEBUG
    base.instance = create_instance(instance_create_info, &base.messenger);

#else
    base.instance = create_instance(instance_create_info);

#endif

    base.surface = create_surface(window, base.instance);

    PhysicalDevicePickInfo pick_info;
    pick_info.requirements =
        (QueueFamilyRequirements)(QUEUE_FAMILY_REQUIREMENTS_GRAPHICS |
                                  QUEUE_FAMILY_REQUIREMENTS_COMPUTE |
                                  QUEUE_FAMILY_REQUIREMENTS_TRANSFER);
    pick_info.preference = create_info.queue_family_preferences;

    pick_physical_device(base.instance, base.surface, base.physical_device,
                         pick_info);

    base.depth_format = query_depth_format(
        base.physical_device.device,
        DepthQuery({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
                    VK_FORMAT_D24_UNORM_S8_UINT},
                   VK_IMAGE_TILING_OPTIMAL,
                   VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT));

    base.device = create_device(base.physical_device.device,
                                base.physical_device.features,
                                base.physical_device.indices);

    create_swapchain(base.swapchain, window, base.device,
                     base.physical_device.device, base.surface,
                     base.physical_device.indices);

    // Get swapchain images
    vkGetSwapchainImagesKHR(base.device, base.swapchain.swapchain,
                            &base.swapchain.image_count, nullptr);
    base.swapchain_images = new VkImage[base.swapchain.image_count];
    base.swapchain_image_views = new VkImageView[base.swapchain.image_count];
    vkGetSwapchainImagesKHR(base.device, base.swapchain.swapchain,
                            &base.swapchain.image_count, base.swapchain_images);

    for (uint32_t i = 0; i < base.swapchain.image_count; ++i) {
        base.swapchain_image_views[i] = create_image_view(
            base.device, base.swapchain_images[i], base.swapchain.format.format,
            ImageViewCreateInfo());
        if (base.swapchain_image_views[i] == VK_NULL_HANDLE) {
#ifdef ALCHEMIST_DEBUG
            std::cerr << "Failed to create swapchain image view." << std::endl;
#endif
            vkDestroySwapchainKHR(base.device, base.swapchain.swapchain,
                                  nullptr);
            vkDestroyDevice(base.device, nullptr);
            vkDestroySurfaceKHR(base.instance, base.surface, nullptr);
            vkDestroyInstance(base.instance, nullptr);
            return -1; // Failed to create swapchain image view
        }
    }

    return 0;
}
