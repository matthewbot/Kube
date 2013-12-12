#include "ChunkMeshManager.h"

#include <iostream>

ChunkMeshManager::ChunkMeshManager(IOServiceThreads &threads) :
    threads(threads) { }

const Mesh *ChunkMeshManager::getMesh(const glm::ivec3 &pos) const {
    // updateMesh doesn't modify anything if chunk is empty
    return const_cast<ChunkMeshManager *>(this)->updateMesh(
        pos, std::shared_ptr<const Chunk>());
}

const Mesh *ChunkMeshManager::updateMesh(const glm::ivec3 &pos,
                                         const std::shared_ptr<const Chunk> &chunk) {
    // Find mesh in our cache
    auto iter = meshmap.find(pos);
    if (iter == meshmap.end()) {
        if (chunk) {
            std::cout << "Don't have mesh!" << std::endl;
            // If we don't have it, generate it but in the mean time return null
            asyncGenerateMesh(pos, chunk);
        }
        
        return nullptr;
    }

    const Entry &entry = iter->second;

    if (chunk && entry.chunkptr.lock() != chunk) {
        // If the mesh is not for this chunk, regenerate it.
        // In this case, we're returning a stale mesh, hopefully not for long.
        asyncGenerateMesh(pos, chunk);
    }

    // Reset entries idle counter to zero
    entry.idlectr = 0;
    
    // Return the mesh we found
    return &entry.mesh;
}

void ChunkMeshManager::asyncGenerateMesh(const glm::ivec3 &pos,
                                         std::shared_ptr<const Chunk> chunk) {
    if (meshgen_pending.count(chunk))
        return;
    meshgen_pending.insert(chunk);

    threads.postWork([=, chunk = std::move(chunk)]() {
        MeshBuilder builder = chunk->tesselate();
        threads.postMain([=,
                          chunk = std::move(chunk),
                          builder = std::move(builder)]() {
            std::cout << "Uploading mesh at "
                      << pos.x << ","
                      << pos.y << std::endl;
            Entry &entry = meshmap[pos];
            entry.mesh = Mesh{builder}; // TODO
            entry.chunkptr = chunk;
            entry.idlectr = 0;
            meshgen_pending.erase(chunk);
        });
    });
}

void ChunkMeshManager::freeUnusedMeshes() {
    for (auto i = std::begin(meshmap); i != std::end(meshmap); ) {
        Entry &entry = i->second;
        if (entry.chunkptr.expired() && entry.idlectr > 100) {
            std::cout << "Erasing mesh at "
                      << i->first.x << ","
                      << i->first.y << std::endl;
            i = meshmap.erase(i);
        } else {
            entry.idlectr++;
            ++i;
        }
    }
}
