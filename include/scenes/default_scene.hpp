
#ifndef ALCHEMIST_SCENES_DEFAULT_SCENE_HPP
#define ALCHEMIST_SCENES_DEFAULT_SCENE_HPP

#include "editor/scene.hpp"
#include "editor/global.hpp"

#include "server/render_pass.hpp"

#include "vulkan/render.hpp"

#include "math/quaternion.hpp"
#include "math/vector/vec3.hpp"
#include "math/matrix/mat4.hpp"
#include "math/matrix/transform.hpp"
#include "math/matrix/graphics.hpp"

#include "memory/misc.hpp"

struct CameraData {
    alignas(16) mat4 view; // View matrix
    alignas(16) mat4 projection; // Projection matrix
};

struct LineData {
    alignas(16) quaternion quat;
    alignas(16) vec3 root;
    alignas(4) float size;
};

struct DefaultScene : public Scene {
    DefaultScene() = default;

    virtual ~DefaultScene() override = default;

    RenderPassBegin render_pass_begin;

    RID gizmo;
    RID camera_ubo;
    RID gizmo_ubo;

    RID ubo_memory;

    void *ubo_data = nullptr;

    void enter() override {
        vec3 lines[] = {
            {0.0f, 0.0f, 0.0f}, // Position of the first vertex
            {1.0f, 0.0f, 0.0f}, // Position of the second vertex
            {0.0f, 0.0f, 0.0f}, // Position of the first vertex
            {0.0f, 1.0f, 0.0f}, // Position of the third vertex
            {0.0f, 0.0f, 0.0f}, // Position of the fourth vertex
            {0.0f, 0.0f, 1.0f}
        };

        vec3 color[] = {
            {1.0f, 0.0f, 0.0f}, // Position of the first vertex
            {1.0f, 0.0f, 0.0f}, // Position of the second vertex
            {0.0f, 1.0f, 0.0f}, // Position of the first vertex
            {0.0f, 1.0f, 0.0f}, // Position of the third vertex
            {0.0f, 0.0f, 1.0f}, // Position of the fourth vertex
            {0.0f, 0.0f, 1.0f}
        };

        LineData gizmos[] = {
            {quaternion(0.0f, 0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 0.0f), 1.0f},
            {quaternion(1.0f, 1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), 1.5f},
            {quaternion(2.0f, 2.0f, 2.0f, 1.0f), vec3(2.0f, 2.0f, 2.0f), 2.5f},
            {quaternion(3.0f, 3.0f, 3.0f, 1.0f), vec3(3.0f, 3.0f, 3.0f), 3.5f},
            {quaternion(4.0f, 4.0f, 4.0f, 1.0f), vec3(4.0f, 4.0f, 4.0f), 4.f},
            {quaternion(5.f, 5.f, 5.f, 1.f), vec3(5.f, 5.f, 5.f), .25}
        };

        BufferServer &buffer_server = BufferServer::instance();
        camera_ubo = buffer_server.new_buffer()
            .set_usage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) // Set usage to uniform buffer
            .set_size(sizeof(CameraData)) // Set size for camera data
            .set_sharing_mode(VK_SHARING_MODE_EXCLUSIVE) // Set sharing mode to exclusive
            .build(); // Build the buffer
        gizmo_ubo = buffer_server.new_buffer()
            .set_usage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) // Set usage to uniform buffer
            .set_size(sizeof(LineData) * 6) // Set size for 6 gizmos
            .set_sharing_mode(VK_SHARING_MODE_EXCLUSIVE) // Set sharing mode to exclusive
            .build(); // Build the buffer

        MeshServer &mesh_server = MeshServer::instance();
        gizmo = mesh_server.new_mesh()
            .add_data(lines, 6) // Add vertex data
            .add_data(color, 6) // Add color data
            .build(); // Build the mesh

        VkMemoryRequirements requirements;
        mesh_server.get_requirements(gizmo, requirements);

        GpuMemoryServer &gpu_memory_server = GpuMemoryServer::instance();
        RID memory = gpu_memory_server.allocate_block<VkBuffer>(
            requirements.size, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
            find_memory_type(
                Global::instance().rendering_device.physical_device,
                requirements.memoryTypeBits, 
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
            )
        );

        buffer_server.get_requirements(camera_ubo, requirements);
        VkMemoryRequirements requirements2;
        buffer_server.get_requirements(gizmo_ubo, requirements2);

        requirements.memoryTypeBits &= requirements2.memoryTypeBits; // Ensure memory type bits are compatible
        requirements.size += requirements2.size; // Combine sizes for both buffers

        ubo_memory = gpu_memory_server.allocate_block<VkBuffer>(
            sizeof(LineData) * 6 + sizeof(CameraData), // Size for 6 gizmos
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, // Memory properties
            find_memory_type(
                Global::instance().rendering_device.physical_device,
                requirements.memoryTypeBits, 
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            )
        );

        const Buffer &camera_ubo_buffer = buffer_server.get_buffer(camera_ubo);
        const Buffer &gizmo_ubo_buffer = buffer_server.get_buffer(gizmo_ubo);

        buffer_server.bind_buffer(camera_ubo, ubo_memory); // Bind the uniform buffer to the GPU memory
        buffer_server.bind_buffer(gizmo_ubo, ubo_memory); // Bind the uniform buffer to the GPU memory

        gpu_memory_server.map(ubo_memory, &ubo_data); // Map the uniform buffer to the CPU memory
        std::memcpy(ubo_data + sizeof(CameraData), gizmos, sizeof(LineData) * 6); // Copy gizmo data to the uniform buffer

        DescriptorServer &descriptor_server = DescriptorServer::instance();
        const Descriptor &desc = descriptor_server.get_descriptor(Global::instance().desc);
        auto write = descriptor_server.get_descriptor(desc.rid)
            .update();
        write.add_write()
            .set_binding(0) // Binding index for the uniform buffer
            .set_descriptor_type(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            .set_descriptor_count(1) // Number of descriptors
            .set_descriptor_set(desc.rid)
            .set_buffer_info(camera_ubo_buffer.buffer, 0, sizeof(CameraData)); // Sejt buffer info for the uniform buffer
        write.add_write()
            .set_binding(1) // Binding index for the uniform buffer
            .set_descriptor_type(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
            .set_descriptor_count(1) // Number of descriptors
            .set_descriptor_set(desc.rid)
            .set_buffer_info(gizmo_ubo_buffer.buffer, 0, sizeof(LineData) * 6); // Sejt buffer info for the uniform buffer
        write.update(); // Update the descriptor set with the new data

        mesh_server.bind_mesh(gizmo, memory); // Bind the mesh to the GPU memory
        
        Global &global = Global::instance();
        
        CommandBuffer cmd_buffer = allocate_command_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, global.command_pool);

        cmd_buffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT); // Begin the command buffer

        buffer_server.execute_commands(cmd_buffer.buffer); // Execute any pending buffer commands

        cmd_buffer.end(); // End the command buffer

        QueueServer::instance().get_queue(global.graphic_queue)
            .submit()
            .add_command_buffer(cmd_buffer)
            .submit().wait(); // Submit the command buffer to the graphics queue
    }

    void exit() override {
        // Clean up resources used by the default scene
    }

    void update(float delta_time) override {
        Global &global = Global::instance();

        // Update camera data
        CameraData *camera_data_ptr = static_cast<CameraData *>(ubo_data);
        camera_data_ptr->view = global.camera.compute_view();
        camera_data_ptr->projection = perspective(radians(75.0f), 
            static_cast<float>(global.rendering_device.swapchain_extent.width) / 
            static_cast<float>(global.rendering_device.swapchain_extent.height), 
            0.1f, 100.0f);
        
        camera_data_ptr->projection[1][1] *= -1.0f; // Invert Y-axis for Vulkan
    }

    void render(VkCommandBuffer command_buffer, uint32_t image_index) override {
        Global &global = Global::instance();

        uint32_t offset = 0;

        const RenderPass &pass = RenderPassServer::instance().get_render_pass(global.render_pass);
        const Mesh &gizmo_mesh = MeshServer::instance().get_mesh(gizmo);

        render_pass_begin = pass.begin(command_buffer);

        render_pass_begin
            .set_framebuffer(global.framebuffer[image_index])
            .set_render_offset({0, 0})
            .set_render_size(global.rendering_device.swapchain_extent)
            .add_clear_color(BLACK) // Clear color
            .add_clear_depth(1.0f); // Clear depth value

        render_pass_begin.begin(); // Begin the render pass

        bind_pipeline(command_buffer, global.gizmo_pipeline); // Bind the graphics pipeline

        VkRect2D scissor_rect = {
            {0, 0}, // Offset
            global.rendering_device.swapchain_extent // Extent
        };

        viewport(command_buffer, global.rendering_device.swapchain_extent); // Set the viewport
        scissor(command_buffer, scissor_rect); // Set the scissor rectangle

        gizmo_mesh.bind(command_buffer); // Bind the gizmo mesh

        bind_descriptor_sets(command_buffer, global.gizmo_pipeline_lyt, global.desc,
            VK_PIPELINE_BIND_POINT_GRAPHICS, &offset); // Bind the descriptor sets

        vkCmdDraw(command_buffer, 6, 1, 0, 0); // Draw the gizmo mesh

        render_pass_begin.end(); // End the render pass
    }

    void imgui() override {
        // Render ImGui elements for the default scene
    }
};

#endif // ALCHEMIST_SCENES_DEFAULT_SCENE_HPP
