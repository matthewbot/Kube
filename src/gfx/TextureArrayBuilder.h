#ifndef TEXTUREARRAYBUILDER_H
#define TEXTUREARRAYBUILDER_H

#include "gfx/Image.h"
#include "gfx/Texture.h"
#include <unordered_map>

// TODO ArrayTextureBuilder
class TextureArrayBuilder {
public:
    TextureArrayBuilder(unsigned int w, unsigned int h);

    // TODO resource system
    unsigned int addImage(const std::string &filename);
    
    ArrayTexture build() const;
    
private:
    unsigned int w;
    unsigned int h;
    Image image;

    std::unordered_map<std::string, unsigned int> image_texnums;
};

#endif
