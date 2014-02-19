#ifndef TEXTUREARRAYBUILDER_H
#define TEXTUREARRAYBUILDER_H

#include "gfx/Image.h"
#include "gfx/Texture.h"

class TextureArrayBuilder {
public:
    TextureArrayBuilder(int w, int h);

    unsigned int addImage(const Image &newimage);
    
    ArrayTexture build() const;
    
private:
    int w;
    int h;
    Image image;
};

#endif
