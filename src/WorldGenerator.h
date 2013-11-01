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
    
    struct RectangleBorder {
        int minx;
        int maxx;
        int miny;
        int maxy;
        int height;

        bool inBounds(const glm::ivec3 &pos) const;
    };

    struct CircleBorder {
        int radius;
        int height;

        bool inBounds(const glm::ivec3 &pos) const;
    };

    struct NoBorder {
        int height;

        NoBorder(int height=1) : height(height) { }
        
        bool inBounds(const glm::ivec3 &pos) const { return pos.z < height; }
    };

    using Border = boost::variant<RectangleBorder, CircleBorder, NoBorder>;

    virtual Border getBorder() const=0;
};

#endif
