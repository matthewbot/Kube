#include "ChunkGenerator.h"

bool ChunkGenerator::RectangleBorder::inBounds(const glm::ivec3 &pos) const {
    return pos.x >= minx && pos.x < maxx &&
        pos.y >= miny && pos.y < maxy &&
        pos.z >= 0 && pos.z < height;
}

bool ChunkGenerator::CircleBorder::inBounds(const glm::ivec3 &pos) const {
    return pos.x*pos.x + pos.y*pos.y < radius*radius &&
                                       pos.z >= 0 && pos.z < height;
}
