#include "World.h"
#include <utility>

World::World(const BlockTypeRegistry &blocktypes,
	     const WorldGenerator &chunkgen,
             ThreadManager &tm) :
    blocktypes(blocktypes),
    chunkgen(chunkgen),
    tm(tm) { }

void World::asyncGenerateChunk(const glm::ivec3 &pos) {
    if (grid.getChunk(pos) || chunkgen_pending.count(pos))
        return;
    
    tm.postWork([=]() {
        std::shared_ptr<Chunk> chunkptr{
            chunkgen.generateChunk(pos, blocktypes).release()};
        tm.postMain([=, chunkptr = std::move(chunkptr)]() {
            grid.setChunk(pos, std::move(chunkptr));
            chunkgen_pending.erase(pos);
        });
    });
    chunkgen_pending.insert(pos);
}
