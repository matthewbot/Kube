#ifndef WORLDGENERATOR_H
#define WORLDGENERATOR_H

#include "Chunk.h"

#include <glm/glm.hpp>
#include <boost/variant.hpp>

#include <memory>

class WorldGenerator {
public:
    virtual ~WorldGenerator() { }

    virtual std::unique_ptr<Chunk> generateChunk(const glm::ivec3 &pos) const=0;
};

#endif
