#ifndef TEXTUREARRAYBUILDER_H
#define TEXTUREARRAYBUILDER_H

#include "gfx/Image.h"
#include "gfx/Texture.h"

class TextureArrayBuilder {
public:
    TextureArrayBuilder(unsigned int w, unsigned int h);

    unsigned int addImage(const Image &newimage);
    
    ArrayTexture build() const;
    
private:
    unsigned int w;
    unsigned int h;
    Image image;
};

#endif
