#ifndef WINDOW_H
#define WINDOW_H

#include <cstdint>
#include <glm/glm.hpp>

enum class MouseButton : uint8_t { LEFT, RIGHT, MIDDLE };

class Window {
public:
    Window(unsigned int width, unsigned int height);
    Window(const Window &) = delete;
    ~Window();

    Window &operator=(const Window &) = delete;

    // TODO input system
    bool isMousePressed(MouseButton btn) const;
    bool isKeyPressed(char ch) const;
    bool isShiftPressed() const;
    bool isControlPressed() const;
    glm::ivec2 getMousePos() const;
    bool isClosed() const;

    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }
    float getAspectRatio() const { return static_cast<float>(width) / height; }

    glm::vec2 getNDCPos(const glm::ivec2 &mouse_pos) const;

    void clear();
    void swapBuffers();

private:
    unsigned int width;
    unsigned int height;
};

#endif
