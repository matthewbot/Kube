#ifndef BLOCKTYPE_H
#define BLOCKTYPE_H

#include "util/Face.h"
#include <string>
#include <cstdint>
#include <unordered_map>

class BlockTypeInfo {
public:
    void setFaceTextureNum(Face face, unsigned int texnum) { face_texes[face] = texnum; }
    void setAllFaceTextureNums(unsigned int texnum);

    unsigned int getFaceTextureNum(Face face) const { return face_texes[face]; }

private:
    FaceMap<unsigned int> face_texes;
};

class BlockType {
public:
    using ID = uint16_t;

    BlockType(ID id, std::string name, BlockTypeInfo info) :
        id(id),
        name(std::move(name)),
        info(std::move(info)) { }
    
    ID getID() const { return id; }
    const std::string &getName() const { return name; }
    const BlockTypeInfo &getInfo() const { return info; }

private:
    ID id;
    std::string name;
    BlockTypeInfo info;
};

#endif
