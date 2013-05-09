#include "Block.h"

BlockType::BlockType() :
    face_texes{0, 0, 0, 0, 0, 0} { }

void BlockType::setAllFaceTextureNums(unsigned int tex) {
    for (int i=0; i<6; i++)
        face_texes[i] = tex;
}
