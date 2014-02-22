#include "tesselate.h"
#include "Chunk.h"
#include "gfx/Font.h"

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
