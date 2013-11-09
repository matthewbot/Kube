#include "gfx/GraphicsSystem.h"

#include <glm/gtc/matrix_transform.hpp>

GraphicsSystem::GraphicsSystem(const World &world) :
    world(world),
    window(800, 600)
{
    Shader vert3d{Shader::Type::VERTEX, "vert.glsl"};
    Shader frag3d{Shader::Type::FRAGMENT, "frag.glsl"};
    prgm3d = ShaderProgram{vert3d, frag3d}; // TODO

    Shader vert2d{Shader::Type::VERTEX, "vert2d.glsl"};
    Shader frag2d{Shader::Type::FRAGMENT, "frag2d.glsl"};
    prgm2d = ShaderProgram{vert2d, frag2d};

    blocktex.setImage(Image::loadPNG("blocks.png"), 4);
    sampler.setFilter(Sampler::NEAREST);
    
    font.load("font.fnt");
    fontmesh = font.tesselate("Hello World!");
}

PerspectiveProjection GraphicsSystem::getPerspectiveProjection() {
    PerspectiveProjection perspective;
    perspective.aspect = window.getAspectRatio();
    return perspective;
}

OrthoProjection GraphicsSystem::getOrthoProjection() {
    return OrthoProjection{
        static_cast<float>(window.getWidth()),
        static_cast<float>(window.getHeight())};
}

void GraphicsSystem::render() {
    window.clear();

    renderer.setCamera(camera);
    renderer.setProjection(getPerspectiveProjection());
    renderer.setProgram(prgm3d);
    renderer.setTexture(0, blocktex, sampler);

    for (const auto &meshpos : world.getChunks().getMeshPoses()) {
        glm::mat4 model{1};
        model = glm::translate(model, glm::vec3{32*meshpos.first});
        renderer.render(model, meshpos.second);
    }

    renderer.setProjection(getOrthoProjection());
    renderer.clearCamera();
    renderer.setProgram(prgm2d);
    renderer.setTexture(0, font.getTexture(), sampler);
    glDisable(GL_DEPTH_TEST);
    renderer.render(glm::mat4{1}, fontmesh);
    glEnable(GL_DEPTH_TEST);    
    window.swapBuffers();
}
