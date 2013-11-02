#include "World.h"

World::World(const WorldGenerator &chunkgen) :
    chunkgen(std::move(chunkgen)) { }

bool World::generateChunk(const glm::ivec3 &pos) {
    if (grid.getChunk(pos))
        return false;
    grid.setChunk(pos, chunkgen.generateChunk(pos));
    return true;
}
