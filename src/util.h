#ifndef UTIL_H
#define UTIL_H

#include <utility>
#include <cmath>

template <typename Self>
class IDBase {
public:
    IDBase() : id(0) { }
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

protected:
    unsigned int id;

private:
    void callDeleteId() {
        if (id > 0) {
            static_cast<Self *>(this)->deleteId();
            id = 0;
        }
    }
};

template <typename T>
class Radians {
public:
    Radians() : val(0) { }
    Radians(T t) : val(t) { wrap(); }

    operator T() const { return val; }

private:
    void wrap() {
        val = std::fmod(val + M_PI, 2*M_PI) - M_PI;
    }

    T val;
};

#endif
