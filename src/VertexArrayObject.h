#ifndef TEST_VERTEXARRAY_H
#define TEST_VERTEXARRAY_H

#include "util.h"

class VertexArrayObject : public IDBase<VertexArrayObject> {
    friend class IDBase<VertexArrayObject>;
public:
    VertexArrayObject() { }
    static VertexArrayObject generate();

private:
    VertexArrayObject(unsigned int id) : IDBase<VertexArrayObject>(id) { }

    void deleteId();
};

#endif
