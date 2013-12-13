#include "gfx/Font.h"
#include "gfx/Image.h"

#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>

Font::Font() : line_height(0), base_height(0) { }

void Font::load(const std::string &filename) {
    charprops.clear();
    
    std::ifstream in(filename);
    
    std::string line;
    ParsedLine parsed;
    while (getline(in, line)) {
        parseLine(line, parsed);

        const std::string &type = parsed[""];
        if (type == "char") {
            unsigned int id = boost::lexical_cast<unsigned int>(parsed["id"]);
            CharProp &prop = charprops[id];
            
            prop.x = boost::lexical_cast<unsigned int>(parsed["x"]);
            prop.y = boost::lexical_cast<unsigned int>(parsed["y"]);
            prop.width = boost::lexical_cast<unsigned int>(parsed["width"]);
            prop.height = boost::lexical_cast<unsigned int>(parsed["height"]);
            prop.xoffset = boost::lexical_cast<int>(parsed["xoffset"]);
            prop.yoffset = boost::lexical_cast<int>(parsed["yoffset"]);
            prop.xadvance = boost::lexical_cast<int>(parsed["xadvance"]);
        } else if (type == "page") {
            tex.setImage(Image::loadPNG(parsed["file"]));
        } else if (type == "common") {
            line_height = boost::lexical_cast<unsigned int>(parsed["lineHeight"]);
            base_height = boost::lexical_cast<unsigned int>(parsed["base"]);
        }
    }
}

void Font::parseLine(const std::string &line, ParsedLine &result) {
    result.clear();

    size_t pos = line.find(' ');
    result[""] = line.substr(0, pos);

    while (true) {
        size_t key_start = line.find_first_not_of(" \"\r", pos);
        if (key_start == std::string::npos)
            break;
        size_t key_end = line.find('=', pos);

        size_t value_start = line.find_first_not_of("=\"", key_end);
        size_t value_end = line.find_first_of(" \"\r", value_start);
        if (value_end == std::string::npos)
            value_end = line.size();

        std::string valuestr = line.substr(value_start, value_end - value_start);
        std::string keystr = line.substr(key_start, key_end - key_start);
        
        result[keystr] = std::move(valuestr);
        pos = value_end;
    }
}

Mesh Font::tesselate(const std::string &str) const {
    MeshBuilder builder{MeshFormat{2, 2}};

    const float texwidth = tex.getWidth();
    const float texheight = tex.getHeight();

    auto vert = [&builder](float x, float y, float tx, float ty) -> MeshBuilder::Index {
        builder.beginVert();
        builder.append(glm::vec2{x, y});
        builder.append(glm::vec2{tx, ty});
        return builder.endVert();
    };

    int curx = 0;
    for (size_t i = 0; i < str.size(); i++) {
        auto it = charprops.find(str[i]);
        if (it == charprops.end())
            continue;
        const CharProp &prop = it->second;

        const float xmin = curx + prop.xoffset;
        const float xmax = xmin + prop.width;
        const float ymax = base_height - prop.yoffset;
        const float ymin = ymax - prop.height;
        const float txmin = prop.x / texwidth;
        const float txmax = txmin + prop.width / texwidth;
        const float tymax = 1 - prop.y / texheight;
        const float tymin = tymax - prop.height / texheight;

        vert(xmin, ymin, txmin, tymin);
        MeshBuilder::Index a = vert(xmax, ymin, txmax, tymin);
        MeshBuilder::Index b = vert(xmin, ymax, txmin, tymax);
        builder.repeatVert(a);
        vert(xmax, ymax, txmax, tymax);
        builder.repeatVert(b);

        curx += prop.xadvance;
    }

    return builder.build();
}
