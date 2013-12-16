#ifndef MATH_H
#define MATH_H

#include <cmath>
#include <functional>
#include <glm/glm.hpp>

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
