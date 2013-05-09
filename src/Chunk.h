#ifndef CHUNK_H
#define CHUNK_H

#include "Mesh.h"
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

    class Pos;

    Block &operator()(const Pos &p) {
        return data[p.getOffset()];
    }

    const Block &operator()(const Pos &p) const {
        return data[p.getOffset()];
    }

    void fill(const Block &block);
    Mesh tesselate() const;

    boost::optional<std::pair<Pos, Face>> pick(const glm::vec3 &startpos,
                                               const glm::vec3 &dir,
                                               float maxdist);

    class Pos {
    public:
        Pos() { }
        Pos(int x, int y, int z) :
            x(x), y(y), z(z) { }
        Pos(const glm::vec3 &vec) :
            x(vec.x), y(vec.y), z(vec.z) { }

        bool operator==(const Pos &p) const {
            return x == p.x && y == p.y && z == p.z;
        }

        bool operator!=(const Pos &p) const {
            return !(*this == p);
        }

        Pos adjacent(Face f) const;
        boost::optional<Face> sharedFace(const Pos &pos) const;
        Pos next() const;

        int getX() const { return x; }
        int getY() const { return y; }
        int getZ() const { return z; }
        int getOffset() const { return z + ZSize*(y + YSize*x); }
        bool isValid() const;

        operator glm::vec3() { return glm::vec3{x, y, z}; }

    private:
        int x;
        int y;
        int z;
    };

    class iterator {
        friend iterator begin(Chunk &chunk);
        friend iterator end(Chunk &chunk);
    public:
        iterator &operator++() { pos = pos.next(); return *this; }
        Block &operator*() const { return (*chunk)(pos); }
        Block *operator->() const { return &(*chunk)(pos); }

        bool operator==(const iterator &i) const { return pos == i.pos; }
        bool operator!=(const iterator &i) const { return pos != i.pos; }

        inline const Pos &getPos() const { return pos; }

    private:
        iterator(Chunk &chunk, const Pos &pos) :
            chunk(&chunk), pos(pos) { }

        Chunk *chunk;
        Pos pos;
    };

    class const_iterator {
        friend const_iterator begin(const Chunk &chunk);
        friend const_iterator end(const Chunk &chunk);
    public:
        const_iterator(const Chunk &chunk, const Pos &pos) :
            chunk(&chunk), pos(pos) { }

        const_iterator &operator++() { pos = pos.next(); return *this; }
        const Block &operator*() const { return (*chunk)(pos); }
        const Block *operator->() const { return &(*chunk)(pos); }

        bool operator==(const const_iterator &i) const { return pos == i.getPos(); }
        bool operator!=(const const_iterator &i) const { return pos != i.getPos(); }

        const Pos &getPos() const { return pos; }

    private:
        const Chunk *chunk;
        Pos pos;
    };

private:
    std::array<Block, XSize*YSize*ZSize> data;

    void tesselate_face(MeshBuilder &builder, const Pos &pos, Face f, unsigned int texnum) const;
};

inline Chunk::iterator begin(Chunk &chunk) {
    return {chunk, {0, 0, 0}};
}

inline Chunk::iterator end(Chunk &chunk) {
    return {chunk, {0, 0, 32}};
}

inline Chunk::const_iterator begin(const Chunk &chunk) {
    return {chunk, {0, 0, 0}};
}

inline Chunk::const_iterator end(const Chunk &chunk) {
    return {chunk, {0, 0, 32}};
}

std::ostream &operator<<(std::ostream &os, const Chunk::Pos &pos);

#endif
