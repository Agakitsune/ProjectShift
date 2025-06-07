
#ifndef ALCHEMIST_GRAPHICS_VULKAN_DEVICE_H
#define ALCHEMIST_GRAPHICS_VULKAN_DEVICE_H

#include <vulkan/vulkan.h>

#include "graphics/vulkan/physical_device.hpp"

VkDevice create_device(VkPhysicalDevice physical_device,
                       const VkPhysicalDeviceFeatures &features,
                       const FamilyIndices &indices);

#endif
