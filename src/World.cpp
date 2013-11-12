#include "World.h"
#include <utility>

World::World(const WorldGenerator &chunkgen, 
             IOServiceThreads &threads) :
    chunkgen(std::move(chunkgen)),
    threads(threads) { }

void World::asyncGenerateChunk(const glm::ivec3 &pos) {
    if (grid.getChunk(pos) || chunkgen_pending.count(pos))
        return;
    
    threads.postWork([=]() {
        std::shared_ptr<Chunk> chunkptr{chunkgen.generateChunk(pos).release()};
        threads.postMain([=]() {
            grid.setChunk(pos, std::move(chunkptr));
            chunkgen_pending.erase(pos);
        });
    });
    chunkgen_pending.insert(pos);
}
