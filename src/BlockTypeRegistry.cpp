#include "BlockTypeRegistry.h"

const BlockType &BlockTypeRegistry::makeType(
    const std::string &name,
    BlockTypeInfo info) 
{
    BlockType::ID id = types_by_id.size()+1;
    types_by_id.emplace_back(id, name, std::move(info));
    ids_by_name.emplace(name, id);
    return types_by_id.back();
}

const BlockType *BlockTypeRegistry::getType(const std::string &name) const {
    auto iter = ids_by_name.find(name);
    if (iter == std::end(ids_by_name))
        return nullptr;
    return &types_by_id[iter->second-1];
}

const BlockType *BlockTypeRegistry::getType(BlockType::ID id) const {
    if (id == 0 || static_cast<unsigned int>(id-1) >= types_by_id.size()) {
        return nullptr;
    } else {
        return &types_by_id[id-1];
    }
}