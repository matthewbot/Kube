#ifndef FACE_H
#define FACE_H

#include <array>
#include <boost/optional.hpp>
#include <glm/glm.hpp>

enum class Face { RIGHT, LEFT, BACK, FRONT, TOP, BOTTOM };
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


#endif
