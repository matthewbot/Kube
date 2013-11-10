#include "World.h"
#include <utility>

World::World(const WorldGenerator &chunkgen, 
             boost::asio::io_service &main_io,
             boost::asio::io_service &work_io) :
    chunkgen(std::move(chunkgen)),
    main_io(main_io),
    work_io(work_io) { }

void World::asyncGenerateChunk(const glm::ivec3 &pos) {
    if (grid.getChunk(pos) || chunkgen_pending.count(pos))
        return;
    
    work_io.post([=]() {
        std::shared_ptr<Chunk> chunkptr{chunkgen.generateChunk(pos).release()};
        main_io.post([=]() {
            grid.setChunk(pos, std::move(chunkptr));
            chunkgen_pending.erase(pos);
        });
    });
    chunkgen_pending.insert(pos);
}
