#ifndef CAMERA_H
#define CAMERA_H

#include "util/math.h"
#include "util/Optional.h"
#include "gfx/Texture.h"
#include "gfx/Window.h"
#include "gfx/Shader.h"
#include "gfx/Mesh.h"

#include <glm/glm.hpp>
#include <initializer_list>

struct RPYCamera {
    glm::vec3 pos{0, 0, 0};
    Radians<float> roll = 0;
    Radians<float> pitch = 0;
    Radians<float> yaw = 0;

    glm::mat4 getMatrix() const;
};

class RPYCameraManipulator {
public:
    RPYCameraManipulator(float scale, float speed) :
        scale(scale), speed(speed) { }

    bool update(RPYCamera &camera, const Window &window, float dt);

private:
    float scale;
    float speed;

    struct DownState {
        glm::ivec2 pos;
        Radians<float> pitch;
        Radians<float> yaw;
    };
    Optional<DownState> mouse_down;
};

struct PerspectiveProjection {
    float fov = 90;
    float aspect = 1;
    float near = .01;
    float far = 1e3;

    glm::mat4 getMatrix() const;
};

struct OrthoProjection {
    float left = 0;
    float right = 0;
    float bottom = 0;
    float top = 0;
    float near = -1;
    float far = 1;

    OrthoProjection() { }
    OrthoProjection(float width, float height) : right(width), top(height) { }
    
    glm::mat4 getMatrix() const;
};

std::pair<glm::vec3, glm::vec3> unproject(const glm::mat4 &projection,
                                          const glm::mat4 &view,
                                          const glm::vec2 &window_pos);

#endif
