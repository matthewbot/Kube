#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

static constexpr float pi = static_cast<float>(M_PI);

glm::mat4 RPYCamera::getView() const {
    glm::mat4 view;
    view = glm::rotate(view, -90.0f, glm::vec3{1, 0, 0});
    view = glm::rotate(view, -pitch/pi*180, glm::vec3{1, 0, 0});
    view = glm::rotate(view, -yaw/pi*180, glm::vec3{0, 0, 1});
    view = glm::translate(view, glm::vec3(-pos));
    return view;
}

glm::mat4 PerspectiveProjection::getProjection() const {
    return glm::perspective(fov/2, aspect, near, far);
}

glm::mat4 OrthoProjection::getProjection() const {
    return glm::ortho(left, right, bottom, top, near, far);
}

bool RPYCameraManipulator::update(RPYCamera &camera, const Window &window, float dt) {
    bool updated = false;

    bool new_mouse_down = window.isMousePressed(MouseButton::LEFT);
    glm::ivec2 new_mouse_pos = window.getMousePos();

    if (!mouse_down) {
        if (new_mouse_down) {
            mouse_down = { new_mouse_pos, camera.pitch, camera.yaw };
        }
    } else {
        if (new_mouse_down) {
            camera.yaw = mouse_down->yaw - (new_mouse_pos.x - mouse_down->pos.x)*scale;
            camera.pitch = mouse_down->pitch - (new_mouse_pos.y - mouse_down->pos.y)*scale;
            updated = true;
        } else {
            mouse_down = boost::none;
        }
    }

    glm::vec3 vel{0, 0, 0};

    float modspeed = speed;
    if (window.isShiftPressed())
        modspeed *= 5;
    
    if (window.isKeyPressed('w'))
        vel.y = modspeed;
    if (window.isKeyPressed('a'))
        vel.x = -modspeed;
    if (window.isKeyPressed('s'))
        vel.y = -modspeed;
    if (window.isKeyPressed('d'))
        vel.x = modspeed;
    if (window.isKeyPressed('1'))
        glEnable(GL_FRAMEBUFFER_SRGB);
    if (window.isKeyPressed('2'))
        glDisable(GL_FRAMEBUFFER_SRGB);

    vel = glm::vec3(glm::rotate(glm::mat4(), camera.yaw/pi*180, glm::vec3{0, 0, 1}) *
                    glm::vec4(vel, 0));

    if (window.isKeyPressed('e'))
        vel.z = modspeed;
    if (window.isKeyPressed('c'))
        vel.z = -modspeed;

    camera.pos += dt*vel;
    if (vel != glm::vec3{0, 0, 0})
        updated = true;

    return updated;
}

Renderer::Renderer() :
    view{1.0},
    prgm(nullptr),
    prgm_dirty(false) { }

void Renderer::setProjection(const glm::mat4 &projection) {
    this->projection = projection;
    prgm_dirty = true;
}

void Renderer::setView(const glm::mat4 &view) {
    this->view = view;
    prgm_dirty = true;
}

void Renderer::clearCamera() {
    view = glm::mat4{1};
}

void Renderer::setProgram(ShaderProgram &prgm) {
    this->prgm = &prgm;
    prgm_dirty = true;
}

void Renderer::render(const glm::mat4 &model, const Mesh &mesh) {
    setup_program(model);
    mesh.draw();
}

std::pair<glm::vec3, glm::vec3> Renderer::unproject(const glm::vec2 &window_pos) {
    glm::vec4 ndc_pos_near{window_pos, 0, 1};
    glm::vec4 ndc_pos_far{window_pos, 1, 1};

    auto inv_pjmv = glm::inverse(projection * view * model);
    auto world_pos_near = inv_pjmv * ndc_pos_near;
    auto world_pos_far = inv_pjmv * ndc_pos_far;

    world_pos_near /= world_pos_near.w;
    world_pos_far /= world_pos_far.w;

    auto vec = glm::normalize(world_pos_far - world_pos_near);
    return std::make_pair(glm::vec3{world_pos_near.x,
                                    world_pos_near.y,
                                    world_pos_near.z},
                          glm::vec3{vec.x,
                                    vec.y,
                                    vec.z});
}

void Renderer::setup_program(const glm::mat4 &model) {
    if (prgm_dirty) {
        glUseProgram(prgm->getID());
    }
    glm::mat4 modelview = view*model;
    glUniformMatrix4fv(prgm->getUniform("modelview"),
                       1, GL_FALSE, glm::value_ptr(modelview));
    if (prgm_dirty) {
        glUniformMatrix4fv(prgm->getUniform("perspective"),
                           1, GL_FALSE, glm::value_ptr(projection));
    }

    prgm_dirty = false;
}
