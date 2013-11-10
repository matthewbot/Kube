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
        if (choose_air(generator) < density) {
            block = blocks[choose_block(generator)];
        } else {
            block = Block::air();
        }
    }

    return chunk;
}

void Chunk::fill(const Block &block) {
    for (auto &b : *this) {
        b = block;
    }
}

MeshBuilder Chunk::tesselate() const {
    MeshBuilder builder{MeshFormat{3, 3, 3}};

    for (auto i = begin(*this); i != end(*this); ++i) {
        if (i->isAir()) {
            continue;
        }

        for (Face f : all_faces) {
            tesselate_face(builder, i.getPos(), f);
        }
    }

    return builder;
}

void Chunk::tesselate_face(MeshBuilder &builder, const glm::ivec3 &pos, Face face) const {
    auto adjpos = adjacentPos(pos, face);
    if (isValid(adjpos) && !getBlock(adjpos).isAir()) {
        return;
    }

    auto &block = getBlock(pos);
    auto &type = block.getType();

    const glm::vec3 bfl{pos};
    const glm::vec3 bfr = bfl + glm::vec3{1, 0, 0};
    const glm::vec3 bbl = bfl + glm::vec3{0, 1, 0};
    const glm::vec3 bbr = bfl + glm::vec3{1, 1, 0};
    const glm::vec3 tfl = bfl + glm::vec3{0, 0, 1};
    const glm::vec3 tfr = bfl + glm::vec3{1, 0, 1};
    const glm::vec3 tbl = bfl + glm::vec3{0, 1, 1};
    const glm::vec3 tbr = bfl + glm::vec3{1, 1, 1};

    const unsigned int texnum = type.getFaceTextureNum(face);
    const glm::vec3 tex_bl{0, 0, texnum};
    const glm::vec3 tex_br{1, 0, texnum};
    const glm::vec3 tex_tl{0, 1, texnum};
    const glm::vec3 tex_tr{1, 1, texnum};

    glm::vec3 normal;

    using Idx = MeshBuilder::Index;

    auto vert = [&](const glm::vec3 &pos, const glm::vec3 &tex) -> Idx {
        builder.beginVert();
        builder.append(pos);
        builder.append(normal);
        builder.append(tex);
        return builder.endVert();
    };

    Idx a;
    Idx b;

    switch (face) {
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

glm::ivec3 Chunk::nextPos(const glm::ivec3 &pos) {
    if (pos.x+1 < XSize) {
        return glm::ivec3{pos.x+1, pos.y, pos.z};
    } else if (pos.y+1 < YSize) {
        return glm::ivec3{0, pos.y+1, pos.z};
    } else {
        return glm::ivec3{0, 0, pos.z+1};
    }
}
