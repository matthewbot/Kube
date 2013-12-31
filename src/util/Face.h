#ifndef FACE_H
#define FACE_H

#include <array>
#include <boost/optional.hpp>
#include <glm/glm.hpp>

enum class Face { RIGHT, LEFT, BACK, FRONT, TOP, BOTTOM };
extern const std::array<Face, 6> all_faces;
extern const std::array<glm::vec3, 6> face_normals;

boost::optional<Face> sharedFace(const glm::ivec3 &a, const glm::ivec3 &b);
glm::ivec3 adjacentPos(const glm::ivec3 &pos, Face face);
inline const glm::vec3 &faceNormal(Face face) {
    return face_normals[static_cast<int>(face)];
}

template <typename T>
class FaceMap {
public:
    T &operator[](Face face) { return data[static_cast<int>(face)]; }
    const T &operator[](Face face) const { return data[static_cast<int>(face)]; };

    void fill(const T &t) { data.fill(t); }
    
private:
    std::array<T, 6> data;
};


#endif
