#include "Mesh.h"
#include <algorithm>
#include <cassert>
#include <GL/glew.h>

MeshFormat::MeshFormat(const std::initializer_list<unsigned int> &attr_lengths) :
    attr_lengths(attr_lengths),
    vert_size(std::accumulate(begin(attr_lengths), end(attr_lengths), 0)) { }

MeshBuilder::MeshBuilder(const MeshFormat &meshformat) :
    vert_size(0),
    next_index(0),
    format(meshformat) { }

void MeshBuilder::beginVert() { }

MeshBuilder::Index MeshBuilder::endVert() {
    assert(vert_size == format.getVertexSize());
    vert_size = 0;

    ibuf.push_back(next_index);
    return next_index++;
}

void MeshBuilder::repeatVert(Index index) {
    assert(index < next_index);

    ibuf.push_back(index);
}

Mesh::Mesh(const MeshBuilder &builder) :
    verts(builder.getVertexCount()),
    format(builder.getFormat()),
    vao(VertexArrayObject::generate()),
    buf(builder.getBuffer()),
    ibuf(builder.getIndexBuffer(), Buffer::ELEMENTS)
{
    glBindVertexArray(vao.getID());
    glBindBuffer(GL_ARRAY_BUFFER, buf.getID());

    const unsigned int vert_size = format.getVertexSize();
    unsigned int offset = 0;
    for (unsigned int attrib=0; attrib < format.getAttributeCount(); attrib++) {
        const unsigned int size = format.getAttributeSize(attrib);
        glEnableVertexAttribArray(attrib);
        glVertexAttribPointer(attrib, size,
                              GL_FLOAT, GL_FALSE, vert_size*4,
                              reinterpret_cast<void *>(offset*4));
        offset += size;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf.getID());

    glBindVertexArray(0);
}

void Mesh::draw() const {
    glBindVertexArray(vao.getID());
    glDrawElements(GL_TRIANGLES, verts, GL_UNSIGNED_INT, reinterpret_cast<void *>(0));
    glBindVertexArray(0);
}
