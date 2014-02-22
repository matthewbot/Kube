#include "DebugView.h"
#include "gfx/tesselate.h"
#include <sstream>

DebugView::DebugView(Font font,
                     ShaderProgram prgm) :
    font(std::move(font)),
    prgm(std::move(prgm))
{ }

void DebugView::render(Window &window) {
    std::stringstream buf;
    buf << "Hello World";
    
    tesselate(builder, font, buf.str());
    Mesh fontmesh = builder.build();

    prgm.setUniform("perspective", getProjection(window).getMatrix());

    font.getTexture().bind(0);
    sampler.bind(0);
    
    glDisable(GL_DEPTH_TEST);
    fontmesh.draw(prgm);
    glEnable(GL_DEPTH_TEST);
}

OrthoProjection DebugView::getProjection(Window &window) {
    return OrthoProjection{
        static_cast<float>(window.getWidth()),
        static_cast<float>(window.getHeight())};
}
