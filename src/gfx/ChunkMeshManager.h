#ifndef CHUNKMESHMANAGER_H
#define CHUNKMESHMANAGER_H

#include "ChunkGrid.h"

#include <vector>
#include <utility>
#include <chrono>
#include <boost/asio.hpp>
#include <unordered_set>

class ChunkMeshManager {
public:
    ChunkMeshManager(boost::asio::io_service &main_io,
                     boost::asio::io_service &work_io,
                     const ChunkGrid &grid);

    const Mesh *getMesh(const glm::ivec3 &pos) const;
    const Mesh *getMeshOrAsyncGenerate(const glm::ivec3 &pos);

    void asyncGenerateMesh(const glm::ivec3 &pos);

    void freeUnusedMeshes();
    
private:
    boost::asio::io_service &main_io;
    boost::asio::io_service &work_io;
    const ChunkGrid &grid;

    struct Entry {
        Mesh mesh;
        mutable int unused_ctr;
    };
    std::unordered_map<glm::ivec3, Entry> meshmap;
    std::unordered_set<glm::ivec3> meshgen_pending;
};

#endif
