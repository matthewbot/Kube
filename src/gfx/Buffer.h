#ifndef TEST_BUFFER_H
#define TEST_BUFFER_H

#include "util/IDBase.h"

#include <vector>
#include <cstdlib>
#include <utility>

class Buffer : public IDBase<Buffer> {
    friend class IDBase<Buffer>;
public:
    enum Type {
        ARRAY,
        ELEMENTS
    };

    Buffer() { }
    Buffer(const void *data, size_t len, Type type=ARRAY) { setData(data, len, type); }

    template <typename T>
    explicit Buffer(const std::vector<T> &vec, Type type=ARRAY) :
        Buffer(&vec.front(), vec.size()*sizeof(T), type) { }

    void setData(const void *data, size_t len, Type type=ARRAY);

private:
    void deleteId();
};

#endif
