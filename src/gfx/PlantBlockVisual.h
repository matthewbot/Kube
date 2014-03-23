#ifndef PLANTBLOCKVISUAL_H
#define PLANTBLOCKVISUAL_H

#include "BlockVisual.h"

class PlantBlockVisual;

struct PlantBlockVisualInfo : public BaseBlockVisualInfo<PlantBlockVisualInfo, PlantBlockVisual> {
    std::string tex_filename;
};

class PlantBlockVisual : public BlockVisual {
public:
    PlantBlockVisual(const PlantBlockVisualInfo &info,
                     TextureArrayBuilder &block_tex_builder);

    virtual void tesselate(MeshBuilder &builder,
                           const BlockVisualRegistry &visuals,
                           const Chunk &chunk,
                           const ChunkIndex &pos,
                           const Block &block) const;

    virtual bool isTransparent() const { return true; }
private:
    unsigned int tex;
};

#endif
