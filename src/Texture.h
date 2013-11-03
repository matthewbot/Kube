#ifndef TEXTURE_H
#define TEXTURE_H

#include "Image.h"
#include "util.h"

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
    friend class Renderer;
public:
    Texture() : width(0), height(0) { }
    Texture(const Image &img) { setImage(img); }

    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }
    
    void setImage(const Image &img);

private:
    void bind() const;

    unsigned int width, height;
};

class ArrayTexture : public BaseTexture {
    friend class Renderer;
public:
    ArrayTexture() { }
    ArrayTexture(const Image &img, unsigned int layers) { setImage(img, layers); }

    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }
    unsigned int getLayers() const { return layers; }
    
    void setImage(const Image &img, unsigned int layers);

private:
    void bind() const;

    unsigned int width, height;
    unsigned int layers;
};

class Sampler : public IDBase<Sampler> {
    friend class IDBase<Sampler>;
    friend class Renderer;
public:
    enum Filter {
        NEAREST,
        LINEAR,
        MIPMAP
    };

    Sampler(Filter filter=LINEAR, bool wrap=false);

    void setFilter(Filter linear);
    void setWrap(bool wrap);

private:
    void bind(unsigned int pos) const;
    
    void deleteId();
};

#endif
