#ifndef BLOCKTYPE_H
#define BLOCKTYPE_H

#include "util/Face.h"
#include <string>
#include <cstdint>
#include <unordered_map>

struct BlockTypeInfo {
    bool solid = true;
};

struct BlockType : public BlockTypeInfo {
    using ID = uint16_t;
    ID id;
    std::string name;

    BlockType(ID id, std::string name, BlockTypeInfo info) :
        BlockTypeInfo(std::move(info)),
        id(id),
        name(std::move(name)) { }

    bool operator==(const BlockType &other) const { return id == other.id; }
    bool operator!=(const BlockType &other) const { return id != other.id; }
};

#endif
