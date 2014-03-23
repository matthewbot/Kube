#ifndef BLOCKVISUAL_H
#define BLOCKVISUAL_H

#include "util/Face.h"
#include "gfx/Mesh.h"
#include "gfx/TextureArrayBuilder.h"
#include "Chunk.h"
#include <memory>

class BlockVisual;

struct BlockVisualInfo {
    virtual ~BlockVisualInfo() { }

    virtual std::unique_ptr<BlockVisual> buildVisual(TextureArrayBuilder &block_tex_builder) const=0;
};

template <typename Self, typename BV>
struct BaseBlockVisualInfo : public BlockVisualInfo {
    virtual std::unique_ptr<BlockVisual> buildVisual(TextureArrayBuilder &block_tex_builder) const {
        return std::unique_ptr<BlockVisual>{new BV(
                static_cast<const Self &>(*this),
                block_tex_builder)};
    }
};

class BlockVisualRegistry;

class BlockVisual {
public:
    virtual ~BlockVisual() { }

    // TODO don't give entire chunk, instead give adjacent blocks
    // and their visuals directly
    virtual void tesselate(MeshBuilder &builder,
                           const BlockVisualRegistry &visuals,
                           const Chunk &chunk,
                           const ChunkIndex &pos,
                           const Block &block) const = 0;
    virtual bool isTransparent() const = 0;
};

#endif
