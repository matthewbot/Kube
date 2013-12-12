#ifndef CHUNKMESHMANAGER_H
#define CHUNKMESHMANAGER_H

#include "IOServiceThreads.h"
#include "Chunk.h"
#include "Mesh.h"

#include <vector>
#include <utility>
#include <chrono>
#include <set>
#include <unordered_map>

class ChunkMeshManager {
public:
    ChunkMeshManager(IOServiceThreads &threads);

    const Mesh *getMesh(const glm::ivec3 &pos) const;
    const Mesh *updateMesh(const glm::ivec3 &pos,
                           const std::shared_ptr<const Chunk> &chunk);

    void freeUnusedMeshes();
    
private:
    IOServiceThreads &threads;

    void asyncGenerateMesh(const glm::ivec3 &pos,
                           std::shared_ptr<const Chunk> chunk);
    
    struct Entry {
        Mesh mesh;
        std::weak_ptr<const Chunk> chunkptr;
        mutable int idlectr;
    };
    std::unordered_map<glm::ivec3, Entry> meshmap;
    // TODO unordered
    std::set<std::shared_ptr<const Chunk>> meshgen_pending;
};

#endif
