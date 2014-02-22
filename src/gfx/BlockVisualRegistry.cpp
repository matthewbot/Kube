#include "BlockVisualRegistry.h"

BlockVisualRegistry::BlockVisualRegistry(ArrayTexture tex, BlockType::ID maxid) :
    tex(std::move(tex)),
    entries(maxid)
{ }

void BlockVisualRegistry::updateTexture(ArrayTexture tex) {
    this->tex = std::move(tex);
}

void BlockVisualRegistry::setVisual(BlockType::ID id, std::unique_ptr<BlockVisual> visual) {
    if (id >= entries.size()) {
        entries.resize(id);
    }

    entries[id] = std::move(visual);
}

bool BlockVisualRegistry::hasVisual(BlockType::ID id) const {
    return getVisual(id) != nullptr;
}

const BlockVisual *BlockVisualRegistry::getVisual(BlockType::ID id) const {
    if (id >= entries.size()) {
        return nullptr;
    } else {
        return entries[id].get();
    }
}

void BlockVisualRegistry::tesselate(MeshBuilder &builder, const Chunk &chunk) {
    builder.reset(MeshFormat{3, 3, 3});

    for (auto &pos : ChunkIndex::range) {
        auto block = chunk.getBlock(pos);
        auto visualptr = getVisual(block.getType().id);
        if (visualptr) {
            visualptr->tesselate(builder, chunk, pos, block);
        }
    }
}

