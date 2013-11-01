#ifndef CHUNKGRID_H
#define CHUNKGRID_H

#include "Chunk.h"

#include <glm/glm.hpp>
#include <unordered_map>
#include <memory>
#include <boost/optional.hpp>

class ChunkGrid {
public:
    ChunkGrid();

    const Chunk *getChunk(const glm::ivec3 &pos) const;
    const Mesh *getMesh(const glm::ivec3 &pos) const;
    std::vector<std::pair<glm::ivec3, const Mesh &>> getMeshPoses() const;

    void setChunk(const glm::ivec3 &pos, std::unique_ptr<Chunk> &chunk);

    const Block *findBlock(glm::ivec3 &pos) const;

    boost::optional<glm::ivec3> pick(const glm::vec3 &pos,
                                     const glm::vec3 &dir,
                                     float range) const;

    static std::pair<glm::ivec3, glm::ivec3> posToChunkBlock(const glm::ivec3 &pos);

private:
    struct Entry {
        std::unique_ptr<Chunk> chunk;
        Mesh mesh;
    };

    using ChunkMap = std::unordered_map<glm::ivec3, Entry>;
    ChunkMap chunks;
};

#endif


