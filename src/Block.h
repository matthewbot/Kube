#ifndef BLOCK_H
#define BLOCK_H

#include "BlockType.h"
#include <cstdint>

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
