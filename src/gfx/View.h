#ifndef VIEW_H
#define VIEW_H

#include "gfx/Renderer.h"
#include "gfx/Window.h"

class View {
public:
    virtual void render(Renderer &renderer, Window &window) = 0;

    // TODO input event system passing events down view stack
};

#endif
