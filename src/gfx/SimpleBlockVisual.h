#ifndef SIMPLEBLOCKVISUAL_H
#define SIMPLEBLOCKVISUAL_H

#include "BlockVisual.h"

class SimpleBlockVisual;

struct SimpleBlockVisualInfo : public BaseBlockVisualInfo<SimpleBlockVisualInfo, SimpleBlockVisual> {
    SimpleBlockVisualInfo() { }
    SimpleBlockVisualInfo(const std::string &filename) { face_tex_filenames.fill(filename); }
    FaceMap<std::string> face_tex_filenames;
};

class SimpleBlockVisual : public BlockVisual {
public:
    SimpleBlockVisual(const SimpleBlockVisualInfo &info,
                      TextureArrayBuilder &block_tex_builder);

    virtual void tesselate(MeshBuilder &builder,
                           const BlockVisualRegistry &visuals,
                           const Chunk &chunk,
                           const ChunkIndex &pos,
                           const Block &block) const;

    virtual bool isTransparent() const { return false; }
private:
    FaceMap<unsigned int> face_texes;
};

#endif
