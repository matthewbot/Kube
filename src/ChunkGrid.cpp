#include "ChunkGrid.h"

ChunkGrid::ChunkGrid() { }

std::shared_ptr<const Chunk> ChunkGrid::getChunk(const glm::ivec3 &pos) const {
    auto iter = chunks.find(pos);
    if (iter == chunks.end()) {
        return nullptr;
    } else {
        return iter->second.chunk;
    }
}

void ChunkGrid::setChunk(const glm::ivec3 &pos, std::shared_ptr<Chunk> chunk) {
    auto &entry = chunks[pos];
    entry.chunk = std::move(chunk);
}

void ChunkGrid::clearAllChunks() {
    chunks.clear();
}

boost::optional<Block> ChunkGrid::findBlock(glm::ivec3 &pos) const {
    glm::ivec3 chunkpos, blockpos;
    std::tie(chunkpos, blockpos) = posToChunkBlock(pos);

    auto maybeChunk = getChunk(chunkpos);
    if (maybeChunk)
        return maybeChunk->getBlock(ChunkIndex{blockpos});
    else
        return boost::none;
}

boost::optional<glm::ivec3> ChunkGrid::pick(const glm::vec3 &startpos,
                                            const glm::vec3 &dir,
                                            float range) const {
    static constexpr float step = .01;
    glm::vec3 pos = startpos;

    while (range >= 0) {
        glm::ivec3 ipos = static_cast<glm::ivec3>(floorVec(pos));
        auto optblock = findBlock(ipos);
        if (optblock && !optblock->isAir()) {
            return ipos;
        }

        pos += step*dir;
        range -= step;
    }

    return boost::none;
}

std::pair<glm::ivec3, glm::ivec3> ChunkGrid::posToChunkBlock(const glm::ivec3 &pos) {
    static_assert(Chunk::XSize == 32 &&
                  Chunk::YSize == 32 &&
                  Chunk::ZSize == 32, "Chunk must be 32x32x32");
    glm::ivec3 chunkpos{pos.x >> 5, pos.y >> 5, pos.z >> 5};
    glm::ivec3 blockpos = pos - 32*chunkpos;
    return {chunkpos, blockpos};
}
