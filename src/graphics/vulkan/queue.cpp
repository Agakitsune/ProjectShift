
#include "graphics/vulkan/queue.hpp"

VkQueue create_queue(VkDevice device, uint32_t queue_family_index,
                     uint32_t queue_index) {
    VkQueue queue;
    vkGetDeviceQueue(device, queue_family_index, queue_index, &queue);
    return queue;
}
