#ifndef BLOCK_H
#define BLOCK_H

#include "util.h"
#include <cstdint>
#include "util.h"

class BlockType {
public:
    BlockType();

    void setFaceTextureNum(Face face, unsigned int texnum) { face_texes[face] = texnum; }
    void setAllFaceTextureNums(unsigned int texnum);

    unsigned int getFaceTextureNum(Face face) const { return face_texes[face]; }

private:
    FaceMap<unsigned int> face_texes;
};

class Block {
public:
    Block(const BlockType &type) : type(&type) { }
    Block() : type(nullptr) { }
    static Block air() { return Block(); }

    bool operator==(const Block &block) { return type == block.type; }
    bool operator!=(const Block &block) { return !(*this == block); }

    bool isAir() const { return type == nullptr; }
    const BlockType &getType() const { return *type; }

private:
    const BlockType *type;
};

#endif

