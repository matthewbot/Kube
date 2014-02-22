#ifndef MESH_H
#define MESH_H

#include "gfx/Buffer.h"
#include "gfx/VertexArrayObject.h"
#include "gfx/Shader.h"

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

class Mesh {
public:
    Mesh() : vertcount(0) { }
    Mesh(unsigned int vertcount, const MeshFormat &format, Buffer buf, Buffer ibuf);

    unsigned int getVertexCount() const { return vertcount; }
    const MeshFormat &getFormat() const { return format; }

    explicit operator bool() const { return !!vao; }

    void draw(const ShaderProgram &prgm) const;

private:
    unsigned int vertcount;
    MeshFormat format;

    VertexArrayObject vao;
    Buffer buf;
    Buffer ibuf;
};

class MeshBuilder {
public:
    using Index = unsigned int;

    MeshBuilder();
    explicit MeshBuilder(MeshFormat format) : MeshBuilder() {
	reset(std::move(format));
    }

    void reset(MeshFormat format);

    Index finishVert();
    void repeatVert(Index idx);

    template <typename T>
    void append(const T &vals) {
        for (const auto &val : vals)
            append(val);
    }

    void append(float f);
    void append(const glm::vec2 &vec);
    void append(const glm::vec3 &vec);
    void append(const glm::vec4 &vec);

    template <typename Val>
    Index makeVert(const Val &val) {
        append(val);
        return finishVert();
    }
    
    template <typename Val, typename... Vals>
    Index makeVert(const Val &val, const Vals&... vals) {
        append(val);
        return makeVert(vals...);
    }
    
    const MeshFormat &getFormat() const { return format; }
    unsigned int getVertexCount() const { return ibuf.size(); }

    const std::vector<float> &getBuffer() const { return buf; }
    const std::vector<Index> &getIndexBuffer() const { return ibuf; }

    Mesh build() const;
    
private:
    unsigned int vert_size;
    Index next_index;

    std::vector<float> buf;
    std::vector<Index> ibuf;

    MeshFormat format;
};

#endif
