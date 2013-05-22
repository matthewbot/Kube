#ifndef MESH_H
#define MESH_H

#include "Buffer.h"
#include "VertexArrayObject.h"

#include <glm/glm.hpp>
#include <initializer_list>
#include <vector>

class MeshFormat {
public:
    MeshFormat() { }
    explicit MeshFormat(const std::initializer_list<unsigned int> &attr_lengths);

    unsigned int getVertexSize() const { return vert_size; }

    unsigned int getAttributeCount() const { return attr_lengths.size(); }
    unsigned int getAttributeSize(unsigned int attribute) const { return attr_lengths[attribute]; }

private:
    std::vector<unsigned int> attr_lengths;
    unsigned int vert_size;
};

class MeshBuilder {
public:
    using Index = unsigned int;

    explicit MeshBuilder(const MeshFormat &meshformat);

    void beginVert();
    Index endVert();
    void repeatVert(Index idx);

    template <typename T>
    void append(const T &vals) {
        for (const auto &val : vals)
            append(val);
    }

    void append(float f) { buf.push_back(f); vert_size++; }
    void append(const glm::vec2 &vec) { buf.push_back(vec.x); buf.push_back(vec.y); vert_size += 2; }
    void append(const glm::vec3 &vec) { buf.push_back(vec.x); buf.push_back(vec.y); buf.push_back(vec.z); vert_size += 3; }
    void append(const glm::vec4 &vec) { buf.push_back(vec.x); buf.push_back(vec.y); buf.push_back(vec.z); buf.push_back(vec.w); vert_size += 4; }

    const MeshFormat &getFormat() const { return format; }
    unsigned int getVertexCount() const { return ibuf.size(); }

    const std::vector<float> &getBuffer() const { return buf; }
    const std::vector<Index> &getIndexBuffer() const { return ibuf; }

private:
    unsigned int vert_size;
    Index next_index;

    std::vector<float> buf;
    std::vector<Index> ibuf;

    const MeshFormat format;
};

class Mesh {
public:
    Mesh() : verts(0) { }
    explicit Mesh(const MeshBuilder &builder);

    unsigned int getVertexCount() const { return verts; }
    const MeshFormat &getFormat() const { return format; }

    operator bool() const { return vao; }

    void draw() const;

private:
    unsigned int verts;
    MeshFormat format;

    VertexArrayObject vao;
    Buffer buf;
    Buffer ibuf;
};

#endif
