#include "Chunk.h"

#include <random>
#include <chrono>
#include <utility>
#include <cassert>
#include <glm/glm.hpp>

Chunk Chunk::genRandom(const std::vector<Block> &blocks,
                       float density) {
    static std::default_random_engine generator;
    std::uniform_real_distribution<float> choose_air{0, 1};
    std::uniform_int_distribution<int> choose_block{0, static_cast<int>(blocks.size()-1)};

    Chunk chunk;
    for (auto &block : chunk) {
        if (choose_air(generator) < density)
            block = blocks[choose_block(generator)];
        else
            block = Block::air();
    }

    return chunk;
}

void Chunk::fill(const Block &block) {
    for (auto &b : *this) {
        b = block;
    }
}

Mesh Chunk::tesselate() const {
    MeshBuilder builder{MeshFormat{3, 3, 3}};

    for (auto i = begin(*this); i != end(*this); ++i) {
        if (i->isAir())
            continue;

        for (Face f : {Face::RIGHT, Face::LEFT,
                       Face::TOP, Face::BOTTOM,
                       Face::FRONT, Face::BACK}) {
            tesselate_face(builder, i.getPos(), f, (*i)->getFaceTextureNum(f));
        }
    }

    return Mesh{builder};
}

void Chunk::tesselate_face(MeshBuilder &builder, const Pos &pos, Face f, unsigned int texnum) const {
    Pos adj = pos.adjacent(f);
    if (adj.isValid() && !(*this)(adj).isAir())
        return;

    const glm::vec3 bfl{pos.getX(),
                        pos.getY(),
                        pos.getZ()};
    const glm::vec3 bfr = bfl + glm::vec3{1, 0, 0};
    const glm::vec3 bbl = bfl + glm::vec3{0, 1, 0};
    const glm::vec3 bbr = bfl + glm::vec3{1, 1, 0};
    const glm::vec3 tfl = bfl + glm::vec3{0, 0, 1};
    const glm::vec3 tfr = bfl + glm::vec3{1, 0, 1};
    const glm::vec3 tbl = bfl + glm::vec3{0, 1, 1};
    const glm::vec3 tbr = bfl + glm::vec3{1, 1, 1};

    const glm::vec3 tex_bl{0, 1, texnum};
    const glm::vec3 tex_br{1, 1, texnum};
    const glm::vec3 tex_tl{0, 0, texnum};
    const glm::vec3 tex_tr{1, 0, texnum};

    glm::vec3 normal;

    using Idx = MeshBuilder::Index;

    auto vert = [&](const glm::vec3 &pos, const glm::vec3 &tex) -> Idx {
        builder.beginVert();
        builder.append(pos);
        builder.append(normal);
        builder.append(tex);
        return builder.endVert();
    };

    Idx a, b;

    switch (f) {
    case Face::RIGHT:
        normal = glm::vec3{1, 0, 0};
        vert(bfr, tex_bl);
        a = vert(bbr, tex_br);
        b = vert(tfr, tex_tl);
        builder.repeatVert(a);
        vert(tbr, tex_tr);
        builder.repeatVert(b);
        break;

    case Face::LEFT:
        normal = glm::vec3{-1, 0, 0};
        vert(bfl, tex_br);
        a = vert(tfl, tex_tr);
        b = vert(bbl, tex_bl);
        builder.repeatVert(a);
        vert(tbl, tex_tl);
        builder.repeatVert(b);
        break;

    case Face::BACK:
        normal = glm::vec3{0, 1, 0};
        vert(bbl, tex_br);
        a = vert(tbl, tex_tr);
        b = vert(bbr, tex_bl);
        builder.repeatVert(a);
        vert(tbr, tex_tl);
        builder.repeatVert(b);
        break;

    case Face::FRONT:
        normal = glm::vec3{0, -1, 0};
        vert(bfl, tex_bl);
        a = vert(bfr, tex_br);
        b = vert(tfl, tex_tl);
        builder.repeatVert(a);
        vert(tfr, tex_tr);
        builder.repeatVert(b);
        break;

    case Face::TOP:
        normal = glm::vec3{0, 0, 1};
        vert(tfl, tex_bl);
        a = vert(tfr, tex_br);
        b = vert(tbl, tex_tl);
        builder.repeatVert(a);
        vert(tbr, tex_tr);
        builder.repeatVert(b);
        break;

    case Face::BOTTOM:
        normal = glm::vec3{0, 0, -1};
        vert(bfl, tex_tl);
        a = vert(bbl, tex_bl);
        b = vert(bfr, tex_tr);
        builder.repeatVert(a);
        vert(bbr, tex_br);
        builder.repeatVert(b);
        break;
    }
}

boost::optional<std::pair<Chunk::Pos, Face>> Chunk::pick(const glm::vec3 &startpos,
                                                         const glm::vec3 &dir,
                                                         float maxdist) {
    static constexpr float stepsize = .1;
    Pos prev_pos{0, 0, 0};

    for (unsigned int step = 0;
         step < static_cast<unsigned int>(maxdist / stepsize);
         step++) {
        Pos pos = startpos + (step * stepsize) * dir;

        if (!pos.isValid()) {
            return {};
        } else if (!(*this)(pos).isAir()) {
            auto face = pos.sharedFace(prev_pos);

            return std::make_pair(pos, get_optional_value_or(face, Face::BOTTOM));
        }

        prev_pos = pos;
    }

    return {};
}

Chunk::Pos Chunk::Pos::adjacent(Face f) const {
    switch (f) {
    case Face::RIGHT:
        return {x+1, y, z};
    case Face::LEFT:
        return {x-1, y, z};
    case Face::BACK:
        return {x, y+1, z};
    case Face::FRONT:
        return {x, y-1, z};
    case Face::TOP:
        return {x, y, z+1};
    case Face::BOTTOM:
        return {x, y, z-1};
    default:
        return {x, y, z};
    }
}

boost::optional<Face> Chunk::Pos::sharedFace(const Pos &pos) const {
    int dx = x - pos.getX();
    int dy = y - pos.getY();
    int dz = z - pos.getZ();

    int zeros = (dx == 0) + (dy == 0) + (dz == 0);
    if (zeros != 2) {
        return {};
    }

    if (dx == -1) {
        return Face::LEFT;
    } else if (dx == 1) {
        return Face::RIGHT;
    } else if (dy == -1) {
        return Face::BACK;
    } else if (dy == 1) {
        return Face::FRONT;
    } else if (dz == -1) {
        return Face::BOTTOM;
    } else if (dz == 1) {
        return Face::TOP;
    }

    return {};
}

Chunk::Pos Chunk::Pos::next() const {
    if (x+1 < XSize) {
        return {x+1, y, z};
    } else if (y+1 < YSize) {
        return {0, y+1, z};
    } else {
        return {0, 0, z+1};
    }
}

bool Chunk::Pos::isValid() const {
    return x >= 0 && x < XSize && y >= 0 && y < YSize && z >= 0 && z < ZSize;
}

std::ostream &operator<<(std::ostream &os, const Chunk::Pos &pos) {
    os << "[" << pos.getX() << ", " << pos.getY() << ", " << pos.getZ() << "]";
    return os;
}
