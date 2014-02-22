#ifndef TESSELATE_H
#define TESSELATE_H

#include "gfx/Mesh.h"
#include <string>

// TODO back to Font
class Font;
void tesselate(MeshBuilder &builder, const Font &font, const std::string &str);

#endif
