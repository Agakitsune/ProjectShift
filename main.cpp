#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#include "editor/camera.hpp"
#include "math/basis.hpp"
#include "math/matrix/graphics.hpp"
#include "math/matrix/mat4.hpp"
#include "math/matrix/transform.hpp"
#include "math/vector/vec3.hpp"

#include "graphics/color.hpp"
// #include "graphics/vulkan/base.hpp"
// #include "graphics/vulkan/buffer.hpp"
// #include "graphics/vulkan/command.hpp"
// #include "graphics/vulkan/command_buffer.hpp"
// #include "graphics/vulkan/descriptor_set.hpp"
// #include "graphics/vulkan/image.hpp"
// #include "graphics/vulkan/image_view.hpp"
// #include "graphics/vulkan/pipeline.hpp"
// #include "graphics/vulkan/queue.hpp"
// #include "graphics/vulkan/render_pass.hpp"
// #include "graphics/vulkan/renderer.hpp"
// #include "graphics/vulkan/sync.hpp"
// #include "graphics/vulkan/framebuffer.hpp"

#include "graphics/rendering_device.hpp"
#include "graphics/rendering_pass.hpp"
#include "graphics/meshes/mesh_server.hpp"
#include "graphics/framebuffer.hpp"
#include "graphics/descriptor.hpp"
#include "graphics/images/image_server.hpp"

#include "memory/gpu.hpp"
#include "memory/rid.hpp"
#include "memory/misc.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

struct Context {
    EditorCamera camera =
        EditorCamera(vec3(1.0f, 1.0f, 1.0f), vec3(0.0f), 5.0f);

    bool middle_mouse_pressed = false;
    bool need_resize = false;
};

struct LineData {
    quaternion orientation;
    vec3 position;
    float pad;
    vec3 color;
    float size;
};

void check_imgui_vulkan(VkResult result) {
    if (result != VK_SUCCESS) {
        std::cerr << "ImGui Vulkan error: " << result << std::endl;
        throw std::runtime_error("ImGui Vulkan initialization failed");
    }
}

void framebuffer_resize_callback(GLFWwindow *window, int width, int height) {
    Context *ctx = static_cast<Context *>(glfwGetWindowUserPointer(window));
    ctx->need_resize = true;

    // Resize the swapchain or handle the resize event as needed
    // This is where you would typically recreate the swapchain with the new
    // dimensions For now, we just print the new dimensions
    std::cout << "Framebuffer resized to: " << width << "x" << height
              << std::endl;
}

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
    Context *ctx = static_cast<Context *>(glfwGetWindowUserPointer(window));
    ctx->middle_mouse_pressed =
        (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS);
}

void mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    Context *ctx = static_cast<Context *>(glfwGetWindowUserPointer(window));
    ctx->camera.zoom(static_cast<float>(yoffset) * 0.1f);
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    Context *ctx = static_cast<Context *>(glfwGetWindowUserPointer(window));
    static double last_x = xpos;
    static double last_y = ypos;

    if (ctx->need_resize) {
        // Reset last_x and last_y if the window was resized
        last_x = xpos;
        last_y = ypos;
        return;
    }

    vec2 mouse_delta(static_cast<float>(xpos - last_x),
                     static_cast<float>(ypos - last_y));
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        ctx->camera.pan(mouse_delta, 0.01f);
    } else {
        ctx->camera.turn(mouse_delta, 0.01f);
    }

    last_x = xpos;
    last_y = ypos;
}

int main() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    Context ctx;

    GLFWwindow *window =
        glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    glfwSetWindowUserPointer(window, &ctx);
    glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    ApplicationInfo info{
        window,
        "Field of Grass",
        "Alchemist",
        VK_MAKE_API_VERSION(0, 1, 0, 0),
        VK_MAKE_API_VERSION(0, 1, 0, 0),
        QueueFamilyPreferences::QUEUE_FAMILY_PREFERENCES_SEPARATE
    };

    auto &rid_sever = new_rid_server();
    auto &atlas = new_rid_atlas();

    auto &device = new_rendering_device(info);
    auto &server = new_gpu_memory_server(device.device, device.physical_device);
    auto &image_server = new_image_server(device);

    // RID rid = server.allocate_block<VkBuffer>(
    //     1024 * 1024, // 1 MB buffer
    //     VK_MEMORY_HEAP_DEVICE_LOCAL_BIT,
    //     device.physical_device.memory_properties.memoryTypeCount - 1
    // );

    RID rid = load_image("../assets/textures/oh.png");
    VkMemoryRequirements requirements;
    image_server.get_requirements(rid, requirements);
    RID block = server.allocate_block<VkImage>(
        requirements.size,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        find_memory_type(
            device.physical_device,
            requirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        )
    );

    image_server.bind_image(rid, block);
    
    // vec3 a[] = {
    //     vec3(0.0f, 0.0f, 0.0f),
    //     vec3(1.0f, 0.0f, 0.0f),
    //     vec3(1.0f, 1.0f, 0.0f),
    //     vec3(0.0f, 1.0f, 0.0f)
    // };
    // uint8_t indices[] = {0, 1, 2, 3};
    // auto &mesh_server = new_mesh_server(device);
    // mesh_server.add_table<vec3>();
    // auto builder = mesh_server.mesh();
    // builder.add_data(
    //     a,
    //     4
    // );
    // std::cout << "Uploading mesh data to GPU..." << std::endl;
    // builder.add_indices<uint8_t>(
    //     indices, 4
    // );
    // std::cout << "Uploading mesh data to GPU..." << std::endl;
    // Mesh mesh = builder.build();

    VkCommandPool command_pool;
    VkCommandPoolCreateInfo pool_info;
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = device.graphics_queue_family_index;
    pool_info.pNext = nullptr;
    if (vkCreateCommandPool(device.device, &pool_info, nullptr, &command_pool) != VK_SUCCESS) {
        return 1;
    }

    VkCommandBuffer cmd_buffer;
    VkCommandBufferAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };

    if (vkAllocateCommandBuffers(device.device, &alloc_info, &cmd_buffer) != VK_SUCCESS) {
        std::cerr << "Failed to allocate command buffer!" << std::endl;
        return 1;
    }
    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };
    if (vkBeginCommandBuffer(cmd_buffer, &begin_info) != VK_SUCCESS) {
        std::cerr << "Failed to begin command buffer!" << std::endl;
        return 1;
    }

    // mesh_server.cmd_upload_gpu_data(cmd_buffer, device);
    image_server.execute_commands(cmd_buffer);
    vkEndCommandBuffer(cmd_buffer);
    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &cmd_buffer
    };
    vkQueueSubmit(
        device.graphics_queue,
        1,
        &submit_info,
        VK_NULL_HANDLE
    );
    vkQueueWaitIdle(device.graphics_queue);
    image_server.clear_commands();

    VkRenderPass render_pass = default_render_pass(device);
    #ifdef ALCHEMIST_DEBUG
    VkRenderPass imgui_rendering_pass = imgui_render_pass(device);
    #endif // ALCHEMIST_DEBUG

    VkFramebuffer framebuffer = FramebufferBuilder(device.swapchain_extent).add_attachment(
        device.swapchain_image_views[0]
    ).set_render_pass(render_pass).build(device.device);

    VkDescriptorPool descriptor_pool = DescriptorPoolBuilder(1)
        .add_pool_size(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1)
        .set_max_sets(1)
        .build(device.device);
    
    auto desc_builder = DescriptorLayoutBuilder(1);
    desc_builder
        .add_binding()
            .set_binding(0)
            .set_descriptor_type(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            .set_descriptor_count(1)
            .set_stage_flags(VK_SHADER_STAGE_VERTEX_BIT);
    VkDescriptorSetLayout descriptor_set_layout = desc_builder.build(device.device);
    VkDescriptorSet descriptor_set = DescriptorAllocation(descriptor_pool, descriptor_set_layout)
        .allocate(device.device);

    // VulkanBase base;
    // vulkan_base_init(
    //     base, window,
    //     {"Field of Grass", "Alchemist", VK_MAKE_API_VERSION(0, 1, 0, 0),
    //      VK_MAKE_API_VERSION(0, 1, 0, 0),
    //      QueueFamilyPreferences::QUEUE_FAMILY_PREFERENCES_SEPARATE});

    // VkQueue transfer_queue =
    //     create_queue(base.device, base.physical_device.indices.transfer);

    // vec3 vertices[] = {
    //     vec3(0.0f, 0.0f, 0.0f),
    //     vec3(1.0f, 0.0f, 0.0f),
    // };

    // mat4 view_matrix =
    //     raw_look_at(ctx.camera.position, ctx.camera.orientation.z,
    //                 ctx.camera.orientation.y, ctx.camera.orientation.x);
    // mat4 projection_matrix =
    //     perspective(45.0f,
    //                 static_cast<float>(base.swapchain.extent.width) /
    //                     static_cast<float>(base.swapchain.extent.height),
    //                 0.1f, 100.0f);
    // projection_matrix[1][1] *= -1.0f; // Invert Y-axis for Vulkan

    // LineData line_data = {.orientation = quaternion(1.0f, 0.0f, 0.0f, 0.0f),
    //                       .position = vec3(0.0f, 0.0f, 0.0f),
    //                       .color = vec3(1.0f, 1.0f, 1.0f),
    //                       .size = 10.0f};

    // VkBuffer vertex_buffer =
    //     create_local_vertex_buffer(base.device, sizeof(vertices));
    // VkBuffer uniform_buffer =
    //     create_buffer(base.device, sizeof(mat4) * 2 + sizeof(LineData),
    //                   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    // VkBuffer staging_buffer =
    //     create_staging_buffer(base.device, sizeof(vertices));
    // VkDeviceMemory vertex_memory = allocate_buffer_memory(
    //     base.physical_device.device, base.device, vertex_buffer,
    //     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    // VkDeviceMemory uniform_memory = allocate_buffer_memory(
    //     base.physical_device.device, base.device, uniform_buffer,
    //     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
    //         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    // VkDeviceMemory staging_memory = allocate_buffer_memory(
    //     base.physical_device.device, base.device, staging_buffer,
    //     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
    //         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // void *data = map_memory(base.device, staging_memory, sizeof(vertices));
    // std::memcpy(data, vertices, sizeof(vertices));
    // unmap_memory(base.device, staging_memory);

    // void *uniform_data = map_memory(base.device, uniform_memory,
    //                                 sizeof(mat4) * 2 + sizeof(LineData));
    // std::memcpy(uniform_data, &view_matrix, sizeof(mat4));
    // std::memcpy(static_cast<char *>(uniform_data) + sizeof(mat4),
    //             &projection_matrix, sizeof(mat4));
    // std::memcpy(static_cast<char *>(uniform_data) + sizeof(mat4) * 2,
    //             &line_data, sizeof(LineData));
    // // unmap_memory(base.device, uniform_memory);

    // VkFence transfer_fence = create_fence(base.device);

    // vkResetFences(base.device, 1, &transfer_fence);

    // VkCommandPool transfer_pool =
    //     create_command_pool(base.device, base.physical_device.indices.transfer);
    // VkCommandBuffer command_buffer = create_command_buffer(
    //     base.device, transfer_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    // begin_command_buffer(command_buffer,
    //                      VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    // copy_buffer(
    //     command_buffer, staging_buffer, vertex_buffer,
    //     VkBufferCopy{.srcOffset = 0, .dstOffset = 0, .size = sizeof(vertices)});

    // end_command_buffer(command_buffer);
    // submit_command_buffer(transfer_queue,
    //                       SubmitInfo().add_command_buffer(command_buffer),
    //                       transfer_fence);

    // vkWaitForFences(base.device, 1, &transfer_fence, VK_TRUE,
    //                 std::numeric_limits<uint64_t>::max());

    // VkImage depth_image = create_image(
    //     base.device,
    //     ImageCreateInfo()
    //         .set_size_2d(base.swapchain.extent.width,
    //                      base.swapchain.extent.height)
    //         .set_format(base.depth_format)
    //         .set_usage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT));
    // VkDeviceMemory depth_memory =
    //     allocate_image_memory(base.physical_device.device, base.device,
    //                           depth_image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    // VkImageView depth_image_view = create_image_view(
    //     base.device, depth_image, base.depth_format,
    //     ImageViewCreateInfo().aspect(VK_IMAGE_ASPECT_DEPTH_BIT));

    // VkRenderPass render_pass = create_render_pass(
    //     base.device,
    //     RenderPassInfo()
    //         .add_attachment(AttachmentDescription(
    //             ATTACHMENT_TYPE_COLOR, base.swapchain.format.format,
    //             VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    //             VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL))
    //         .add_attachment(AttachmentDescription(
    //             ATTACHMENT_TYPE_DEPTH_STENCIL, base.depth_format,
    //             VK_IMAGE_LAYOUT_UNDEFINED,
    //             VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    //             VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL))
    //         .add_subpass_description(
    //             SubpassDescription(VK_PIPELINE_BIND_POINT_GRAPHICS)
    //                 .add_color_attachment(0)
    //                 .set_depth_stencil_attachment(1))
    //         .add_subpass_dependecy(VkSubpassDependency{
    //             .srcSubpass = VK_SUBPASS_EXTERNAL,
    //             .dstSubpass = 0,
    //             .srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
    //             .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
    //                             VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
    //             .srcAccessMask = 0,
    //             .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
    //                              VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
    //             .dependencyFlags = 0}));
    
    // VkRenderPass imgui_render_pass = create_render_pass(
    //         base.device,
    //         RenderPassInfo()
    //             .add_attachment(AttachmentDescription(
    //                 ATTACHMENT_TYPE_COLOR, base.swapchain.format.format,
    //                 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    //                 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    //                 .set_load_op(VK_ATTACHMENT_LOAD_OP_LOAD)
    //             )
    //             .add_subpass_description(
    //                 SubpassDescription(VK_PIPELINE_BIND_POINT_GRAPHICS)
    //                     .add_color_attachment(0))
    //             .add_subpass_dependecy(VkSubpassDependency{
    //                 .srcSubpass = VK_SUBPASS_EXTERNAL,
    //                 .dstSubpass = 0,
    //                 .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    //                 .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    //                 .srcAccessMask = 0,
    //                 .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    //                 .dependencyFlags = 0}));

    // DescriptorSetBinding bindings[] = {
    //     DescriptorSetBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    //                          VK_SHADER_STAGE_VERTEX_BIT, 1),
    //     DescriptorSetBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    //                          VK_SHADER_STAGE_VERTEX_BIT, 1),
    // };
    // // imgui descriptor pool
    // VkDescriptorPoolSize pool_sizes[] =
    // {
    //     { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
    //     { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
    //     { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
    //     { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
    //     { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
    //     { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
    //     { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
    //     { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
    //     { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
    //     { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
    //     { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    // };
    // VkDescriptorPoolCreateInfo pool_info{};
    // pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    // pool_info.poolSizeCount = 11;
    // pool_info.pPoolSizes = pool_sizes;
    // pool_info.maxSets = 1; // Maximum number of descriptor sets that can
    //                              // be allocated from this pool
    // pool_info.flags =
    //     VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    // VkDescriptorPool imgui_descriptor_pool;
    // if (vkCreateDescriptorPool(base.device, &pool_info, nullptr,
    //                            &imgui_descriptor_pool) != VK_SUCCESS) {
    //     throw std::runtime_error("Failed to create descriptor pool for ImGui");
    // }

    // VkDescriptorSetLayout descriptor_set_layout =
    //     create_descriptor_set_layout(base.device, bindings, 2);
    // VkDescriptorPool descriptor_pool =
    //     create_descriptor_pool(base.device, bindings, 2, 2);
    // VkDescriptorSet *descriptor_sets = allocate_descriptor_sets(
    //     base.device, descriptor_pool, descriptor_set_layout, 2);

    // VkDescriptorBufferInfo uniform_buffer_info = {
    //     .buffer = uniform_buffer, .offset = 0, .range = sizeof(mat4) * 2};
    // VkDescriptorBufferInfo line_data_info = {.buffer = uniform_buffer,
    //                                          .offset = sizeof(mat4) * 2,
    //                                          .range = sizeof(LineData)};
    // update_descriptor_sets(
    //     base.device, UpdateDescriptorSets()
    //                      .add_set(descriptor_sets[0])
    //                      .add_set(descriptor_sets[1])
    //                      .emplace_buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0,
    //                                      uniform_buffer_info)
    //                      .emplace_buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
    //                                      line_data_info));

    // VkPipelineLayout pipeline_layout = create_pipeline_layout(
    //     base.device,
    //     PipelineLayoutInfo().add_set_layout(descriptor_set_layout));
    // VkPipeline graphics_pipeline = create_graphics_pipeline(
    //     base.device, pipeline_layout, render_pass,
    //     GraphicsPipelineInfo(VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
    //                          VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE,
    //                          VK_FRONT_FACE_COUNTER_CLOCKWISE)
    //         .add_shader_module(ShaderModule::from_file(
    //             base.device, "../assets/shaders/line.vert.spv",
    //             VK_SHADER_STAGE_VERTEX_BIT))
    //         .add_shader_module(ShaderModule::from_file(
    //             base.device, "../assets/shaders/line.frag.spv",
    //             VK_SHADER_STAGE_FRAGMENT_BIT))
    //         .set_vertex_input(
    //             VertexInput()
    //                 .add_binding(0, sizeof(vec3), VK_VERTEX_INPUT_RATE_VERTEX)
    //                 .add_attribute<vec3>(0, 0, 0))
    //         .set_depth_testing(true)
    //         .set_depth_write(true)
    //         .set_depth_compare_op(VK_COMPARE_OP_LESS));

    // VkCommandPool command_pool =
    //     create_command_pool(base.device, base.physical_device.indices.graphics);
    // VkCommandPool imgui_command_pool =
    //     create_command_pool(base.device, base.physical_device.indices.graphics);
    // VkQueue graphics_queue =
    //     create_queue(base.device, base.physical_device.indices.graphics);
    // VkQueue present_queue =
    //     create_queue(base.device, base.physical_device.indices.present);

    // VkCommandBuffer *imgui_command_buffers;
    // imgui_command_buffers = new VkCommandBuffer[base.swapchain.image_count];
    // for (uint32_t i = 0; i < base.swapchain.image_count; ++i) {
    //     imgui_command_buffers[i] = create_command_buffer(
    //         base.device, command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    // }

    // VkFramebuffer *imgui_framebuffers =
    //     new VkFramebuffer[base.swapchain.image_count];
    // for (uint32_t i = 0; i < base.swapchain.image_count; ++i) {
    //     imgui_framebuffers[i] = create_framebuffer(base.device, FramebufferCreateInfo(imgui_render_pass, base.swapchain.extent.width,
    //                                                                          base.swapchain.extent.height)
    //                                             .add_attachment(base.swapchain_image_views[i])
    //     );
    // }

    // Renderer renderer;
    // create_renderer(
    //     renderer, base.device,
    //     RendererCreateInfo()
    //         .set_render_pass(render_pass)
    //         .set_swapchain_info(SwapchainInfo()
    //                                 .set_images(base.swapchain.image_count,
    //                                             base.swapchain_image_views)
    //                                 .set_depth_image_view(depth_image_view)
    //                                 .set_extent(base.swapchain.extent))
    //         .set_command_pool(command_pool));
    
    // ImGui::CreateContext();
    // ImGui::StyleColorsDark();
    // ImGuiIO &io = ImGui::GetIO();
    // ImGui_ImplGlfw_InitForVulkan(window, true);

    // ImGui_ImplVulkan_InitInfo init_info = {};
    // init_info.Instance = base.instance;
    // init_info.PhysicalDevice = base.physical_device.device;
    // init_info.Device = base.device;
    // init_info.QueueFamily = base.physical_device.indices.graphics;
    // init_info.Queue = graphics_queue;
    // init_info.PipelineCache = VK_NULL_HANDLE;
    // init_info.DescriptorPool = imgui_descriptor_pool;
    // init_info.Allocator = nullptr;
    // init_info.MinImageCount = base.swapchain.image_count;
    // init_info.ImageCount = base.swapchain.image_count;
    // init_info.CheckVkResultFn = check_imgui_vulkan;
    // init_info.RenderPass = imgui_render_pass;
    // ImGui_ImplVulkan_Init(&init_info);

    // ImGui_ImplVulkan_CreateFontsTexture();

    // uint32_t image_index = 0;
    // while (!glfwWindowShouldClose(window)) {
    //     glfwPollEvents();

    //     begin_draw(renderer, base.device, base.swapchain.swapchain,
    //                image_index);

    //     begin_command_buffer(
    //         renderer.command_buffers.data[renderer.current_frame], 0);

    //     begin_render_pass(
    //         renderer.command_buffers.data[renderer.current_frame],
    //         RenderPassBeginInfo()
    //             .set_render_pass(render_pass)
    //             .set_framebuffer(renderer.framebuffers[image_index])
    //             .set_extent(base.swapchain.extent)
    //             .set_clear_flags(CLEAR_COLOR_DEPTH_STENCIL)
    //             .set_clear_color(BLACK)
    //             .set_clear_depth(1.0f));

    //     bind_pipeline(renderer.command_buffers.data[renderer.current_frame],
    //                   graphics_pipeline);

    //     set_viewport(
    //         renderer.command_buffers.data[renderer.current_frame],
    //         Viewport{vec2(0.0f, 0.0f),
    //                  vec2(static_cast<float>(base.swapchain.extent.width),
    //                       static_cast<float>(base.swapchain.extent.height)),
    //                  0.0f, 1.0f});
    //     set_scissor(renderer.command_buffers.data[renderer.current_frame],
    //                 Scissor{0, 0, base.swapchain.extent.width,
    //                         base.swapchain.extent.height});

    //     bind_vertex_buffers(
    //         renderer.command_buffers.data[renderer.current_frame],
    //         BindVertexBuffers().add_buffer(vertex_buffer, 0));

    //     bind_descriptor_sets(
    //         renderer.command_buffers.data[renderer.current_frame],
    //         BindDescriptorSets()
    //             .add_set(descriptor_sets[renderer.current_frame])
    //             .set_bind_point(VK_PIPELINE_BIND_POINT_GRAPHICS)
    //             .set_pipeline_layout(pipeline_layout));

    //     draw(renderer.command_buffers.data[renderer.current_frame], 2);

    //     end_render_pass(renderer.command_buffers.data[renderer.current_frame]);

    //     end_command_buffer(
    //         renderer.command_buffers.data[renderer.current_frame]);
        
    //     ImGui_ImplVulkan_NewFrame();
    //     ImGui_ImplGlfw_NewFrame();
    //     ImGui::NewFrame();
    //     ImGui::ShowDemoWindow();
    //     ImGui::Render();

    //     ImDrawData* main_draw_data = ImGui::GetDrawData();
    //     const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);

    //     begin_command_buffer(
    //         imgui_command_buffers[image_index], 0);

    //     begin_render_pass(
    //         imgui_command_buffers[image_index],
    //         RenderPassBeginInfo()
    //             .set_render_pass(imgui_render_pass)
    //             .set_framebuffer(imgui_framebuffers[image_index])
    //             .set_extent(base.swapchain.extent)
    //             .set_clear_flags(CLEAR_COLOR)
    //             .set_clear_color(BLACK)
    //     );

    //     ImGui_ImplVulkan_RenderDrawData(
    //         main_draw_data, imgui_command_buffers[image_index]);

    //     end_render_pass(imgui_command_buffers[image_index]);
    //     end_command_buffer(imgui_command_buffers[image_index]);

    //     submit_command_buffer(graphics_queue,
    //         SubmitInfo()
    //         .add_command_buffer(renderer.command_buffers.data[renderer.current_frame])
    //         .add_command_buffer(imgui_command_buffers[image_index])
    //         .add_wait_semaphore(renderer.image_available_semaphores.data[renderer.current_frame],
    //                             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
    //         .add_signal_semaphore(renderer.render_finished_semaphores.data[renderer.current_frame]),
    //         renderer.in_flight_fences.data[renderer.current_frame]);

    //     // submit_draw(renderer, graphics_queue);

    //     present_draw(renderer, present_queue, base.swapchain.swapchain,
    //                  image_index);
    //     renderer.current_frame = (renderer.current_frame + 1) % 2;
    // }

    delete &image_server;
    // delete &mesh_server;
    delete &server;
    delete &device;
    delete &rid_sever;
    delete &atlas;

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
