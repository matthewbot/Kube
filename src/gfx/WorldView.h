#ifndef WORLDVIEW_H
#define WORLDVIEW_H

#include "util/ThreadManager.h"
#include "gfx/View.h"
#include "gfx/Texture.h"
#include "gfx/Shader.h"
#include "gfx/ChunkMeshManager.h"
#include "gfx/Camera.h"
#include "World.h"

class WorldView : public View {
public:
    WorldView(ThreadManager &tm,
              const World &world,
              ArrayTexture tex,
              Sampler sampler,
              ShaderProgram prgm,
              BlockVisualRegistry blockvisuals);

    RPYCamera &getCamera() { return camera; }
    const RPYCamera &getCamera() const { return camera; }

    virtual void render(Window &window);

    PerspectiveProjection getProjection(Window &window); // TODO

private:
    const World &world;
    ArrayTexture tex;
    Sampler sampler;
    ShaderProgram prgm;

    ChunkMeshManager chunkmeshes;
    RPYCamera camera;
};

#endif
