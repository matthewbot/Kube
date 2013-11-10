#ifndef GRAPHICSSYSTEM_H
#define GRAPHICSSYSTEM_H

#include "World.h"
#include "gfx/ChunkMeshManager.h"
#include "gfx/Window.h"
#include "gfx/Renderer.h"
#include "gfx/Shader.h"
#include "gfx/Texture.h"
#include "gfx/Font.h"

#include <boost/asio.hpp>

class GraphicsSystem {
public:
    typedef std::function<void ()> InputCallback;
    
    GraphicsSystem(const World &world, boost::asio::io_service &main_io);

    void setInputCallback(const InputCallback &input_callback) {
        this->input_callback = input_callback;
    }
    
    Window &getWindow() { return window; }
    const Window &getWindow() const { return window; }

    RPYCamera &getCamera() { return camera; }
    const RPYCamera &getCamera() const { return camera; }

    PerspectiveProjection getPerspectiveProjection();
    OrthoProjection getOrthoProjection();

    void regenerateChunkMesh(const glm::ivec3 &chunkpos);

private:
    void renderFrame();
    void renderChunks();
    void renderText();
    void waitTimer();

    const World &world;
    boost::asio::io_service &main_io;
    boost::asio::deadline_timer timer;
    InputCallback input_callback;
    
    Window window;
    Renderer renderer;
    RPYCamera camera;
    
    ShaderProgram prgm2d;
    ShaderProgram prgm3d;
    ArrayTexture blocktex;
    Sampler sampler;
    Font font;
    Mesh fontmesh;
    ChunkMeshManager chunkmeshes;
};

#endif
