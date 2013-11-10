#include "ChunkMeshManager.h"

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
        return &iter->second;
    }
}

const Mesh *ChunkMeshManager::getMeshOrAsyncGenerate(const glm::ivec3 &pos) {
    const Mesh *mesh = getMesh(pos);
    if (mesh == nullptr) {
        asyncGenerateMesh(pos);
    }
    return mesh;
}

ChunkMeshManager::MeshPoses ChunkMeshManager::getMeshPoses() const {
    MeshPoses meshposes;

    for (const auto &mapentry : meshmap) {
        meshposes.push_back(mapentry);
    }

    return meshposes;
}

void ChunkMeshManager::asyncGenerateMesh(const glm::ivec3 &pos) {
    work_io.post([=]() {
        auto chunkptr = grid.getChunk(pos);
        if (!chunkptr) {
            return;
        }
        
        MeshBuilder builder = chunkptr->tesselate();
        main_io.post([=, builder = std::move(builder)]() {
            meshmap[pos] = Mesh{builder}; // TODO
        });
    });
}
