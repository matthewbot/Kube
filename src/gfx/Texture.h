#ifndef TEXTURE_H
#define TEXTURE_H

#include "gfx/Image.h"
#include "util/IDBase.h"

#include <GL/glew.h>

class BaseTexture : public IDBase<BaseTexture> {
    friend class IDBase<BaseTexture>;
public:
    BaseTexture() { }

protected:
    void genId();
    void genMipMaps();

private:
    void deleteId();
};

class Texture : public BaseTexture {
public:
    Texture() : width(0), height(0) { }
    Texture(const Image &img) { setImage(img); }

    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }
    
    void setImage(const Image &img);

    void bind(unsigned int pos) const;

private:
    unsigned int width, height;
};

class ArrayTexture : public BaseTexture {
public:
    ArrayTexture() : width(0), height(0), layers(0) { }
    ArrayTexture(const Image &img, unsigned int layers) { setImage(img, layers); }

    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }
    unsigned int getLayers() const { return layers; }
    
    void setImage(const Image &img, unsigned int layers);

    void bind(unsigned int pos) const;

private:
    unsigned int width, height;
    unsigned int layers;
};

class Sampler : public IDBase<Sampler> {
    friend class IDBase<Sampler>;
public:
    enum Filter {
        NEAREST,
        LINEAR,
        MIPMAP
    };

    Sampler(Filter filter=LINEAR, bool wrap=false);

    void setFilter(Filter linear);
    void setWrap(bool wrap);

    void bind(unsigned int pos) const;
    
private:
    void deleteId();
};

#endif
