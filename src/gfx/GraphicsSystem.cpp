#include "gfx/GraphicsSystem.h"

#include <glm/gtc/matrix_transform.hpp>
#include <boost/bind.hpp>

static const auto rate = boost::posix_time::seconds(1/60.0);

GraphicsSystem::GraphicsSystem(const World &world,
                               ThreadManager &tm) :
    world(world),
    tm(tm),
    window(800, 600),
    chunkmeshes(tm)
{
    Shader vert3d{Shader::Type::VERTEX, "vert.glsl"};
    Shader frag3d{Shader::Type::FRAGMENT, "frag.glsl"};
    prgm3d = ShaderProgram{vert3d, frag3d};

    Shader vert2d{Shader::Type::VERTEX, "vert2d.glsl"};
    Shader frag2d{Shader::Type::FRAGMENT, "frag2d.glsl"};
    prgm2d = ShaderProgram{vert2d, frag2d};

    blocktex.setImage(Image::loadPNG("blocks.png"), 4);
    sampler.setFilter(Sampler::NEAREST);
    
    font.load("font.fnt");
}

void GraphicsSystem::runRenderLoop(std::function<bool ()> input_callback) {
    do {
	if (!input_callback()) {
	    break;
	}
	renderFrame();
    } while (tm.runMain(std::chrono::duration_cast<std::chrono::milliseconds>(
			    std::chrono::duration<double>(1.0/60.0))));
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

void GraphicsSystem::renderFrame() {
    renderChunks();
    renderText();
}

void GraphicsSystem::renderChunks() {
    window.clear();
    renderer.setCamera(camera);
    renderer.setProjection(getPerspectiveProjection());
    renderer.setProgram(prgm3d);
    renderer.setTexture(0, blocktex, sampler);

    glm::ivec3 centerchunkpos =
        ChunkGrid::posToChunkBlock(glm::ivec3{camera.pos}).first;

    for (int x = centerchunkpos.x - 3; x <= centerchunkpos.x + 3; x++) {
        for (int y = centerchunkpos.y - 3; y <= centerchunkpos.y + 3; y++) {
            for (int z = centerchunkpos.z - 3; z <= centerchunkpos.z + 3; z++) {
                glm::ivec3 chunkpos{x, y, z};
                auto chunkptr = world.getChunks().getChunk(chunkpos);
                auto meshptr = chunkmeshes.updateMesh(chunkpos, chunkptr);
                if (!meshptr) {
                    continue;
                }
                
                glm::mat4 model{1};
                model = glm::translate(model, glm::vec3{32*chunkpos});
                renderer.render(model, *meshptr);
            }
        }
    }

    chunkmeshes.freeUnusedMeshes();
}

void GraphicsSystem::renderText() {
    std::stringstream buf;
    buf << "X: " << static_cast<int>(camera.pos.x) << ' '
        << "Y: " << static_cast<int>(camera.pos.y) << ' '
        << "Z: " << static_cast<int>(camera.pos.z);
    fontmesh = font.tesselate(buf.str());
    
    renderer.setProjection(getOrthoProjection());
    renderer.clearCamera();
    renderer.setProgram(prgm2d);
    renderer.setTexture(0, font.getTexture(), sampler);
    glDisable(GL_DEPTH_TEST);
    renderer.render(glm::mat4{1}, fontmesh);
    glEnable(GL_DEPTH_TEST);    
    window.swapBuffers();
}
