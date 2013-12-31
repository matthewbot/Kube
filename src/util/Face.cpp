#include "Face.h"

const std::array<Face, 6> all_faces = {
    Face::RIGHT, Face::LEFT, Face::BACK, Face::FRONT, Face::TOP, Face::BOTTOM
};

const std::array<glm::vec3, 6> face_normals = {
    glm::vec3{1, 0, 0}, glm::vec3{-1, 0, 0},
    glm::vec3{0, 1, 0}, glm::vec3{0, -1, 0},
    glm::vec3{0, 0, 1}, glm::vec3{0, 0, -1}
};

boost::optional<Face> sharedFace(const glm::ivec3 &a, const glm::ivec3 &b) {
    glm::ivec3 d = a - b;

    int zeros = (d.x == 0) + (d.y == 0) + (d.z == 0);
    if (zeros != 2) {
        return boost::none;
    }

    if (d.x == -1) {
        return Face::LEFT;
    } else if (d.x == 1) {
        return Face::RIGHT;
    } else if (d.y == -1) {
        return Face::BACK;
    } else if (d.y == 1) {
        return Face::FRONT;
    } else if (d.z == -1) {
        return Face::BOTTOM;
    } else if (d.z == 1) {
        return Face::TOP;
    }

    return boost::none;
}

glm::ivec3 adjacentPos(const glm::ivec3 &pos, Face face) {
    switch (face) {
    case Face::RIGHT:
        return glm::ivec3{pos.x+1, pos.y, pos.z};
    case Face::LEFT:
        return glm::ivec3{pos.x-1, pos.y, pos.z};
    case Face::BACK:
        return glm::ivec3{pos.x, pos.y+1, pos.z};
    case Face::FRONT:
        return glm::ivec3{pos.x, pos.y-1, pos.z};
    case Face::TOP:
        return glm::ivec3{pos.x, pos.y, pos.z+1};
    case Face::BOTTOM:
        return glm::ivec3{pos.x, pos.y, pos.z-1};
    default:
        return glm::ivec3{pos.x, pos.y, pos.z};
    }
}
