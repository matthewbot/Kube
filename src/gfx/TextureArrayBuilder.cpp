#include "TextureArrayBuilder.h"
#include <cassert>

TextureArrayBuilder::TextureArrayBuilder(unsigned int w, unsigned int h) : w(w), h(h) { }

unsigned int TextureArrayBuilder::addImage(const Image &newimage) {
    assert(newimage.getWidth() == w);
    assert(newimage.getHeight() == h);

    auto pos = image.getHeight();
    image.resize(w, pos + h);
    image.blit(newimage, 0, pos);
    return pos / h;
}

ArrayTexture TextureArrayBuilder::build() const {
    return ArrayTexture{image, image.getHeight() / h};
}
