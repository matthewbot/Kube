#include "Buffer.h"
#include "GL/glew.h"

#include <iostream>

void Buffer::setData(const void *data, size_t len, Type type) {
    if (!id) {
        GLuint tmp;
        glGenBuffers(1, &tmp);
        id = tmp;
    }

    GLenum target = (type == ARRAY) ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;

    glBindBuffer(target, id);
    glBufferData(target, len, data, GL_STATIC_DRAW);
    glBindBuffer(target, 0);
}

void Buffer::deleteId() {
    GLuint tmp = id;
    glDeleteBuffers(1, &tmp);
}
