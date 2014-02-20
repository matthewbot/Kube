#include "DebugView.h"
#include "gfx/tesselate.h"
#include <sstream>

DebugView::DebugView(Font font,
                     ShaderProgram prgm) :
    font(std::move(font)),
    prgm(std::move(prgm))
{ }

void DebugView::render(Renderer &renderer, Window &window) {
    std::stringstream buf;
    buf << "Hello World";
    
    tesselate(builder, font, buf.str());
    Mesh fontmesh = builder.build();

    renderer.setProjection(getProjection(window));
    renderer.clearCamera();
    renderer.setProgram(prgm);
    renderer.setTexture(0, font.getTexture(), sampler);
    glDisable(GL_DEPTH_TEST);
    renderer.render(glm::mat4{1}, fontmesh);
    glEnable(GL_DEPTH_TEST);
}

OrthoProjection DebugView::getProjection(Window &window) {
    return OrthoProjection{
        static_cast<float>(window.getWidth()),
        static_cast<float>(window.getHeight())};
}
