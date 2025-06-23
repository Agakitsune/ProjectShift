
#ifndef ALCHEMIST_EDITOR_GLOBAL_HPP
#define ALCHEMIST_EDITOR_GLOBAL_HPP

#include <memory>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include "server/rid.hpp"
#include "graphics/rendering_device.hpp"

#include "vulkan/sync.hpp"
#include "vulkan/command_buffer.hpp"

struct Global {
    GLFWwindow *window = nullptr; // Pointer to the GLFW window

    RenderingDevice rendering_device; // Pointer to the rendering device

    std::vector<CommandBuffer> command_buffers;
    std::vector<Fence> fences;
    std::vector<Semaphore> image_semaphores;
    std::vector<Semaphore> render_semaphores;

    RID command_pool;

    RID render_pass;
    RID imgui_render_pass;

    RID pipeline_layout;
    RID pipeline;

    RID graphic_queue;
    RID present_queue;

    RID depth_image;
    RID depth_memory;
    RID depth_view;

    RID framebuffer;

    Global() = default;
    void init(const ApplicationInfo &info);

    static Global &instance();

    static std::unique_ptr<Global> current_scene;
};

#endif // ALCHEMIST_EDITOR_GLOBAL_HPP
