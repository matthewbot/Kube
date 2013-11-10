#ifndef WORLD_H
#define WORLD_H

#include "ChunkGrid.h"
#include "WorldGenerator.h"

#include <boost/asio.hpp>
#include <unordered_set>
#include <memory>

class World {
public:
    World(const WorldGenerator &chunkgen,
          boost::asio::io_service &main_io,
          boost::asio::io_service &work_io);

    ChunkGrid &getChunks() { return grid; }
    const ChunkGrid &getChunks() const { return grid; }

    void asyncGenerateChunk(const glm::ivec3 &pos);
    
private:
    ChunkGrid grid;
    const WorldGenerator &chunkgen;
    std::unordered_set<glm::ivec3> chunkgen_pending;

    boost::asio::io_service &main_io;
    boost::asio::io_service &work_io;
};

#endif
