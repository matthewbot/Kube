#ifndef BLOCKVISUALREGISTRY_H
#define BLOCKVISUALREGISTRY_H

#include "BlockType.h"
#include "gfx/BlockVisual.h"
#include "gfx/Texture.h"
#include <vector>
#include <memory>

class BlockVisualRegistry {
public:
    BlockVisualRegistry(unsigned int block_tex_size);

    const BlockVisual &makeVisual(BlockType::ID id, const BlockVisualInfo &info);
    const BlockVisual *getVisual(BlockType::ID id) const;
    bool hasVisual(BlockType::ID id) const;
    
    void prepareTesselate();
    void tesselate(MeshBuilder &builder, const Chunk &chunk) const;

    // TODO, put textures into meshes
    const ArrayTexture &getBlockTex() const { return block_tex; }
    
private:
    TextureArrayBuilder block_tex_builder;
    ArrayTexture block_tex;
    std::vector<std::unique_ptr<const BlockVisual>> visuals;
};

#endif
