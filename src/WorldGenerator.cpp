#include "WorldGenerator.h"

bool WorldGenerator::RectangleBorder::inBounds(const glm::ivec3 &pos) const {
    return pos.x >= minx && pos.x < maxx &&
        pos.y >= miny && pos.y < maxy &&
        pos.z >= 0 && pos.z < height;
}

bool WorldGenerator::CircleBorder::inBounds(const glm::ivec3 &pos) const {
    return pos.x*pos.x + pos.y*pos.y < radius*radius &&
                                       pos.z >= 0 && pos.z < height;
}
