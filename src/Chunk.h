#ifndef CHUNK_H
#define CHUNK_H

#include "gfx/Mesh.h"
#include "Block.h"

#include <boost/optional.hpp>

#include <array>
#include <tuple>
#include <vector>
#include <ostream>

class Chunk {
public:
    static constexpr int XSize = 32;
    static constexpr int YSize = 32;
    static constexpr int ZSize = 32;

    Chunk() { }
    explicit Chunk(const Block &block) { fill(block); }
    static Chunk genRandom(const std::vector<Block> &blocks,
                           float density);

    static int getOffset(const glm::ivec3 &pos) {
        return pos.z + ZSize*(pos.y + YSize*pos.x);
    }
    static bool isValid(const glm::ivec3 &pos) {
        return pos.x >= 0 && pos.x < XSize &&
            pos.y >= 0 && pos.y < YSize &&
            pos.z >= 0 && pos.z < ZSize;
    }
    static glm::ivec3 nextPos(const glm::ivec3 &pos);

    Block &operator[](const glm::ivec3 &p) { return getBlock(p); }
    Block &getBlock(const glm::ivec3 &p) { return data[getOffset(p)]; }
    const Block &operator[](const glm::ivec3 &p) const { return getBlock(p); }
    const Block &getBlock(const glm::ivec3 &p) const { return data[getOffset(p)]; }

    void fill(const Block &block);
    void tesselate(MeshBuilder &builder) const;

    class iterator {
        friend iterator begin(Chunk &chunk);
        friend iterator end(Chunk &chunk);
    public:
        iterator &operator++() { pos = nextPos(pos); return *this; }
        Block &operator*() const { return (*chunk)[pos]; }
        Block *operator->() const { return &(*chunk)[pos]; }

        bool operator==(const iterator &i) const { return pos == i.pos; }
        bool operator!=(const iterator &i) const { return pos != i.pos; }

        const glm::ivec3 &getPos() const { return pos; }

    private:
        iterator(Chunk &chunk, const glm::ivec3 &pos) :
            chunk(&chunk), pos(pos) { }

        Chunk *chunk;
        glm::ivec3 pos;
    };

    class const_iterator {
        friend const_iterator begin(const Chunk &chunk);
        friend const_iterator end(const Chunk &chunk);
    public:
        const_iterator(const Chunk &chunk, const glm::ivec3 &pos) :
            chunk(&chunk), pos(pos) { }

        const_iterator &operator++() { pos = nextPos(pos); return *this; }
        const Block &operator*() const { return (*chunk)[pos]; }
        const Block *operator->() const { return &(*chunk)[pos]; }

        bool operator==(const const_iterator &i) const { return pos == i.pos; }
        bool operator!=(const const_iterator &i) const { return pos != i.pos; }

        const glm::ivec3 &getPos() const { return pos; }

    private:
        const Chunk *chunk;
        glm::ivec3 pos;
    };

private:
    std::array<Block, XSize*YSize*ZSize> data;

    void tesselate_face(MeshBuilder &builder, const glm::ivec3 &pos, Face f) const;
};

inline Chunk::iterator begin(Chunk &chunk) {
    return {chunk, glm::ivec3{0, 0, 0}};
}

inline Chunk::iterator end(Chunk &chunk) {
    return {chunk, glm::ivec3{0, 0, 32}};
}

inline Chunk::const_iterator begin(const Chunk &chunk) {
    return {chunk, glm::ivec3{0, 0, 0}};
}

inline Chunk::const_iterator end(const Chunk &chunk) {
    return {chunk, glm::ivec3{0, 0, 32}};
}

#endif
