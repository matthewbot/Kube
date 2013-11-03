#ifndef CAMERA_H
#define CAMERA_H

#include "util.h"
#include "Texture.h"
#include "Window.h"
#include "Shader.h"
#include "Mesh.h"
#include <glm/glm.hpp>
#include <boost/optional.hpp>
#include <initializer_list>

struct RPYCamera {
    glm::vec3 pos{0, 0, 0};
    Radians<float> roll = 0;
    Radians<float> pitch = 0;
    Radians<float> yaw = 0;

    glm::mat4 getView() const;
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
    boost::optional<DownState> mouse_down;
};

struct PerspectiveProjection {
    float fov = 90;
    float aspect = 1;
    float near = .01;
    float far = 1e3;

    glm::mat4 getProjection() const;
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
    
    glm::mat4 getProjection() const;
};

class Renderer {
public:
    Renderer();

    void setWindow(const Window &window) { this->window = &window; }

    const glm::mat4 &getProjection() const { return projection; }
    void setProjection(const glm::mat4 &projection);

    const glm::mat4 &getView() const { return view; }
    void setView(const glm::mat4 &view);

    template <typename T>
    void setProjection(const T &t) {
        setProjection(t.getProjection());
    }

    template <typename T>
    void setCamera(const T &t) {
        setView(t.getView());
    }

    void clearCamera();

    template <typename Tex>
    void setTexture(unsigned int pos, const Tex &tex, const Sampler &sampler) {
        glActiveTexture(GL_TEXTURE0 + pos);
        tex.bind();
        glBindSampler(pos, sampler.getID()); // TODO .bind()
    }

    void setProgram(ShaderProgram &prgm);
    void render(const glm::mat4 &model, const Mesh &mesh);

    std::pair<glm::vec3, glm::vec3> unproject(const glm::vec2 &windowpos);

private:
    const Window *window;
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 model;
    ShaderProgram *prgm;

    bool prgm_dirty;

    void setup_program(const glm::mat4 &model);
};

#endif
