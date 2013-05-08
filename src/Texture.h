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
    Texture() { }
    Texture(const Image &img) { setImage(img); }

    void setImage(const Image &img);

private:
    void bind() const;
};

class ArrayTexture : public BaseTexture {
    friend class Renderer;
public:
    ArrayTexture() { }
    ArrayTexture(const Image &img, unsigned int layers) { setImage(img, layers); }

    void setImage(const Image &img, unsigned int layers);

private:
    void bind() const;
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

private:
    void deleteId();
};

#endif
