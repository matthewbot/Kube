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
        auto &kv = parsed.keyvals;

        if (parsed.type == "char") {
            unsigned int id = boost::lexical_cast<unsigned int>(kv["id"]);
            auto &prop = charprops[id];
            
            prop.x = boost::lexical_cast<unsigned int>(kv["x"]);
            prop.y = boost::lexical_cast<unsigned int>(kv["y"]);
            prop.width = boost::lexical_cast<unsigned int>(kv["width"]);
            prop.height = boost::lexical_cast<unsigned int>(kv["height"]);
            prop.xoffset = boost::lexical_cast<int>(kv["xoffset"]);
            prop.yoffset = boost::lexical_cast<int>(kv["yoffset"]);
            prop.xadvance = boost::lexical_cast<int>(kv["xadvance"]);
        } else if (parsed.type == "page") {
            tex.setImage(Image::loadPNG(kv["file"]));
        } else if (parsed.type == "common") {
            line_height = boost::lexical_cast<unsigned int>(kv["lineHeight"]);
            base_height = boost::lexical_cast<unsigned int>(kv["base"]);
        }
    }
}

const Font::CharProps *Font::getProps(unsigned int ch) const {
    auto iter = charprops.find(ch);
    if (iter == std::end(charprops)) { // TODO utility func
        return nullptr;
    } else {
        return &iter->second;
    }
}

void Font::parseLine(const std::string &line, ParsedLine &result) {
    size_t pos = line.find(' ');
    result.type = std::move(line.substr(0, pos));

    result.keyvals.clear();
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
        
        result.keyvals[keystr] = std::move(valuestr);
        pos = value_end;
    }
}
