#ifndef ALCHEMIST_EDITOR_CAMERA_H
#define ALCHEMIST_EDITOR_CAMERA_H

#include "math/basis.hpp"
#include "math/matrix/graphics.hpp"
#include "math/matrix/mat4.hpp"
#include "math/quaternion.hpp"
#include "math/vector/vec3.hpp"

struct EditorCamera {
    basis orientation;
    vec3 position;
    float distance;

    EditorCamera(const vec3 &pos = {1.0f, 0.0f, 0.0f},
                 const vec3 &target = {0.0f, 0.0f, 0.0f}, float dist = 1.0f) {
        vec3 f = (target - pos).normalize();
        vec3 s = f.cross(vec3(0.0f, 1.0f, 0.0f)).normalize();
        vec3 u = s.cross(f);
        distance = dist;

        if (fabsf(f.dot(vec3(0.0f, 1.0f, 0.0f))) > 0.999f) {
            // up and f are almost parallel, use a different up vector
            s = f.cross(vec3(0.0f, 0.0f, 1.0f)).normalize();
            u = s.cross(f).normalize();
        }

        position = target - f * distance;
        orientation = {
            s,
            u, // y-axis
            -f // z-axis
        };
    }

    void zoom(float delta) {
        vec3 target = position - orientation.z * distance;
        distance += delta;
        if (distance < 0.1f) {
            distance = 0.1f; // Prevent zooming too close
        }
        position = target + orientation.z * distance;
    }

    void pan(const vec2 &mouse_delta, float sensitivity) {
        position -= orientation.x * mouse_delta.x * sensitivity;
        position += orientation.y * mouse_delta.y * sensitivity;
    }

    void turn(const vec2 &mouse_delta, float sensitivity) {
        vec3 target = position - orientation.z * distance;
        vec3 x = orientation.x;
        vec3 z = orientation.z;
        quaternion up = quaternion::from_axis(vec3(0.0f, 1.0f, 0.0f),
                                              mouse_delta.x * -0.005f);
        x.y = 0.0f;
        x = up.rotate(x);
        quaternion right = quaternion::from_axis(x, mouse_delta.y * -0.005f);

        orientation.x = x;
        // camera.y = q.rotate(camera.y);
        orientation.z = right.rotate(up.rotate(z));
        orientation.y = orientation.z.cross(orientation.x).normalize();
        position = target + orientation.z * distance;
    }

    mat4 compute_view() const {
        return raw_look_at(position, orientation.z, orientation.y,
                           orientation.x);
    }
};

#endif // ALCHEMIST_EDITOR_CAMERA_H
