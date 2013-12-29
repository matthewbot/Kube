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
    if (data[offset] == 0) {
        return {};
    }

    auto typeptr = reg->getType(data[offset]);
    assert(typeptr);
    return {*typeptr};
}

void Chunk::setBlock(unsigned int offset, const Block &block) {
    assert(offset < data.size());
    data[offset] = block.getID();
}

void Chunk::fill(const Block &block) {
    data.fill(block.getID());
}

void Chunk::tesselate(MeshBuilder &builder) const {
    builder.reset(MeshFormat{3, 3, 3});

    for (auto &pos : ChunkIndex::Range) {
        // TODO: isVisible() or something
        if (getBlock(pos).isAir()) {
            continue;
        }

        for (Face f : all_faces) {
            tesselate_face(builder, pos, f);
        }
    }
}

void Chunk::tesselate_face(MeshBuilder &builder, const ChunkIndex &pos, Face face) const {
    auto adjpos = pos.adjacent(face);
    if (adjpos && !getBlock(adjpos).isAir()) {
        return;
    }

    auto block = getBlock(pos);
    auto &info = block.getType().getInfo();

    const glm::vec3 bfl{pos.getVec()};
    const glm::vec3 bfr = bfl + glm::vec3{1, 0, 0};
    const glm::vec3 bbl = bfl + glm::vec3{0, 1, 0};
    const glm::vec3 bbr = bfl + glm::vec3{1, 1, 0};
    const glm::vec3 tfl = bfl + glm::vec3{0, 0, 1};
    const glm::vec3 tfr = bfl + glm::vec3{1, 0, 1};
    const glm::vec3 tbl = bfl + glm::vec3{0, 1, 1};
    const glm::vec3 tbr = bfl + glm::vec3{1, 1, 1};

    const unsigned int texnum = info.getFaceTextureNum(face);
    const glm::vec3 tex_bl{0, 0, texnum};
    const glm::vec3 tex_br{1, 0, texnum};
    const glm::vec3 tex_tl{0, 1, texnum};
    const glm::vec3 tex_tr{1, 1, texnum};

    glm::vec3 normal;

    using Idx = MeshBuilder::Index;

    // Todo, can generate most of this with template wizardry
    auto vert = [&](const glm::vec3 &pos, const glm::vec3 &tex) -> Idx {
        builder.append(pos);
        builder.append(normal);
        builder.append(tex);
        return builder.finishVert();
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
