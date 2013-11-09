#ifndef GRAPHICSSYSTEM_H
#define GRAPHICSSYSTEM_H

#include "World.h"
#include "gfx/Window.h"
#include "gfx/Renderer.h"
#include "gfx/Shader.h"
#include "gfx/Texture.h"
#include "gfx/Font.h"

class GraphicsSystem {
public:
    GraphicsSystem(const World &world);

    Window &getWindow() { return window; }
    const Window &getWindow() const { return window; }

    RPYCamera &getCamera() { return camera; }
    const RPYCamera &getCamera() const { return camera; }

    PerspectiveProjection getPerspectiveProjection();
    OrthoProjection getOrthoProjection();

    void render();
    
private:
    const World &world;

    Window window;
    Renderer renderer;
    RPYCamera camera;
    
    ShaderProgram prgm2d;
    ShaderProgram prgm3d;
    ArrayTexture blocktex;
    Sampler sampler;
    Font font;
    Mesh fontmesh;
};

#endif
