#include "ChunkMeshManager.h"

#include <iostream>

ChunkMeshManager::ChunkMeshManager(boost::asio::io_service &main_io,
                                   boost::asio::io_service &work_io,
                                   const ChunkGrid &grid) :
    main_io(main_io),
    work_io(work_io),
    grid(grid) { }

const Mesh *ChunkMeshManager::getMesh(const glm::ivec3 &pos) const {
    auto iter = meshmap.find(pos);
    if (iter == meshmap.end()) {
        return nullptr;
    } else {
        const Entry &entry = iter->second;
        entry.unused_ctr = 0;
        return &entry.mesh;
    }
}

const Mesh *ChunkMeshManager::getMeshOrAsyncGenerate(const glm::ivec3 &pos) {
    const Mesh *mesh = getMesh(pos);
    if (mesh == nullptr) {
        asyncGenerateMesh(pos);
    }
    return mesh;
}

void ChunkMeshManager::asyncGenerateMesh(const glm::ivec3 &pos) {
    // TODO race condition here actually
    // if asyncGenerateMesh is called while an old version of the mesh
    // is being generated, a new version won't be made
    // Need a proper work queue
    if (meshgen_pending.count(pos))
        return;
    
    meshgen_pending.insert(pos);
    work_io.post([=]() {
        auto chunkptr = grid.getChunk(pos);
        if (!chunkptr) {
            main_io.post([=]() {
                meshgen_pending.erase(pos);
            });
            return;
        }
        
        MeshBuilder builder = chunkptr->tesselate();
        main_io.post([this, pos, builder = std::move(builder)]() {
            std::cout << "Uploading mesh at "
                      << pos.x << ","
                      << pos.y << std::endl;
            Entry &entry = meshmap[pos];
            entry.mesh = Mesh{builder}; // TODO
            entry.unused_ctr = 0;
            meshgen_pending.erase(pos);
        });
    });
}

void ChunkMeshManager::freeUnusedMeshes() {
    for (decltype(meshmap)::iterator i = std::begin(meshmap);
         i != std::end(meshmap);
        ) {        
        Entry &entry = i->second;
        if (entry.unused_ctr++ > 60*15) {
            std::cout << "Erasing mesh at "
                      << i->first.x << ","
                      << i->first.y << std::endl;
            i = meshmap.erase(i);
        } else {
            ++i;
        }
    }
}