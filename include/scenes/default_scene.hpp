
#ifndef ALCHEMIST_SCENES_DEFAULT_SCENE_HPP
#define ALCHEMIST_SCENES_DEFAULT_SCENE_HPP

#include "editor/scene.hpp"
#include "editor/global.hpp"

#include "server/render_pass.hpp"

struct DefaultScene : public Scene {
    DefaultScene() = default;

    RenderPassBegin render_pass_begin;

    void enter() override {
        // Initialize the default scene
        // This could include setting up resources, loading assets, etc.
    }

    void exit() override {
        // Clean up resources used by the default scene
    }

    void update(float delta_time) override {
        // Update logic for the default scene
    }

    void render(VkCommandBuffer command_buffer) override {
        Global &global = Global::instance();

        if (render_pass_begin.cmd_buffer == VK_NULL_HANDLE) {
            const RenderPass &pass = RenderPassServer::instance().get_render_pass(global.render_pass);

            render_pass_begin = pass.begin(command_buffer);

            render_pass_begin
                .set_framebuffer(global.render_pass)
                .set_render_offset({0, 0})
                .set_render_size(global.rendering_device.swapchain_extent)
                .add_clear_color({0.0f, 0.0f, 0.0f, 1.0f}) // Clear color
                .add_clear_depth(1.0f); // Clear depth value
        }
    }

    void imgui() override {
        // Render ImGui elements for the default scene
    }
};

#endif // ALCHEMIST_SCENES_DEFAULT_SCENE_HPP
