#include "Chunk.h"

#include <random>
#include <chrono>
#include <utility>
#include <cassert>
#include <glm/glm.hpp>

const detail::ChunkIndexRangeType ChunkIndex::Range;

unsigned int ChunkIndex::getOffset() const {
    return vec.z + Chunk::ZSize*(vec.y + Chunk::YSize*vec.x);
}

void ChunkIndex::advance() {
    vec.z++;
    if (vec.z >= Chunk::ZSize) {
        vec.z = 0;
        vec.y++;
        if (vec.y >= Chunk::YSize) {
            vec.y = 0;
            vec.x++;
        }
    }
}

bool ChunkIndex::isValid() const {
    return vec.x >= 0 && vec.x < Chunk::XSize &&
           vec.y >= 0 && vec.y < Chunk::YSize &&
           vec.z >= 0 && vec.z < Chunk::ZSize;
}

Block Chunk::getBlock(unsigned int offset) const {
    assert(offset < data.size());
    return reg->getType(data[offset]);
}

void Chunk::setBlock(unsigned int offset, const Block &block) {
    assert(offset < data.size());
    data[offset] = block.getID();
}

void Chunk::fill(const Block &block) {
    data.fill(block.getID());
}
