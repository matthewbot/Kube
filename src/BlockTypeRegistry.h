#ifndef BLOCKTYPEREGISTRY_H
#define BLOCKTYPEREGISTRY_H

#include "BlockType.h"
#include <string>
#include <vector>

class BlockTypeRegistry {
public:
    const BlockType &makeType(const std::string &name, BlockTypeInfo info);

    const BlockType &getType(const std::string &name) const;
    const BlockType &getType(BlockType::ID id) const;
    
private:
    std::vector<BlockType> types_by_id;
    std::unordered_map<std::string, BlockType::ID> ids_by_name;    
};

#endif
