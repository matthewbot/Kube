#include "World.h"

World::World(const ChunkGenerator &chunkgen) :
    chunkgen(std::move(chunkgen)) { }

struct InBoundsVisitor :public boost::static_visitor<bool> {
    glm::ivec3 pos;
    InBoundsVisitor(const glm::ivec3 &pos) : pos(pos) { }

    template <typename Border>
    bool operator()(const Border &border) const {
        return border.inBounds(pos);
    }
};

void World::generateChunk(const glm::ivec3 &pos) {
    if (grid.getChunk(pos))
        return;
    ChunkGenerator::Border border = chunkgen.getBorder();
    if (!boost::apply_visitor(InBoundsVisitor(pos), border))
        return;
    grid.setChunk(pos, chunkgen.generateChunk(pos));
}
