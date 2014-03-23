#include "PlantBlockVisual.h"

PlantBlockVisual::PlantBlockVisual(const PlantBlockVisualInfo &info,
                                   TextureArrayBuilder &block_tex_builder) {
    tex = block_tex_builder.addImage(info.tex_filename);
}

void PlantBlockVisual::tesselate(MeshBuilder &builder,
                                 const BlockVisualRegistry &visuals,
                                 const Chunk &chunk,
                                 const ChunkIndex &pos,
                                 const Block &block) const {
    const glm::vec3 bfl{pos.getVec()};
    const glm::vec3 bfr = bfl + glm::vec3{1, 0, 0};
    const glm::vec3 bbl = bfl + glm::vec3{0, 1, 0};
    const glm::vec3 bbr = bfl + glm::vec3{1, 1, 0};
    const glm::vec3 tfl = bfl + glm::vec3{0, 0, 1};
    const glm::vec3 tfr = bfl + glm::vec3{1, 0, 1};
    const glm::vec3 tbl = bfl + glm::vec3{0, 1, 1};
    const glm::vec3 tbr = bfl + glm::vec3{1, 1, 1};

    const glm::vec3 tex_bl{0, 0, tex};
    const glm::vec3 tex_br{1, 0, tex};
    const glm::vec3 tex_tl{0, 1, tex};
    const glm::vec3 tex_tr{1, 1, tex};

    auto normal = glm::normalize(glm::vec3{1, -1, 0});
    MeshBuilder::Index a;
    MeshBuilder::Index b;
    
    builder.makeVert(bfl, normal, tex_bl);
    a = builder.makeVert(bbr, normal, tex_br);
    b = builder.makeVert(tfl, normal, tex_tl);
    builder.repeatVert(a);
    builder.makeVert(tbr, normal, tex_tr);
    builder.repeatVert(b);

    a = builder.makeVert(bbr, -normal, tex_bl);
    builder.makeVert(bfl, -normal, tex_br);
    b = builder.makeVert(tfl, -normal, tex_tr);
    builder.repeatVert(a);
    builder.repeatVert(b);
    builder.makeVert(tbr, -normal, tex_tl);

    normal = glm::normalize(glm::vec3{-1, -1, 0});
    
    builder.makeVert(bbl, normal, tex_bl);
    a = builder.makeVert(bfr, normal, tex_br);
    b = builder.makeVert(tbl, normal, tex_tl);
    builder.repeatVert(a);
    builder.makeVert(tfr, normal, tex_tr);
    builder.repeatVert(b);

    a = builder.makeVert(bfr, -normal, tex_bl);
    builder.makeVert(bbl, -normal, tex_br);
    b = builder.makeVert(tbl, -normal, tex_tr);
    builder.repeatVert(a);
    builder.repeatVert(b);
    builder.makeVert(tfr, -normal, tex_tl);
}
