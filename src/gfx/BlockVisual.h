#ifndef BLOCKVISUAL_H
#define BLOCKVISUAL_H

#include "util/Face.h"
#include "gfx/Mesh.h"
#include "Chunk.h"

class BlockVisual {
public:
    virtual ~BlockVisual() { }

    virtual void tesselate(MeshBuilder &builder,
                           const Chunk &chunk,
                           const ChunkIndex &pos,
                           const Block &block) const = 0;
};

class SimpleBlockVisual : public BlockVisual {
public:
    SimpleBlockVisual() { }
    SimpleBlockVisual(unsigned int id) { face_texes.fill(id); }

    virtual void tesselate(MeshBuilder &builder,
                           const Chunk &chunk,
                           const ChunkIndex &pos,
                           const Block &block) const;
    
    FaceMap<unsigned int> face_texes;
};

#endif
