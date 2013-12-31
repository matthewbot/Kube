#ifndef TESSELATE_H
#define TESSELATE_H

#include "gfx/Mesh.h"
#include <string>

class Chunk;
void tesselate(MeshBuilder &builder, const Chunk &chunk);

class Font;
void tesselate(MeshBuilder &builder, const Font &font, const std::string &str);

#endif
