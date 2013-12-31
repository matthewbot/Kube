#ifndef FONT_H
#define FONT_H

#include "gfx/Mesh.h"
#include "gfx/Texture.h"

#include <boost/variant.hpp>
#include <string>
#include <unordered_map>

class Font {
public:
    Font();
    explicit Font(const std::string &filename) { load(filename); }

    void load(const std::string &filename);

    struct CharProps {
        unsigned int x;
        unsigned int y;
        unsigned int width;
        unsigned int height;
        int xoffset;
        int yoffset;
        int xadvance;
    };

    const CharProps *getProps(unsigned int ch) const;
    const Texture &getTexture() const { return tex; }
    unsigned int getLineHeight() const { return line_height; }
    unsigned int getBaseHeight() const { return base_height; }
    
private:
    std::unordered_map<unsigned int, CharProps> charprops;
    Texture tex;
    unsigned int line_height;
    unsigned int base_height;
    
    struct ParsedLine {
        std::string type;
        std::unordered_map<std::string, std::string> keyvals;
    };
    static void parseLine(const std::string &line, ParsedLine &result);
};

#endif
