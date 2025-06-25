
#ifndef ALCHEMIST_EDITOR_SCENE_HPP
#define ALCHEMIST_EDITOR_SCENE_HPP

#include <vulkan/vulkan.h>

struct Scene {
    bool in_transition = false; // Flag to indicate if the scene is in transition
    bool in_editor = false; // Flag to indicate if the scene is in editor mode

    virtual ~Scene() = default;

    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(VkCommandBuffer command_buffer, uint32_t image_index) = 0;
    virtual void imgui() = 0;
};

#endif // ALCHEMIST_EDITOR_SCENE_HPP
