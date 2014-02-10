#include "tesselate.h"
#include "Chunk.h"
#include "gfx/Font.h"

static void tesselate_cube(MeshBuilder &builder,
                           const Chunk &chunk,
                           const ChunkIndex &pos,
                           const Block &block);

void tesselate(MeshBuilder &builder, const Chunk &chunk) {
    builder.reset(MeshFormat{3, 3, 3});

    for (auto &pos : ChunkIndex::range) {
        auto block = chunk.getBlock(pos);
        if (!block.getType().visible) {
            continue;
        }

        tesselate_cube(builder, chunk, pos, block);
    }
}

static void tesselate_cube(MeshBuilder &builder,
                           const Chunk &chunk,
                           const ChunkIndex &pos,
                           const Block &block) {
    const auto &type = block.getType();

    const glm::vec3 bfl{pos.getVec()};
    const glm::vec3 bfr = bfl + glm::vec3{1, 0, 0};
    const glm::vec3 bbl = bfl + glm::vec3{0, 1, 0};
    const glm::vec3 bbr = bfl + glm::vec3{1, 1, 0};
    const glm::vec3 tfl = bfl + glm::vec3{0, 0, 1};
    const glm::vec3 tfr = bfl + glm::vec3{1, 0, 1};
    const glm::vec3 tbl = bfl + glm::vec3{0, 1, 1};
    const glm::vec3 tbr = bfl + glm::vec3{1, 1, 1};
    
    for (auto face : all_faces) {
        if (type.solid) {
            auto adjpos = pos.adjacent(face);
            if (adjpos && chunk.getBlock(adjpos).getType().solid) {
                continue;
            }
        }
            
        const auto texnum = type.face_texes[face];
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

void tesselate(MeshBuilder &builder,
               const Font &font,
               const std::string &str) {
    builder.reset(MeshFormat{2, 2});

    const float texwidth = font.getTexture().getWidth();
    const float texheight = font.getTexture().getHeight();

    int curx = 0;
    for (size_t i = 0; i < str.size(); i++) {
        auto propsptr = font.getProps(str[i]);
        if (!propsptr)
            continue;

        const float xmin = curx + propsptr->xoffset;
        const float xmax = xmin + propsptr->width;
        const float ymax = font.getBaseHeight() - propsptr->yoffset;
        const float ymin = ymax - propsptr->height;
        const float txmin = propsptr->x / texwidth;
        const float txmax = txmin + propsptr->width / texwidth;
        const float tymax = 1 - propsptr->y / texheight;
        const float tymin = tymax - propsptr->height / texheight;

        builder.makeVert(glm::vec4{xmin, ymin, txmin, tymin});
        MeshBuilder::Index a = builder.makeVert(glm::vec4{xmax, ymin, txmax, tymin});
        MeshBuilder::Index b = builder.makeVert(glm::vec4{xmin, ymax, txmin, tymax});
        builder.repeatVert(a);
        builder.makeVert(glm::vec4{xmax, ymax, txmax, tymax});
        builder.repeatVert(b);

        curx += propsptr->xadvance;
    }
}
