#ifndef BLOCK_H
#define BLOCK_H

#include "BlockType.h"
#include <cstdint>

class Block {
public:
    Block(const BlockType &type) : type(&type) { }

    bool operator==(const Block &block) { return *type == *block.type; }
    bool operator!=(const Block &block) { return !(*this == block); }

    const BlockType &getType() const { return *type; }
    BlockType::ID getID() const { return type->id; }
    
private:
    const BlockType *type;
};

#endif
