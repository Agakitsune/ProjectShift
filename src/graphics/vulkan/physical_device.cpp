
#include "graphics/vulkan/physical_device.hpp"

#ifdef ALCHEMIST_DEBUG
#include <iostream>
#endif

static uint32_t is_data_in_set(uint32_t value, const uint32_t *set,
                               uint32_t count) {
    for (uint32_t i = 0; i < count; ++i) {
        if (set[i] == value) {
            return 1; // Value is in the set
        }
    }
    return 0; // Value is not in the set
}

static FamilyIndices find_queue_families(VkPhysicalDevice device,
                                         VkSurfaceKHR surface,
                                         const PhysicalDevicePickInfo &info) {
    FamilyIndices indices;
    uint32_t *set;
    VkQueueFamilyProperties *queue_families = nullptr;
    uint32_t queue_family_count = 0;
    VkBool32 present_support = false;

    uint32_t count = 0;

    indices.graphics = UINT32_MAX;
    indices.present = UINT32_MAX;

    indices.compute = UINT32_MAX;
    indices.transfer = UINT32_MAX;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                             nullptr);
    queue_families = new VkQueueFamilyProperties[queue_family_count];
    set = new uint32_t[queue_family_count];

    for (uint32_t i = 0; i < queue_family_count; ++i) {
        set[i] = UINT32_MAX; // Initialize the set with invalid values
    }

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count,
                                             queue_families);

    for (uint32_t i = 0; i < queue_family_count; ++i) {
        present_support = false;

        if ((info.requirements & queue_families[i].queueFlags) &
            QUEUE_FAMILY_REQUIREMENTS_GRAPHICS) {
            if (indices.graphics == UINT32_MAX ||
                is_data_in_set(i, set, count) == info.preference) {
                indices.graphics = i;
                if (is_data_in_set(i, set, count) == 0) {
                    set[count] = i;
                    count++;
                }
            }

            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface,
                                                 &present_support);
            if (present_support) {
                if (indices.present == UINT32_MAX ||
                    is_data_in_set(i, set, count) == info.preference) {
                    indices.present = i;
                    if (is_data_in_set(i, set, count) == 0) {
                        set[count] = i;
                        count++;
                    }
                }
            }
        }
        if ((info.requirements & queue_families[i].queueFlags) &
            QUEUE_FAMILY_REQUIREMENTS_COMPUTE) {
            if (indices.compute == UINT32_MAX ||
                is_data_in_set(i, set, count) == info.preference) {
                indices.compute = i;
                if (is_data_in_set(i, set, count) == 0) {
                    set[count] = i;
                    count++;
                }
            }
        }
        if ((info.requirements & queue_families[i].queueFlags) &
            QUEUE_FAMILY_REQUIREMENTS_TRANSFER) {
            if (indices.transfer == UINT32_MAX ||
                is_data_in_set(i, set, count) == info.preference) {
                indices.transfer = i;
                if (is_data_in_set(i, set, count) == 0) {
                    set[count] = i;
                    count++;
                }
            }
        }
    }

    delete[] queue_families;
    delete[] set;

    return indices;
}

static uint32_t rate_physical_device(VkPhysicalDevice device,
                                     const FamilyIndices &indices,
                                     const QueueFamilyPreferences preferences) {
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;

    uint32_t set[4];
    uint32_t count = 0;

    uint32_t score = 0;
    uint32_t family_combined = 0;
    uint32_t family_separate = 0;

    if (preferences == QUEUE_FAMILY_PREFERENCES_COMBINED) {
        family_combined = 100; // Prefer combined queues for graphics, compute,
                               // transfer, present
        family_separate = 0;
    } else {
        family_combined = 0;
        family_separate = 100; // Prefer separate queues for graphics, compute,
                               // transfer, present
    }

    vkGetPhysicalDeviceProperties(device, &properties);
    vkGetPhysicalDeviceFeatures(device, &features);

    if (!features.geometryShader) {
        return 0; // Geometry shaders are required
    }

    if (indices.graphics == UINT32_MAX || indices.present == UINT32_MAX) {
        return 0; // Graphics and presentation queues are required
    }

    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000; // Discrete GPUs are preferred
    }

    score += properties.limits
                 .maxImageDimension2D; // Higher resolution support is better

    if (indices.graphics != UINT32_MAX) {
        // set is empty, so we can add the graphics queue directly
        set[count++] = indices.graphics; // Add graphics queue to the set
        score += family_separate;        // Separate graphics queue is better
    }

    if (indices.compute != UINT32_MAX) {
        if (is_data_in_set(indices.compute, set, count) == 0) {
            set[count++] = indices.compute; // Add compute queue to the set
            score += family_separate;       // Separate compute queue is better
        } else {
            score +=
                family_combined; // Compute queue is the same as another queue
        }
    }

    if (indices.transfer != UINT32_MAX) {
        if (is_data_in_set(indices.transfer, set, count) == 0) {
            set[count++] = indices.transfer; // Add transfer queue to the set
            score += family_separate; // Separate transfer queue is better
        } else {
            score +=
                family_combined; // Transfer queue is the same as another queue
        }
    }

    if (indices.present == indices.graphics) {
        score += family_combined; // Present queue is the same as graphics queue
    } else {
        score += family_separate; // Separate present queue is better
    }

    return score;
}

void pick_physical_device(VkInstance instance, VkSurfaceKHR surface,
                          PhysicalDevice &device,
                          const PhysicalDevicePickInfo &pick_info) {
    VkPhysicalDevice *physical_devices = nullptr;
    FamilyIndices *indices = nullptr;
    uint32_t device_count = 0;
    uint32_t score = 0;
    uint32_t best_score = 0;
    uint32_t best_index = 0;

    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);

    if (device_count == 0) {
#ifdef ALCHEMIST_DEBUG
        std::cerr << "No Vulkan-compatible physical devices found."
                  << std::endl;
#endif
        return; // No Vulkan-compatible physical devices found
    }

    physical_devices = new VkPhysicalDevice[device_count];
    indices = new FamilyIndices[device_count];
    if (vkEnumeratePhysicalDevices(instance, &device_count, physical_devices) !=
        VK_SUCCESS) {
#ifdef ALCHEMIST_DEBUG
        std::cerr << "Failed to enumerate physical devices." << std::endl;
#endif
        delete[] physical_devices;
        delete[] indices;
        return; // Failed to enumerate physical devices
    }

    for (uint32_t i = 0; i < device_count; ++i) {
        indices[i] =
            find_queue_families(physical_devices[i], surface, pick_info);
        score = rate_physical_device(physical_devices[i], indices[i],
                                     pick_info.preference);
        if (score > best_score) {
            best_score = score;
            best_index = i;
        }
    }

    device.device = physical_devices[best_index];
    device.indices = indices[best_index];
    vkGetPhysicalDeviceProperties(device.device, &device.properties);
    vkGetPhysicalDeviceFeatures(device.device, &device.features);

    delete[] physical_devices;
    delete[] indices;

    std::cout << "Selected physical device: " << device.properties.deviceName
              << std::endl;
    std::cout << "Graphics queue family index: " << device.indices.graphics
              << std::endl;
    std::cout << "Present queue family index: " << device.indices.present
              << std::endl;
    std::cout << "Compute queue family index: " << device.indices.compute
              << std::endl;
    std::cout << "Transfer queue family index: " << device.indices.transfer
              << std::endl;

    return;
}
