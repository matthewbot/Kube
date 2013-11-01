#ifndef WORLD_H
#define WORLD_H

#include "ChunkGrid.h"
#include <memory>

class World {
public:
    World(const ChunkGenerator &chunkgen);

    ChunkGrid &getChunks() { return grid; }
    const ChunkGrid &getChunks() const { return grid; }

    void generateChunk(const glm::ivec3 &pos);
    
private:
    ChunkGrid grid;
    const ChunkGenerator &chunkgen;
};

#endif
