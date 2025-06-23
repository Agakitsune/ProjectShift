
#ifndef ALCHEMIST_EDITOR_TRANSITION_HPP
#define ALCHEMIST_EDITOR_TRANSITION_HPP

#include <vulkan/vulkan.h>

struct Transition {
    bool in_editor = false; // Flag to indicate if the scene is in editor mode

    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(VkCommandBuffer command_buffer) = 0;
};

#endif // ALCHEMIST_EDITOR_TRANSITION_HPP
