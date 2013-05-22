#include "ChunkGrid.h"

ChunkGrid::ChunkGrid() { }

boost::optional<const Chunk &> ChunkGrid::getChunk(const glm::ivec3 &pos) const {
    auto iter = chunks.find(pos);
    if (iter == chunks.end()) {
        return boost::none;
    } else {
        return *iter->second.chunk;
    }
}

boost::optional<const Mesh &> ChunkGrid::getMesh(const glm::ivec3 &pos) const {
    auto iter = chunks.find(pos);
    if (iter == chunks.end()) {
        return boost::none;
    } else if (!iter->second.mesh) {
        return boost::none;
    } else {
        return iter->second.mesh;
    }
}

std::vector<std::pair<glm::ivec3, const Mesh &>> ChunkGrid::getMeshPoses() const {
    std::vector<std::pair<glm::ivec3, const Mesh &>> meshposes;

    for (const auto &mapentry : chunks) {
        if (!!mapentry.second.mesh) {
            meshposes.emplace_back(mapentry.first, mapentry.second.mesh);
        }
    }

    return meshposes;
}

void ChunkGrid::setChunk(const glm::ivec3 &pos, std::unique_ptr<Chunk> &chunk) {
    auto &entry = chunks[pos];
    entry.chunk = std::move(chunk);
    entry.mesh = entry.chunk->tesselate();
}
