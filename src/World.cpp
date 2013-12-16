#include "World.h"
#include <utility>

World::World(const BlockTypeRegistry &blocktypes,
	     const WorldGenerator &chunkgen,
             IOServiceThreads &threads) :
    chunkgen(chunkgen),
    blocktypes(blocktypes),
    threads(threads) { }

void World::asyncGenerateChunk(const glm::ivec3 &pos) {
    if (grid.getChunk(pos) || chunkgen_pending.count(pos))
        return;
    
    threads.postWork([=]() {
        std::shared_ptr<Chunk> chunkptr{
            chunkgen.generateChunk(pos, blocktypes).release()};
        threads.postMain([=]() {
            grid.setChunk(pos, std::move(chunkptr));
            chunkgen_pending.erase(pos);
        });
    });
    chunkgen_pending.insert(pos);
}
