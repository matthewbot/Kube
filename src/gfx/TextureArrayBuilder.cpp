#include "TextureArrayBuilder.h"
#include <cassert>
#include <fstream>

TextureArrayBuilder::TextureArrayBuilder(unsigned int w, unsigned int h) : w(w), h(h) { }

unsigned int TextureArrayBuilder::addImage(const std::string &filename) {
    auto iter = image_texnums.find(filename);
    if (iter != image_texnums.end()) {
        return iter->second;
    }
    
    std::ifstream file(filename.c_str());
    Image newimage = Image::loadPNG(file);
    assert(newimage.getWidth() == w);
    assert(newimage.getHeight() == h);

    auto pos = image.getHeight();
    image.resize(w, pos + h);
    image.blit(newimage, 0, pos);

    unsigned int num = pos / h;
    image_texnums[filename] = num;
    return num;
}

ArrayTexture TextureArrayBuilder::build() const {
    return ArrayTexture{image, image.getHeight() / h};
}
