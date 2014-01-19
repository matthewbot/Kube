#ifndef CHUNK_H
#define CHUNK_H

#include "gfx/Mesh.h" // TODO
#include "util/Optional.h"

#include "Block.h"
#include "BlockTypeRegistry.h"

#include <array>
#include <tuple>
#include <vector>
#include <ostream>

namespace detail {
    struct ChunkIndexRangeType;
}
class Chunk;

class ChunkIndex {
public:
    ChunkIndex() : vec{0, 0, 0} { }
    ChunkIndex(int x, int y, int z) : vec{x, y, z} { }
    ChunkIndex(glm::ivec3 vec) : vec{vec} { }

    static const detail::ChunkIndexRangeType Range; // TODO lowercase
    
    unsigned int getOffset() const;
    const glm::ivec3 &getVec() const { return vec; }
    
    ChunkIndex adjacent(Face face) const { return { adjacentPos(vec, face) }; }
    Optional<Face> sharedFace(const ChunkIndex &other) const {
        return ::sharedFace(vec, other.vec);
    }

    bool isValid() const;
    explicit operator bool() const { return isValid(); }

    void advance();
    ChunkIndex &operator++() { advance(); return *this; }
    ChunkIndex operator++(int) { auto tmp = *this; tmp.advance(); return tmp; }
    bool operator==(const ChunkIndex &other) { return vec == other.vec; }
    bool operator!=(const ChunkIndex &other) { return vec != other.vec; }
    
private:
    glm::ivec3 vec;
};

class Chunk {
public:
    static constexpr int XSize = 32;
    static constexpr int YSize = 32;
    static constexpr int ZSize = 32;

    Chunk(const BlockTypeRegistry &reg) : reg(&reg) { }

    Block getBlock(unsigned int offset) const;
    void setBlock(unsigned int offset, const Block &block);

    Block getBlock(const ChunkIndex &index) const {
        return getBlock(index.getOffset());
    }

    void setBlock(const ChunkIndex &index, const Block &block) {
        setBlock(index.getOffset(), block);
    }

    void fill(const Block &block);

    // TODO move elsewhere (gfx/tesselate.h)
    void tesselate(MeshBuilder &builder) const;

private:
    const BlockTypeRegistry *reg;
    std::array<BlockType::ID, XSize*YSize*ZSize> data;

    void tesselate_face(MeshBuilder &builder, const ChunkIndex &pos, Face f) const;
};

namespace detail {
    struct ChunkIndexRangeType { };

    struct ChunkIndexIterator {
        ChunkIndex pos;

        const ChunkIndex &operator*() const { return pos; }
        const ChunkIndex *operator->() const { return &pos; }
        ChunkIndexIterator &operator++() { pos.advance(); return *this; }
        ChunkIndexIterator operator++(int) { auto tmp = *this; ++tmp; return tmp; }
        bool operator==(const ChunkIndexIterator &other) { return pos == other.pos; }
        bool operator!=(const ChunkIndexIterator &other) { return pos != other.pos; }
    };

    inline ChunkIndexIterator begin(ChunkIndexRangeType) {
        return { {0, 0, 0} };
    }
    
    inline ChunkIndexIterator end(ChunkIndexRangeType) {
        return { {Chunk::XSize, 0, 0} };
    }
};

#endif
