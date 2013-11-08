#include "gfx/VertexArrayObject.h"
#include <GL/glew.h>

#include <iostream>

VertexArrayObject VertexArrayObject::generate() {
    GLuint tmp;
    glGenVertexArrays(1, &tmp);
    return {tmp};
}

void VertexArrayObject::deleteId() {
    GLuint tmp = id;
    glDeleteVertexArrays(1, &tmp);
}
