
#ifndef ALCHEMIST_EDITOR_GLOBAL_HPP
#define ALCHEMIST_EDITOR_GLOBAL_HPP

#include <memory>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include "editor/camera.hpp"

#include "server/rid.hpp"
#include "graphics/rendering_device.hpp"

#include "vulkan/sync.hpp"
#include "vulkan/command_buffer.hpp"

struct Global {
    EditorCamera camera = EditorCamera(vec3(1.0f, 1.0f, 1.0f), vec3(0.0f), 5.0f);

    bool middle_mouse_pressed = false;
    bool need_resize = false;

    GLFWwindow *window = nullptr; // Pointer to the GLFW window

    RenderingDevice rendering_device; // Pointer to the rendering device

    uint32_t flight_frame = 0;

    std::vector<CommandBuffer> command_buffers;
    std::vector<CommandBuffer> gui_command_buffers;
    std::vector<Fence> fences;
    std::vector<Semaphore> image_semaphores;
    std::vector<Semaphore> render_semaphores;

    RID command_pool;
    RID gui_command_pool;

    RID render_pass;
    RID gui_render_pass;

    RID desc_pool;
    RID desc_layout;
    RID desc;

    RID gui_desc_pool;
    RID gui_desc_layout;
    RID gui_desc;

    RID vert;
    RID frag;
    RID gizmo_pipeline_lyt;
    RID gizmo_pipeline;

    RID cube_vert;
    RID cube_frag;
    RID cube_pipeline_lyt;
    RID cube_pipeline;

    RID graphic_queue;
    RID present_queue;

    RID depth_image;
    RID depth_memory;
    RID depth_view;

    std::vector<RID> framebuffer;
    std::vector<RID> gui_framebuffer;

    Global() = default;
    void init(const ApplicationInfo &info);
    ~Global();

    static Global &instance();

    static std::unique_ptr<Global> __instance;
};

#endif // ALCHEMIST_EDITOR_GLOBAL_HPP
