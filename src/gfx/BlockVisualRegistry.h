#ifndef BLOCKVISUALREGISTRY_H
#define BLOCKVISUALREGISTRY_H

#include "BlockType.h"
#include "gfx/BlockVisual.h"
#include "gfx/Texture.h"
#include <vector>
#include <memory>

class BlockVisualRegistry {
public:
    BlockVisualRegistry(ArrayTexture tex, BlockType::ID maxid);

    void updateTexture(ArrayTexture tex);

    void setVisual(BlockType::ID id, std::unique_ptr<BlockVisual> visual);
    bool hasVisual(BlockType::ID id) const;
    const BlockVisual *getVisual(BlockType::ID id) const;

    void tesselate(MeshBuilder &builder, const Chunk &chunk);
    
private:
    ArrayTexture tex;
    std::vector<std::unique_ptr<BlockVisual>> entries;
};

#endif
