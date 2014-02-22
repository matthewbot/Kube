#include "BlockTypeRegistry.h"
#include <stdexcept>

const BlockType &BlockTypeRegistry::makeType(
    const std::string &name,
    BlockTypeInfo info) 
{
    BlockType::ID id = types_by_id.size();
    types_by_id.emplace_back(id, name, std::move(info));
    ids_by_name.emplace(name, id);
    return types_by_id.back();
}

const BlockType &BlockTypeRegistry::getType(const std::string &name) const {
    auto iter = ids_by_name.find(name);
    if (iter == std::end(ids_by_name))
        throw std::runtime_error("No BlockType named " + name);
    return types_by_id[iter->second];
}

const BlockType &BlockTypeRegistry::getType(BlockType::ID id) const {
    if (static_cast<unsigned int>(id) >= types_by_id.size()) {
        throw std::runtime_error("Bad BlockType ID");
    } else {
        return types_by_id[id];
    }
}

void BlockTypeRegistry::dump(std::ostream &out) const {
    for (auto &btype : types_by_id) {
        out << btype.id << ' '
            << btype.name << ' '
            << std::endl;
    }
}
