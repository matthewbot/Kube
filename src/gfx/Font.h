#ifndef FONT_H
#define FONT_H

#include "gfx/Mesh.h"
#include "gfx/Texture.h"

#include <boost/variant.hpp>
#include <string>
#include <unordered_map>

class Font {
public:
    explicit Font(const std::string &filename);

    const Texture &getTexture() const { return tex; }
    Mesh tesselate(const std::string &str) const;
    
private:
    struct CharProp {
        unsigned int x;
        unsigned int y;
        unsigned int width;
        unsigned int height;
        unsigned int xoffset;
        unsigned int yoffset;
        unsigned int xadvance;
    };

    std::unordered_map<unsigned int, CharProp> charprops;
    Texture tex;
    unsigned int line_height;
    unsigned int base_height;
    
    using ParsedLine =
        std::unordered_map<std::string, std::string>;
    static void parseLine(const std::string &line, ParsedLine &result);
};

#endif
