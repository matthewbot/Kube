#include "Block.h"

BlockType::BlockType() { }

void BlockType::setAllFaceTextureNums(unsigned int tex) {
    for (auto face : all_faces)
        face_texes[face] = tex;
}
