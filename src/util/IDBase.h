#ifndef IDBASE_H
#define IDBASE_H

#include <utility>

template <typename Self>
class IDBase {
public:
    IDBase() { }
    explicit IDBase(unsigned int id) : id(id) { }
    IDBase(const IDBase<Self> &) = delete;
    IDBase(IDBase<Self> &&other) { *this = std::move(other); }
    ~IDBase() { callDeleteId(); }

    IDBase<Self> &operator=(const IDBase<Self> &) = delete;
    IDBase<Self> &operator=(IDBase<Self> &&other) {
        callDeleteId();
        id = other.id;
        other.id = 0;
        return *this;
    }

    unsigned int getID() const { return id; }
    explicit operator bool() const { return id > 0; }

protected:
    unsigned int id = 0;

private:
    void callDeleteId() {
        if (id > 0) {
            static_cast<Self *>(this)->deleteId();
            id = 0;
        }
    }
};

#endif
