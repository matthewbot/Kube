#include "Texture.h"
#include "GL/glew.h"

#include <iostream>

void BaseTexture::genId() {
   if (!id) {
        GLuint tmp;
        glGenTextures(1, &tmp);
        id = tmp;
    }
}

void BaseTexture::deleteId() {
    GLuint tmp = id;
    glDeleteTextures(1, &tmp);
}

void Texture::setImage(const Image &img) {
    genId();
    bind();
    glTexImage2D(GL_TEXTURE_2D, 0, img.getGamma() < 1 ? GL_SRGB8_ALPHA8 : GL_RGBA8,
                 img.getWidth(), img.getHeight(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getData());
    glGenerateMipmap(GL_TEXTURE_2D);
    width = img.getWidth();
    height = img.getHeight();
}

void Texture::bind() const {
    glBindTexture(GL_TEXTURE_2D, id);
}

void ArrayTexture::setImage(const Image &img, unsigned int layers) {
    genId();
    bind();
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, img.getGamma() < 1 ? GL_SRGB8_ALPHA8 : GL_RGBA8,
                 img.getWidth(), img.getHeight()/layers, layers,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getData());
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    width = img.getWidth();
    height = img.getHeight() / layers;
    this->layers = layers;
}

void ArrayTexture::bind() const {
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);
}

Sampler::Sampler(Filter filter, bool wrap) {
    GLuint tmp;
    glGenSamplers(1, &tmp);
    id = tmp;

    setFilter(filter);
    setWrap(wrap);
}

void Sampler::setFilter(Filter filter) {
    static const GLint gl_magfilters[] = { GL_NEAREST, GL_LINEAR, GL_LINEAR };
    static const GLint gl_minfilters[] = { GL_NEAREST, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR };

    glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, gl_magfilters[filter]);
    glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, gl_minfilters[filter]);
}

void Sampler::setWrap(bool wrap) {
    GLint glwrap = wrap ? GL_REPEAT : GL_CLAMP_TO_EDGE;
    glSamplerParameteri(id, GL_TEXTURE_WRAP_S, glwrap);
    glSamplerParameteri(id, GL_TEXTURE_WRAP_T, glwrap);

}

void Sampler::deleteId() {
    GLuint tmp = id;
    glDeleteSamplers(1, &tmp);
}
