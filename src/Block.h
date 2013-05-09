#ifndef BLOCK_H
#define BLOCK_H

#include <cstdint>

enum class Face : uint8_t { RIGHT, LEFT, BACK, FRONT, TOP, BOTTOM};

class BlockType {
public:
    BlockType();

    void setFaceTextureNum(Face face, unsigned int texnum) {
        face_texes[static_cast<int>(face)] = texnum;
    }

    void setAllFaceTextureNums(unsigned int texnum);

    unsigned int getFaceTextureNum(Face face) const {
        return face_texes[static_cast<int>(face)];
    }

private:
    unsigned int face_texes[6];
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

