#ifndef UTIL_H
#define UTIL_H

#include <glm/glm.hpp>

#include <boost/optional.hpp>
#include <utility>
#include <cmath>

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

enum class Face { RIGHT, LEFT, BACK, FRONT, TOP, BOTTOM};
extern const std::array<Face, 6> all_faces;

boost::optional<Face> sharedFace(const glm::ivec3 &a, const glm::ivec3 &b);
glm::ivec3 adjacentPos(const glm::ivec3 &pos, Face face);

template <typename T>
class FaceMap {
public:
    T &operator[](Face face) { return data[static_cast<int>(face)]; }
    const T &operator[](Face face) const { return data[static_cast<int>(face)]; };

private:
    std::array<T, 6> data;
};

namespace std {
    template <typename T> struct hash<glm::detail::tvec3<T>> {
        size_t operator()(const glm::detail::tvec3<T> &vec) const {
            std::hash<T> hashT;
            size_t x = hashT(vec.x);
            size_t y = hashT(vec.y);
            size_t z = hashT(vec.z);
            return x + (y<<1) + (z<<2);
        }
    };
}

template <typename T>
glm::detail::tvec3<T> floorVec(const glm::detail::tvec3<T> &in) {
    return glm::detail::tvec3<T>{
        std::floor(in.x),
        std::floor(in.y),
        std::floor(in.z)};
}

#endif
