#ifndef DEBUGVIEW_H
#define DEBUGVIEW_H

#include "gfx/View.h"
#include "gfx/Font.h"
#include "gfx/Shader.h"

class DebugView : public View {
public:
    DebugView(Font font,
              ShaderProgram prgm);

    virtual void render(Renderer &renderer, Window &window);
    
private:
    Font font;
    Sampler sampler;
    ShaderProgram prgm;

    MeshBuilder builder;
    
    OrthoProjection getProjection(Window &window);
};

#endif
