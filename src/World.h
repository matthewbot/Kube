#ifndef WORLD_H
#define WORLD_H

#include "ChunkGrid.h"
#include "Block.h"
#include "WorldGenerator.h"
#include "util/ThreadManager.h"

#include <boost/asio.hpp>
#include <unordered_set>
#include <memory>

class World {
public:
    World(const BlockTypeRegistry &blocktypes,
	  const WorldGenerator &chunkgen,
          ThreadManager &tm);

    ChunkGrid &getChunks() { return grid; }
    const ChunkGrid &getChunks() const { return grid; }

    const BlockTypeRegistry &getBlockTypes() const { return blocktypes; }
    
    void asyncGenerateChunk(const glm::ivec3 &pos);
    
private:
    ChunkGrid grid;
    const BlockTypeRegistry &blocktypes;
    const WorldGenerator &chunkgen;
    std::unordered_set<glm::ivec3> chunkgen_pending;

    ThreadManager &tm;
};

#endif
