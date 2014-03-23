#include "SimpleBlockVisual.h"
#include "BlockVisualRegistry.h"

SimpleBlockVisual::SimpleBlockVisual(const SimpleBlockVisualInfo &info,
                                     TextureArrayBuilder &block_tex_builder) {
    for (Face f : all_faces) {
        face_texes[f] = block_tex_builder.addImage(info.face_tex_filenames[f]);
    }
}

void SimpleBlockVisual::tesselate(MeshBuilder &builder,
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
    
    for (auto face : all_faces) {
        if (block.getType().solid) {
            auto adjpos = pos.adjacent(face);
            if (adjpos) {
                auto visualptr = visuals.getVisual(chunk.getBlock(adjpos).getType().id);
                if (visualptr && !visualptr->isTransparent()) {
                    continue;
                }
            }
        }
            
        const auto texnum = face_texes[face];
        const glm::vec3 tex_bl{0, 0, texnum};
        const glm::vec3 tex_br{1, 0, texnum};
        const glm::vec3 tex_tl{0, 1, texnum};
        const glm::vec3 tex_tr{1, 1, texnum};

        auto normal = static_cast<glm::vec3>(faceNormal(face));
        MeshBuilder::Index a;
        MeshBuilder::Index b;

        switch (face) {
        case Face::RIGHT:
            builder.makeVert(bfr, normal, tex_bl);
            a = builder.makeVert(bbr, normal, tex_br);
            b = builder.makeVert(tfr, normal, tex_tl);
            builder.repeatVert(a);
            builder.makeVert(tbr, normal, tex_tr);
            builder.repeatVert(b);
            break;

        case Face::LEFT:
            builder.makeVert(bfl, normal, tex_br);
            a = builder.makeVert(tfl, normal, tex_tr);
            b = builder.makeVert(bbl, normal, tex_bl);
            builder.repeatVert(a);
            builder.makeVert(tbl, normal, tex_tl);
            builder.repeatVert(b);
            break;

        case Face::BACK:
            builder.makeVert(bbl, normal, tex_br);
            a = builder.makeVert(tbl, normal, tex_tr);
            b = builder.makeVert(bbr, normal, tex_bl);
            builder.repeatVert(a);
            builder.makeVert(tbr, normal, tex_tl);
            builder.repeatVert(b);
            break;

        case Face::FRONT:
            builder.makeVert(bfl, normal, tex_bl);
            a = builder.makeVert(bfr, normal, tex_br);
            b = builder.makeVert(tfl, normal, tex_tl);
            builder.repeatVert(a);
            builder.makeVert(tfr, normal, tex_tr);
            builder.repeatVert(b);
            break;

        case Face::TOP:
            builder.makeVert(tfl, normal, tex_bl);
            a = builder.makeVert(tfr, normal, tex_br);
            b = builder.makeVert(tbl, normal, tex_tl);
            builder.repeatVert(a);
            builder.makeVert(tbr, normal, tex_tr);
            builder.repeatVert(b);
            break;

        case Face::BOTTOM:
            builder.makeVert(bfl, normal, tex_tl);
            a = builder.makeVert(bbl, normal, tex_bl);
            b = builder.makeVert(bfr, normal, tex_tr);
            builder.repeatVert(a);
            builder.makeVert(bbr, normal, tex_br);
            builder.repeatVert(b);
            break;
        }
    }
}

