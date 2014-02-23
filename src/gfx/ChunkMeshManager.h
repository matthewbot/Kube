#ifndef CHUNKMESHMANAGER_H
#define CHUNKMESHMANAGER_H

#include "Chunk.h"
#include "util/ThreadManager.h"
#include "util/math.h"
#include "gfx/BlockVisualRegistry.h"
#include "gfx/Mesh.h"

#include <vector>
#include <utility>
#include <chrono>
#include <set>
#include <unordered_map>

class ChunkMeshManager {
public:
    ChunkMeshManager(ThreadManager &tm, BlockVisualRegistry blockvisuals);

    const Mesh *getMesh(const glm::ivec3 &pos) const;
    const Mesh *updateMesh(const glm::ivec3 &pos,
                           const std::shared_ptr<const Chunk> &chunk);

    // TODO delete me after Meshes have textures
    const ArrayTexture &getBlockTex() { return blockvisuals.getBlockTex(); }
    
    void freeUnusedMeshes();
    
private:
    ThreadManager &tm;
    BlockVisualRegistry blockvisuals;

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
