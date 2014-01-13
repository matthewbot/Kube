#include "Face.h"
#include <glm/gtx/string_cast.hpp>
#include <gtest/gtest.h>
#include <iostream>

TEST(SharedFace, None) {
    EXPECT_EQ(boost::none, sharedFace(glm::ivec3{0, 0, 0}, glm::ivec3{2, 0, 0}));
    EXPECT_EQ(boost::none, sharedFace(glm::ivec3{0, 0, 0}, glm::ivec3{1, 1, 0}));
    EXPECT_EQ(boost::none, sharedFace(glm::ivec3{1, 0, 0}, glm::ivec3{1, 1, 1}));
}

static const glm::ivec3 offsets[] = {
    glm::ivec3{0, 0, 0},
    glm::ivec3{1, 2, 3},
    glm::ivec3{-8, 4, 7}};

TEST(SharedFace, FaceNormals) {

    for (auto face : all_faces) {
        auto normal = faceNormal(face);
        SCOPED_TRACE(glm::to_string(normal));
        for (auto &offset : offsets) {
            SCOPED_TRACE(glm::to_string(offset));
            EXPECT_EQ(face, *sharedFace(offset, offset+normal));
        }
    }
}

TEST(AdjacentPos, Simple) {
    for (auto face : all_faces) {
        auto normal = faceNormal(face);
        for (auto &offset : offsets) {
            EXPECT_EQ(offset + normal, adjacentPos(offset, face));
        }
    }
}

TEST(FaceMap, Simple) {
    FaceMap<int> fmap;
    fmap.fill(0);
    fmap[Face::LEFT] = 1;
    fmap[Face::RIGHT] = 2;
    EXPECT_EQ(1, fmap[Face::LEFT]);
    EXPECT_EQ(2, fmap[Face::RIGHT]);
    EXPECT_EQ(0, fmap[Face::BOTTOM]);
    EXPECT_EQ(0, fmap[Face::BACK]);
}
