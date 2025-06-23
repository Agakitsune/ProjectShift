
#ifndef ALCHEMIST_EDITOR_SCENE_MANAGER_HPP
#define ALCHEMIST_EDITOR_SCENE_MANAGER_HPP

#include <unordered_map>
#include <memory>

#include "editor/global.hpp"
#include "editor/scene.hpp"
#include "editor/transition.hpp"

struct SceneManager {
    std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;

    Scene *current_scene = nullptr;
    std::unique_ptr<Transition> current_transition = nullptr;

    SceneManager() = default;
    ~SceneManager() = default;

    template<typename T, typename... Args>
    requires std::is_base_of<Scene, T>::value
    void add_scene(const std::string &name, Args&&... args) {
        scenes[name] = std::make_unique<T>(std::forward<Args>(args)...);
    }

    void set_current_scene(const std::string &name) {
        Scene *last_scene = current_scene; // Store the last scene for potential exit
        auto it = scenes.find(name);
        if (it != scenes.end()) {
            if (last_scene) {
                last_scene->exit(); // Exit the current scene if it exists
            }
            current_scene = it->second.get(); // Set the new current scene
            current_scene->in_editor = last_scene ? last_scene->in_editor : false; // Set editor mode flag
            current_scene->in_transition = !!current_transition; // Reset transition flag
            current_scene->enter(); // Enter the new scene
        } else {
            // Handle error: scene not found
        }
    }

    template<typename T, typename... Args>
    requires std::is_base_of<Transition, T>::value
    void transition_begin(Args&&... args) {
        if (current_scene) {
            current_scene->in_transition = true;
        }
        current_transition = std::make_unique<T>(std::forward<Args>(args)...);
        current_transition->in_editor = current_scene ? current_scene->in_editor : false;
        current_transition->enter();
        // current_transition->update(0.0f); // Start the transition
    }

    void transition_end() {
        if (current_transition) {
            // current_transition->update(0.0f); // End the transition
            current_transition->exit();
            current_transition = nullptr;
        }
        if (current_scene) {
            current_scene->in_transition = false;
        }
    }

    void update(float delta) {
        if (current_scene && !current_scene->in_editor) {
            current_scene->update(delta); // Update the current scene
        }
        if (current_transition && !current_transition->in_editor) {
            current_transition->update(delta); // Update the current transition
        }
    }

    void render() {
        Global &global = Global::instance();

        global.fences[global.flight_frame].wait();

        const Queue &graphic_queue = QueueServer::instance().get_queue(global.graphic_queue);
        const Queue &present_queue = QueueServer::instance().get_queue(global.present_queue);

        uint32_t image_index = 0;
        VkResult result = vkAcquireNextImageKHR(
            global.rendering_device.device,
            global.rendering_device.swapchain,
            UINT64_MAX,
            global.image_semaphores[global.flight_frame].semaphore,
            VK_NULL_HANDLE,
            &image_index
        );

        global.fences[global.flight_frame].reset(); // Reset the fence for the current frame
        global.command_buffers[global.flight_frame].reset(); // Reset the command buffer for the current frame

        global.command_buffers[global.flight_frame].begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        if (current_scene) {
            current_scene->render(global.command_buffers[global.flight_frame].buffer, image_index); // Render the current scene
        }
        if (current_transition) {
            current_transition->render(global.command_buffers[global.flight_frame].buffer); // Render the current transition
        }

        global.command_buffers[global.flight_frame].end(); // End the command buffer recording

        graphic_queue.submit()
            .add_command_buffer(global.command_buffers[global.flight_frame])
            .add_wait_semaphore(global.image_semaphores[global.flight_frame].semaphore, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
            .add_signal_semaphore(global.render_semaphores[global.flight_frame].semaphore)
            .submit(global.fences[global.flight_frame].fence);
        
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &global.render_semaphores[global.flight_frame].semaphore;

        VkSwapchainKHR swapChains[] = {global.rendering_device.swapchain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &image_index;

        result = vkQueuePresentKHR(present_queue.queue, &presentInfo);

        global.flight_frame = (global.flight_frame + 1) & 1; // Cycle to the next frame
    }

    void imgui() {
        if (current_scene) {
            current_scene->imgui(); // Render ImGui for the current scene
        }
    }
};

#endif // ALCHEMIST_EDITOR_SCENE_MANAGER_HPP
