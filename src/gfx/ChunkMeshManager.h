#ifndef CHUNKMESHMANAGER_H
#define CHUNKMESHMANAGER_H

#include "IOServiceThreads.h"
#include "ChunkGrid.h"

#include <vector>
#include <utility>
#include <chrono>
#include <boost/asio.hpp>
#include <unordered_set>

class ChunkMeshManager {
public:
    ChunkMeshManager(const ChunkGrid &grid,
                     IOServiceThreads &threads);

    const Mesh *getMesh(const glm::ivec3 &pos) const;
    const Mesh *getMeshOrAsyncGenerate(const glm::ivec3 &pos);

    void asyncGenerateMesh(const glm::ivec3 &pos);

    void freeUnusedMeshes();
    
private:
    const ChunkGrid &grid;
    IOServiceThreads &threads;

    struct Entry {
        Mesh mesh;
        mutable int unused_ctr;
    };
    std::unordered_map<glm::ivec3, Entry> meshmap;
    std::unordered_set<glm::ivec3> meshgen_pending;
};

#endif
