
#ifndef ALCHEMIST_GRAPHICS_MESH_HPP
#define ALCHEMIST_GRAPHICS_MESH_HPP

#include <vulkan/vulkan.h>

#include "math/vector/vec3.hpp"

#include "memory/slice.hpp"

// Slices over the CPU data for the mesh, last slice is the index buffer if it exists
struct Mesh {
    Slice *slices = nullptr;

    uint32_t count = 0; // Number of slices in the mesh

    ~Mesh() {
        if (slices) {
            delete[] slices; // Free the slices array
        }
    }
};

#endif // ALCHEMIST_GRAPHICS_MESH_HPP
