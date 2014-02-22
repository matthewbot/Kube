#include "gfx/WorldView.h"
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>

WorldView::WorldView(ThreadManager &tm,
                     const World &world,
                     ArrayTexture tex,
                     Sampler sampler,
                     ShaderProgram prgm,
                     BlockVisualRegistry blockvisuals) :
    world(world),
    tex(std::move(tex)),
    sampler(std::move(sampler)),
    prgm(std::move(prgm)),
    chunkmeshes(tm, std::move(blockvisuals))
{ }

void WorldView::render(Window &window) {
    prgm.setUniform("perspective", getProjection(window).getMatrix());

    tex.bind(0);
    sampler.bind(0);

    const glm::mat4 view = camera.getMatrix();

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
                prgm.setUniform("modelview", view*model);
                meshptr->draw(prgm);
            }
        }
    }

    chunkmeshes.freeUnusedMeshes();
}
    
PerspectiveProjection WorldView::getProjection(Window &window) {
    PerspectiveProjection proj;
    proj.aspect = window.getAspectRatio();
    return proj;
}
