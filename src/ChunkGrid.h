#ifndef CHUNKGRID_H
#define CHUNKGRID_H

#include "Chunk.h"
#include "util/math.h"
#include "util/Optional.h"

#include <glm/glm.hpp>
#include <unordered_map>
#include <memory>

class ChunkGrid {
public:
    ChunkGrid();

    std::shared_ptr<const Chunk> getChunk(const glm::ivec3 &pos) const;

    void setChunk(const glm::ivec3 &pos, std::shared_ptr<Chunk> chunk);
    void clearAllChunks();
    
    Optional<Block> findBlock(glm::ivec3 &pos) const;

    Optional<glm::ivec3> pick(const glm::vec3 &pos,
                              const glm::vec3 &dir,
                              float range) const;

    static std::pair<glm::ivec3, glm::ivec3> posToChunkBlock(const glm::ivec3 &pos);

private:
    struct Entry {
        std::shared_ptr<Chunk> chunk;
    };

    using ChunkMap = std::unordered_map<glm::ivec3, Entry>;
    ChunkMap chunks;
};

#endif


