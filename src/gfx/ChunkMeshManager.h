#ifndef CHUNKMESHMANAGER_H
#define CHUNKMESHMANAGER_H

#include "ChunkGrid.h"

#include <vector>
#include <utility>
#include <boost/asio.hpp>

class ChunkMeshManager {
public:
    ChunkMeshManager(boost::asio::io_service &main_io,
                     boost::asio::io_service &work_io,
                     const ChunkGrid &grid);

    const Mesh *getMesh(const glm::ivec3 &pos) const;
    const Mesh *getMeshOrAsyncGenerate(const glm::ivec3 &pos);

    using MeshPoses = std::vector<std::pair<glm::ivec3, const Mesh &>>;
    MeshPoses getMeshPoses() const;
    
    void asyncGenerateMesh(const glm::ivec3 &pos);
    
private:
    boost::asio::io_service &main_io;
    boost::asio::io_service &work_io;
    const ChunkGrid &grid;

    std::unordered_map<glm::ivec3, Mesh> meshmap;
};

#endif
