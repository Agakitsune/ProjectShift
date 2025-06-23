
#ifdef ALCHEMIST_DEBUG
#include <iostream> // Include iostream for debug output
#endif

#include "editor/global.hpp"
#include "editor/server.hpp"

#include "server/gpu_memory.hpp"
#include "server/rid.hpp"
#include "server/image.hpp"
#include "server/buffer.hpp"
#include "server/render_pass.hpp"
#include "server/mesh.hpp"
#include "server/command_pool.hpp"
#include "server/queue.hpp"
#include "server/descriptor.hpp"
#include "server/pipeline.hpp"
#include "server/shader.hpp"
#include "server/framebuffer.hpp"

#include "vulkan/command_buffer.hpp" // Include the RID type definition
#include "vulkan/sync.hpp" // Include the RID type definition

#include "memory/misc.hpp" // Include the RID type definition

void Global::init(const ApplicationInfo &info) {
    window = info.window; // Set the GLFW window pointer
    rendering_device = std::move(RenderingDevice(info)); // Initialize the rendering device with the provided application info

    EditorServer &editor_server = EditorServer::instance();

    editor_server.emplace_server<RIDServer>();
    editor_server.emplace_server<GpuMemoryServer>(rendering_device.device, rendering_device.physical_device);
    editor_server.emplace_server<RenderPassServer>(rendering_device.device);
    editor_server.emplace_server<ImageServer>(rendering_device.device, rendering_device.physical_device);
    editor_server.emplace_server<ImageViewServer>(rendering_device.device);
    editor_server.emplace_server<SamplerServer>(rendering_device.device);
    editor_server.emplace_server<BufferServer>(rendering_device.device, rendering_device.physical_device);
    editor_server.emplace_server<MeshServer>(rendering_device.device, rendering_device.physical_device);
    editor_server.emplace_server<CommandPoolServer>(rendering_device.device);
    editor_server.emplace_server<QueueServer>(rendering_device.device);
    editor_server.emplace_server<DescriptorServer>(rendering_device.device);
    editor_server.emplace_server<DescriptorLayoutServer>(rendering_device.device);
    editor_server.emplace_server<DescriptorPoolServer>(rendering_device.device);
    editor_server.emplace_server<PipelineServer>(rendering_device.device);
    editor_server.emplace_server<ShaderServer>(rendering_device.device);
    editor_server.emplace_server<PipelineLayoutServer>(rendering_device.device);
    editor_server.emplace_server<FramebufferServer>(rendering_device.device);

    command_buffers.reserve(2);
    fences.reserve(2);
    image_semaphores.reserve(2);
    render_semaphores.reserve(2);
    
    framebuffer.resize(rendering_device.swapchain_image_count);

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
        // .set_aspect(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)
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
        .set_aspect_mask(VK_IMAGE_ASPECT_DEPTH_BIT)
        .build();
    
    desc_pool = DescriptorPoolServer::instance().new_descriptor_pool()
        .add_pool_size(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10)
        .add_pool_size(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10)
        .build();
    auto builder = DescriptorLayoutServer::instance().new_descriptor_layout();
    builder.add_binding()
        .set_binding(0)
        .set_stage_flags(VK_SHADER_STAGE_VERTEX_BIT)
        .set_descriptor_type(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
        .set_descriptor_count(1);
    builder.add_binding()
        .set_binding(1)
        .set_stage_flags(VK_SHADER_STAGE_VERTEX_BIT)
        .set_descriptor_type(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
        .set_descriptor_count(1);
    desc_layout = builder.build();
    desc = DescriptorServer::instance().new_descriptor(desc_pool, desc_layout);
    
    vert = ShaderServer::instance().from_file("../assets/shaders/line.vert.spv");
    frag = ShaderServer::instance().from_file("../assets/shaders/line.frag.spv");

    gizmo_pipeline_lyt = PipelineLayoutServer::instance().new_pipeline_layout().add_layout(desc_layout).build();

    render_pass = default_render_pass(rendering_device.surface_format.format, rendering_device.depth_format);
    
    auto pipeline_builder = PipelineServer::instance().new_simple_pipeline();
    pipeline_builder.add_shader(vert, VK_SHADER_STAGE_VERTEX_BIT);
    pipeline_builder.add_shader(frag, VK_SHADER_STAGE_FRAGMENT_BIT);
    pipeline_builder.set_vertex_input()
        .add_binding<vec3>(0)
        .add_binding<vec3>(1)
        .add_attribute<vec3>(0, 0, 0)
        .add_attribute<vec3>(1, 0, 0)
        .build();
    pipeline_builder.set_input_assembly(VK_PRIMITIVE_TOPOLOGY_LINE_LIST)
        .set_depth_stencil()
            .set_depth_test_enable(VK_TRUE)
            .set_depth_write_enable(VK_TRUE)
            .set_depth_compare_op(VK_COMPARE_OP_LESS)
            .set_depth_bounds_test_enable(VK_FALSE)
            .set_stencil_test_enable(VK_FALSE);
    gizmo_pipeline = pipeline_builder.set_layout(gizmo_pipeline_lyt).set_render_pass(render_pass).build();

    for (uint32_t i = 0; i < rendering_device.swapchain_image_count; ++i) {
        framebuffer[i] = FramebufferServer::instance().new_framebuffer(
                rendering_device.swapchain_extent.width,
                rendering_device.swapchain_extent.height,
                1
            )
            .set_render_pass(render_pass)
            .add_attachment(rendering_device.swapchain_image_views[i])
            .add_attachment(depth_view)
            .build();
    }

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

    #ifdef ALCHEMIST_DEBUG
    std::cout << "Global initialized with window: " << window << std::endl;
    #endif
}

Global::~Global() {
    command_buffers.clear();
    fences.clear();
    image_semaphores.clear();
    render_semaphores.clear();
}

Global &Global::instance() {
    if (!__instance) {
        __instance = std::make_unique<Global>();
    }
    return *__instance;
}

std::unique_ptr<Global> Global::__instance = nullptr;
