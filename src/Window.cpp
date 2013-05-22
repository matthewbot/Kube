#include "Window.h"
#include <GL/glew.h>
#include <GL/glfw.h>
#include <stdexcept>
#include <iostream>
#include <cctype>

Window::Window(unsigned int width, unsigned int height) :
    width(width), height(height)
{
    if (!glfwInit()) {
        throw std::runtime_error("Failed to init GLFW");
    }

    glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);

    if (GL_TRUE != glfwOpenWindow(width, height, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)) {
        throw std::runtime_error("Unable to create OpenGL context");
    }

    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Unable to init GLEW");
    }

    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);

    glEnable(GL_FRAMEBUFFER_SRGB);
}

Window::~Window() {
    glfwTerminate();
}

bool Window::isMousePressed(MouseButton btn) const {
    static const int glfw_btns[] = { GLFW_MOUSE_BUTTON_LEFT,
                                     GLFW_MOUSE_BUTTON_RIGHT,
                                     GLFW_MOUSE_BUTTON_MIDDLE };
    return glfwGetMouseButton(glfw_btns[static_cast<int>(btn)]) == GLFW_PRESS;
}

bool Window::isKeyPressed(char ch) const {
    return glfwGetKey(toupper(ch));
}

glm::ivec2 Window::getMousePos() const {
    int cur_x, cur_y;
    glfwGetMousePos(&cur_x, &cur_y);
    return glm::ivec2{cur_x, cur_y};
}

bool Window::isClosed() const {
    return !glfwGetWindowParam(GLFW_OPENED);
}

glm::vec2 Window::getNDCPos(const glm::ivec2 &mouse_pos) const {
    return glm::vec2{
        2*static_cast<float>(mouse_pos.x) / 800 - 1,
        -2*static_cast<float>(mouse_pos.y) / 600 + 1,
    };
}

void Window::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::swapBuffers() {
    glfwSwapBuffers();
}
