#include "BlockVisualRegistry.h"

BlockVisualRegistry::BlockVisualRegistry(unsigned int block_tex_size) :
    block_tex_builder(block_tex_size, block_tex_size)
{ }

const BlockVisual &BlockVisualRegistry::makeVisual(BlockType::ID id, const BlockVisualInfo &info) {
    if (id >= visuals.size()) {
        visuals.resize(id+1);
    }

    return *(visuals[id] = info.buildVisual(block_tex_builder));
}

const BlockVisual *BlockVisualRegistry::getVisual(BlockType::ID id) const {
    if (id >= visuals.size()) {
        return nullptr;
    } else {
        return visuals[id].get();
    }
}

bool BlockVisualRegistry::hasVisual(BlockType::ID id) const {
    return getVisual(id) != nullptr;
}

void BlockVisualRegistry::prepareTesselate() {
    block_tex = block_tex_builder.build();
}

void BlockVisualRegistry::tesselate(MeshBuilder &builder, const Chunk &chunk) const {
    builder.reset(MeshFormat{3, 3, 3});

    for (auto &pos : ChunkIndex::range) {
        auto block = chunk.getBlock(pos);
        auto visualptr = getVisual(block.getType().id);
        if (visualptr) {
            visualptr->tesselate(builder, *this, chunk, pos, block);
        }
    }
}
