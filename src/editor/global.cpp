
#ifdef ALCHEMIST_DEBUG
#include <iostream> // Include iostream for debug output
#endif

#include "editor/global.hpp"

#include "server/rid.hpp" // Include the RID type definition
#include "server/render_pass.hpp" // Include the RID type definition
#include "server/queue.hpp" // Include the CommandPoolServer and CommandPoolBuilder definitions
#include "server/command_pool.hpp" // Include the CommandPoolServer and CommandPoolBuilder definitions
#include "server/framebuffer.hpp" // Include the CommandPoolServer and CommandPoolBuilder definitions
#include "server/image.hpp" // Include the CommandPoolServer and CommandPoolBuilder definitions
#include "server/gpu_memory.hpp" // Include the CommandPoolServer and CommandPoolBuilder definitions

#include "vulkan/command_buffer.hpp" // Include the RID type definition
#include "vulkan/sync.hpp" // Include the RID type definition

#include "memory/misc.hpp" // Include the RID type definition

void Global::init(const ApplicationInfo &info) {
    window = info.window; // Set the GLFW window pointer
    rendering_device = RenderingDevice(info); // Initialize the rendering device with the provided application info

    command_buffers.resize(2);
    fences.resize(2);
    image_semaphores.resize(2);
    render_semaphores.resize(2);

    RID command_pool = CommandPoolServer::instance().new_command_pool()
        .set_flags(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
        .set_queue_family_index(rendering_device.graphics_queue_family_index)
        .build();
    
    emplace_command_buffer(command_buffers, 2, command_pool); // Allocate command buffers
    SemaphoreBuilder(rendering_device.device)
        .emplace(image_semaphores, 2); // Create image semaphores
    SemaphoreBuilder(rendering_device.device)
        .emplace(render_semaphores, 2); // Create render semaphores
    FenceBuilder(rendering_device.device)
        .signaled()
        .emplace(fences, 2); // Create fences

    render_pass = default_render_pass(rendering_device.surface_format.format, rendering_device.depth_format);
    #ifdef ALCHEMIST_DEBUG
    gui_render_pass = imgui_render_pass(rendering_device.surface_format.format);
    #endif

    graphic_queue = QueueServer::instance().new_queue(
        rendering_device.graphics_queue_family_index, 0
    );
    present_queue = QueueServer::instance().new_queue(
        rendering_device.present_queue_family_index, 0
    );

    depth_image = ImageServer::instance().new_image()
        .set_format(rendering_device.depth_format)
        .set_size(rendering_device.swapchain_extent.width,
                    rendering_device.swapchain_extent.height, 1)
        .set_usage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
        .set_aspect(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)
        .set_samples(VK_SAMPLE_COUNT_1_BIT)
        .build();
    
    VkMemoryRequirements depth_requirements;
    
    ImageServer::instance().get_requirements(depth_image, depth_requirements);
    depth_memory = GpuMemoryServer::instance().allocate_block<VkImage>(
        depth_requirements.size,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        find_memory_type(rendering_device.physical_device,
                         depth_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    );

    ImageServer::instance().bind_image(depth_image, depth_memory);

    depth_view = ImageViewServer::instance().new_image_view()
        .set_image(depth_image)
        .set_view_type(VK_IMAGE_VIEW_TYPE_2D)
        .set_format(rendering_device.depth_format)
        .set_aspect_mask(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)
        .build();

    framebuffer = FramebufferServer::instance().new_framebuffer(rendering_device.swapchain_extent.width,
                                                              rendering_device.swapchain_extent.height,
                                                              1)
        .set_render_pass(render_pass)
        .add_attachment(rendering_device.swapchain_image_views[0])
        .add_attachment(depth_view)
        .build();

    #ifdef ALCHEMIST_DEBUG
    std::cout << "Global initialized with window: " << window << std::endl;
    #endif
}

Global &Global::instance() {
    if (!current_scene) {
        current_scene = std::make_unique<Global>();
    }
    return *current_scene;
}

std::unique_ptr<Global> Global::current_scene = nullptr;
