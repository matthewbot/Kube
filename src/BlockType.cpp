#include "BlockType.h"

void BlockTypeInfo::setAllFaceTextureNums(unsigned int tex) {
    for (auto face : all_faces)
        face_texes[face] = tex;
}
